#include <Arduino.h>
#include <TinyGsmClient.h>

// ================= USER CONFIGURATION =================
const char APN[]       = "e-ideas";       // e.g., "internet", "shntel"
const char GPRS_USER[] = "";               // Leave empty if not required
const char GPRS_PASS[] = "";               // Leave empty if not required

// Google Apps Script Deployment ID (e.g., AKfycbx...)
const char GAS_SCRIPT_ID[] = "AKfycbyCrH6vVaznfHf4bLDbJxBdZjPP7FsokRXKmH5J4fwS1Ypp-JgbEafOhqvAiNBhBeNE"; 
// ======================================================

// LilyGO T-SIM7670G (ESP32-S3) Pin Definitions
#define BOARD_POWERON_PIN 12
#define MODEM_PWRKEY_PIN  4
#define MODEM_RESET_PIN   5
#define MODEM_TX_PIN      11
#define MODEM_RX_PIN      10

#define SerialAT Serial1

TinyGsm modem(SerialAT);

const char GOOGLE_HOST[] = "script.google.com";
const int  GOOGLE_PORT = 443;

void logStage(int stage, const char* name, bool success, const char* details = "") {
    Serial.printf("[%02d] %-25s : %s", stage, name, success ? "[PASS]" : "[FAIL]");
    if (strlen(details) > 0) {
        Serial.printf(" (%s)", details);
    }
    Serial.println();
}

void powerOnModem() {
    pinMode(BOARD_POWERON_PIN, OUTPUT);
    digitalWrite(BOARD_POWERON_PIN, HIGH);

    pinMode(MODEM_PWRKEY_PIN, OUTPUT);
    digitalWrite(MODEM_PWRKEY_PIN, LOW);
    delay(100);
    digitalWrite(MODEM_PWRKEY_PIN, HIGH);
    delay(1000);
    digitalWrite(MODEM_PWRKEY_PIN, LOW);
    delay(3000);
}

