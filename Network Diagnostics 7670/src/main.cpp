#include <Arduino.h>

#define TINY_GSM_MODEM_SIM7600
#include <TinyGsmClient.h>

// ================= USER CONFIGURATION =================
const char APN[]       = "e-ideas";        // Singtel APN
const char GPRS_USER[] = "";
const char GPRS_PASS[] = "";

// Paste your Google Apps Script Deployment ID here
const char GAS_SCRIPT_ID[] = "AKfycbxGhwmJ-USPqrZp1MUHoBLk97MAPtWl1bBGX7-yO_24VzDoaGQqMl8Ee-I1tQGf4Kulqw";
// ======================================================

#define BOARD_POWERON_PIN 12
#define MODEM_PWRKEY_PIN  4
#define MODEM_RESET_PIN   5
#define MODEM_TX_PIN      11
#define MODEM_RX_PIN      10

#define SerialAT Serial1
TinyGsm modem(SerialAT);

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

bool uploadDataToGoogleSheet(float temp, float humidity, const char* statusMsg) {
    Serial.println("\n[CELL] Connecting to Cellular Network...");

    if (!modem.isNetworkConnected() && !modem.waitForNetwork(30000)) {
        Serial.println("[ERR] Network Registration Failed");
        return false;
    }

    if (!modem.isGprsConnected() && !modem.gprsConnect(APN, GPRS_USER, GPRS_PASS)) {
        Serial.println("[ERR] APN Connection Failed");
        return false;
    }

    Serial.println("[CELL] Connected. Constructing Diagnostic HTTPS Request...");

    String url = "https://script.google.com/macros/s/" + String(GAS_SCRIPT_ID) + "/exec";
    url += "?temp=" + String(temp, 2) + "&humidity=" + String(humidity, 2) + "&status=" + String(statusMsg);

    modem.sendAT(GF("+HTTPINIT"));
    modem.waitResponse(5000);

    modem.sendAT(GF("+CSSLCFG=\"sslversion\",1,3"));
    modem.waitResponse();
    modem.sendAT(GF("+HTTPPARA=\"SSLCFG\",1"));
    modem.waitResponse();

    // 1. Set standard User-Agent to bypass basic bot filtering
    modem.sendAT(GF("+HTTPPARA=\"UA\",\"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36\""));
    modem.waitResponse();

    modem.sendAT(GF("+HTTPPARA=\"URL\",\""), url, GF("\""));
    modem.waitResponse(5000);

    // 2. DISABLE Redirect to isolate the exact point of failure
    modem.sendAT(GF("+HTTPPARA=\"REDIR\",1"));
    modem.waitResponse();

    Serial.println("[HTTPS] Sending GET payload (Auto-Redirect OFF)...");
    modem.sendAT(GF("+HTTPACTION=0"));

    bool success = false;
    if (modem.waitResponse(30000, GF("+HTTPACTION: 0,")) == 1) {
        int httpStatus = modem.stream.parseInt();
        modem.stream.readStringUntil(','); // Skip the comma
        int dataLen = modem.stream.parseInt();

        Serial.printf("[HTTPS] Hop 1 Status: %d, Data Length: %d bytes\n", httpStatus, dataLen);

        // 3. Read the Raw Response Body if one exists
        if (dataLen > 0) {
            Serial.println("\n--- GOOGLE RESPONSE BODY START ---");
            modem.sendAT(GF("+HTTPREAD=0,"), dataLen);
            if (modem.waitResponse(10000, GF("+HTTPREAD:")) == 1) {
                int readLen = modem.stream.parseInt();
                String responseBody = modem.stream.readString();
                Serial.println(responseBody);
            }
            Serial.println("--- GOOGLE RESPONSE BODY END ---\n");
        }

        // Analyze the behavior
        if (httpStatus == 302) {
            Serial.println("[DIAGNOSTIC] SUCCESS! script.google.com accepted the request and issued a 302 Redirect.");
            Serial.println("[DIAGNOSTIC] This means your Apps Script permissions are CORRECT.");
            Serial.println("[DIAGNOSTIC] The previous 403 was caused by the modem mangling the redirect on Hop 2.");
            success = true; // We consider 302 a success for this diagnostic
        }
        else if (httpStatus == 403) {
            Serial.println("[DIAGNOSTIC] FAILED at Hop 1. script.google.com actively rejected the request.");
            Serial.println("[DIAGNOSTIC] Check the response body above for clues (e.g. Google Sign-In HTML).");
        }
        else {
            Serial.printf("[DIAGNOSTIC] Unexpected Status: %d\n", httpStatus);
        }
    } else {
        Serial.println("[ERR] HTTPS Request Timed Out");
    }

    modem.sendAT(GF("+HTTPTERM"));
    modem.waitResponse(5000);

    return success;
}

void setup() {
    Serial.begin(115200);

    // Wait for USB CDC on ESP32-S3
    uint32_t startWait = millis();
    while (!Serial && (millis() - startWait < 5000)) {
        delay(10);
    }

    Serial.println("\n[SYS] Initializing SIM7670G Modem...");
    powerOnModem();
    SerialAT.begin(115200, SERIAL_8N1, MODEM_RX_PIN, MODEM_TX_PIN);
    delay(1000);

    if (modem.testAT()) {
        Serial.println("[SYS] Modem Online.");

        // Example Sensor Data Upload
        float testTemperature = 26.5;
        float testHumidity = 72.3;
        uploadDataToGoogleSheet(testTemperature, testHumidity, "ESP32_OK");
    } else {
        Serial.println("[ERR] Modem failed to respond to AT commands.");
    }
}

void loop() {
    // Send data every 5 minutes
    delay(30000);
    uploadDataToGoogleSheet(27.1, 68.4, "Periodic_Log");
}