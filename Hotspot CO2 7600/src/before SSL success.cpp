// #include <Arduino.h>

// // LilyGO T-SIM7600G-H Pin Definitions
// #define MODEM_TX      27
// #define MODEM_RX      26
// #define MODEM_PWRKEY  4
// #define MODEM_DTR     32
// #define MODEM_RI      33
// #define MODEM_FLIGHT  25
// #define MODEM_STATUS  34

// #define SerialAT Serial1

// // --- CONFIGURATION ---
// const String APN = "e-ideas"; // e.g., "internet" or "wholesale"
// const String GAS_URL = 

// "https://script.google.com/macros/s/AKfycbyCrH6vVaznfHf4bLDbJxBdZjPP7FsokRXKmH5J4fwS1Ypp-JgbEafOhqvAiNBhBeNE/exec";

// // Helper function to send AT commands and wait for a response
// String sendAT(String command, uint32_t timeout = 2000, String expected1 = "OK", String expected2 = "ERROR") {
//     Serial.print("\n---> ");
//     Serial.println(command);
//     SerialAT.println(command);
    
//     String response = "";
//     uint32_t start = millis();
    
//     while (millis() - start < timeout) {
//         while (SerialAT.available()) {
//             char c = SerialAT.read();
//             response += c;
//         }
//         // Break early if we find our expected success or failure strings
//         if (expected1 != "" && response.indexOf(expected1) != -1) break;
//         if (expected2 != "" && response.indexOf(expected2) != -1) break;
//     }
    
//     Serial.print("<--- ");
//     Serial.println(response);
//     return response;
// }

// void printBanner(String text) {
//     Serial.println("\n========================================");
//     Serial.println(text);
//     Serial.println("========================================");
// }

// void setup() {
//     Serial.begin(115200);
//     delay(1000);
    
//     printBanner("[1] ESP32 Running");
//     Serial.println("Initializing LilyGO T-SIM7600G-H pins...");
    
//     // Modem Power-on sequence
//     pinMode(MODEM_PWRKEY, OUTPUT);
//     digitalWrite(MODEM_PWRKEY, HIGH);
//     delay(300);
//     digitalWrite(MODEM_PWRKEY, LOW);
    
//     Serial.println("Waiting 10 seconds for modem to boot...");
//     delay(10000); 

//     SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);

//     printBanner("[2] Modem Communication");
//     sendAT("AT");
//     sendAT("ATE0"); // Turn off echo for cleaner logs

//     printBanner("[3] SIM Card Detection");
//     sendAT("AT+CPIN?", 3000, "READY");

//     printBanner("[4] Signal Strength");
//     sendAT("AT+CSQ");

//     printBanner("[5] Network Registration");
//     // 0,1 means registered home network, 0,5 means registered roaming
//     sendAT("AT+CEREG?", 3000); 

//     printBanner("[6] Packet Attach");
//     sendAT("AT+CGATT?", 3000);

//     printBanner("[7] APN Connection (PDP Context)");
//     sendAT("AT+CGDCONT=1,\"IP\",\"" + APN + "\"", 3000);
//     sendAT("AT+CGACT=1,1", 5000); // Activate PDP context

//     printBanner("[8] IP Address");
//     sendAT("AT+CGPADDR=1", 3000); 

//     printBanner("[9] DNS Resolution");
//     // Resolving just to prove DNS works, though HTTP stack will do this natively
//     sendAT("AT+CDNSGIP=\"script.google.com\"", 10000, "+CDNSGIP: 1");

//     // ----------------------------------------------------------------
//     // Transitioning from raw TCP (CAOPEN) to the built-in HTTP Stack
//     // ----------------------------------------------------------------

//     printBanner("[10] Initialize HTTP Stack");
//     sendAT("AT+HTTPINIT", 3000);

//     printBanner("[11] HTTPS Configuration & URL");
//     // Set the target URL
//     sendAT("AT+HTTPPARA=\"URL\",\"" + GAS_URL + "\"", 3000);
//     // CRITICAL for Google Apps Script: Tell modem to follow 302 Redirects!
//     sendAT("AT+HTTPPARA=\"REDIR\",1", 3000);

//     printBanner("[12] Google Apps Script Execution");
//     Serial.println("Sending HTTP GET... (This can take up to 20 seconds due to redirects)");
//     // 0 = GET request. Wait up to 30 seconds for the +HTTPACTION response
//     sendAT("AT+HTTPACTION=0", 30000, "+HTTPACTION:");

//     printBanner("[13] Read Google Sheet Response");
//     // Reads the body of the response. If status was 200, you should see your Apps Script output here.
//     sendAT("AT+HTTPREAD", 10000);

//     // Cleanup
//     sendAT("AT+HTTPTERM");
//     Serial.println("\n--- Diagnostics Complete ---");
// }

// void loop() {
//     // Pass-through for manual AT commands after diagnostic finishes
//     if (Serial.available()) {
//         SerialAT.write(Serial.read());
//     }
//     if (SerialAT.available()) {
//         Serial.write(SerialAT.read());
//     }
// }