void runNetworkDiagnostics() {
    Serial.println("\n==========================================");
    Serial.println("  ESP32-S3 T-SIM7670G DIAGNOSTICS SUITE   ");
    Serial.println("==========================================");

    // [1] ESP32 Running
    uint32_t freeHeap = ESP.getFreeHeap();
    char heapStr[32];
    snprintf(heapStr, sizeof(heapStr), "Free Heap: %u bytes", freeHeap);
    logStage(1, "ESP32 Running", freeHeap > 10000, heapStr);

    // [2] Modem Communication
    powerOnModem();
    SerialAT.begin(115200, SERIAL_8N1, MODEM_RX_PIN, MODEM_TX_PIN);
    delay(1000);
    
    bool modemOk = modem.testAT(5000);
    logStage(2, "Modem Communication", modemOk, modemOk ? "AT Sync OK" : "No AT response");
    if (!modemOk) return;

    // [3] SIM Card Detection
    SimStatus simStatus = modem.getSimStatus();
    bool simReady = (simStatus == SIM_READY);
    logStage(3, "SIM Card Detection", simReady, simReady ? "SIM Ready" : "Check SIM/PIN");
    if (!simReady) return;

    // [4] Signal Strength
    int16_t csq = modem.getSignalQuality();
    char csqStr[32];
    snprintf(csqStr, sizeof(csqStr), "CSQ: %d/31", csq);
    logStage(4, "Signal Strength", csq > 0 && csq != 99, csqStr);
    if (csq == 0 || csq == 99) return;

    // [5] Network Registration
    RegStatus regStatus = modem.getRegistrationStatus();
    bool isRegistered = (regStatus == REG_OK_HOME || regStatus == REG_OK_ROAMING);
    char regStr[32];
    snprintf(regStr, sizeof(regStr), "Reg State: %d", regStatus);
    logStage(5, "Network Registration", isRegistered, regStr);
    if (!isRegistered) return;

    // [6] Packet Attach
    bool isAttached = modem.isGprsConnected();
    if (!isAttached) {
        modem.sendAT(GF("+CGATT=1"));
        modem.waitResponse(5000);
        isAttached = modem.isGprsConnected();
    }
    logStage(6, "Packet Attach", isAttached, isAttached ? "GPRS/LTE Attached" : "Packet attach failed");

    // [7] APN Connection
    bool gprsConnected = modem.gprsConnect(APN, GPRS_USER, GPRS_PASS);
    logStage(7, "APN Connection", gprsConnected, gprsConnected ? APN : "APN Auth Failed");
    if (!gprsConnected) return;

    // [8] IP Address
    IPAddress localIP = modem.localIP();
    char ipStr[32];
    snprintf(ipStr, sizeof(ipStr), "IP: %s", localIP.toString().c_str());
    logStage(8, "IP Address", localIP != IPAddress(0,0,0,0), ipStr);

    // [9] TCP Connection (Proves DNS and IP routing work natively)
    TinyGsmClient rawClient(modem);
    bool tcpOk = rawClient.connect(GOOGLE_HOST, GOOGLE_PORT);
    logStage(9, "TCP Routing", tcpOk, tcpOk ? "Port 443 TCP Open" : "TCP Handshake Failed");
    rawClient.stop();
    if (!tcpOk) return;

    // [10] Google Apps Script via Native SIM7600 HTTPS
    Serial.println("\n--- Initiating Native SIM7600 HTTPS GET ---");
    
    modem.sendAT(GF("+HTTPINIT"));
    modem.waitResponse(5000);

    // Force TLS 1.2 required by Google
    modem.sendAT(GF("+CSSLCFG=\"sslversion\",1,3"));
    modem.waitResponse();
    modem.sendAT(GF("+HTTPPARA=\"SSLCFG\",1"));
    modem.waitResponse();

    // Set URL
    String url = "https://script.google.com/macros/s/" + String(GAS_SCRIPT_ID) + "/exec?test=1";
    modem.sendAT(GF("+HTTPPARA=\"URL\",\""), url, GF("\""));
    modem.waitResponse(5000);

    // Some SIM7600 firmwares natively support redirect following
    modem.sendAT(GF("+HTTPPARA=\"REDIR\",1"));
    modem.waitResponse();

    // Execute GET Request
    modem.sendAT(GF("+HTTPACTION=0"));
    
    int httpStatus = 0;
    int dataLen = 0;
    
    // Wait up to 30s for the HTTPACTION callback
    if (modem.waitResponse(30000, GF("+HTTPACTION: 0,")) == 1) {
        httpStatus = modem.stream.parseInt();
        modem.stream.readStringUntil(','); // Skip comma
        dataLen = modem.stream.parseInt();
        
        char httpResult[64];
        snprintf(httpResult, sizeof(httpResult), "HTTP %d (Len: %d)", httpStatus, dataLen);
        
        // 200 OK or 302 Redirect both prove Google received the request
        logStage(10, "Google Script Reachable", (httpStatus == 200 || httpStatus == 302), httpResult);

        if (httpStatus == 302) {
            Serial.println("     --> 302 Redirect Received. Google authenticated the script URL successfully!");
        } else if (httpStatus == 200 && dataLen > 0) {
            // Read the body if HTTP 200 OK
            modem.sendAT(GF("+HTTPREAD=0,"), dataLen);
            if (modem.waitResponse(10000, GF("+HTTPREAD:")) == 1) {
                int readLen = modem.stream.parseInt();
                String responseBody = modem.stream.readString();
                Serial.printf("     --> Body: %s\n", responseBody.c_str());
            }
        }
    } else {
        logStage(10, "Google Script Reachable", false, "Timeout waiting for HTTPS response");
    }

    // Terminate HTTP service
    modem.sendAT(GF("+HTTPTERM"));
    modem.waitResponse(5000);

    modem.gprsDisconnect();
    Serial.println("==========================================\n");
}

void setup() {
    Serial.begin(115200);

    // Wait up to 5 seconds for the native USB Serial terminal to attach
    uint32_t startWait = millis();
    while (!Serial && (millis() - startWait < 5000)) {
        delay(10);
    }

    delay(1000);
    Serial.println("\n[SYS] USB Serial Connected. Starting Diagnostics...");
    runNetworkDiagnostics();
}

void loop() {
    delay(1000);
}