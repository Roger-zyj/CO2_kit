// #include <Arduino.h>

// #define MODEM_TX      27
// #define MODEM_RX      26
// #define MODEM_PWRKEY  4

// #define SerialAT Serial1

// // --- CONFIGURATION ---
// const String APN = "e-ideas";
// const String GAS_URL = "https://script.google.com/macros/s/AKfycbyCrH6vVaznfHf4bLDbJxBdZjPP7FsokRXKmH5J4fwS1Ypp-JgbEafOhqvAiNBhBeNE/exec";

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

// // Extracts the Location header from AT+HTTPHEAD output
// String extractLocationHeader(String headers) {
//     int locIdx = headers.indexOf("Location: ");
//     if (locIdx == -1) locIdx = headers.indexOf("location: ");
//     if (locIdx == -1) return "";
    
//     int start = locIdx + 10;
//     int end = headers.indexOf("\r\n", start);
//     if (end == -1) end = headers.length();
    
//     String redirectUrl = headers.substring(start, end);
//     redirectUrl.trim();
//     return redirectUrl;
// }

// void setup() {
//     Serial.begin(115200);
//     delay(1000);
    
//     printBanner("[1] ESP32 Running");
//     pinMode(MODEM_PWRKEY, OUTPUT);
//     digitalWrite(MODEM_PWRKEY, HIGH);
//     delay(300);
//     digitalWrite(MODEM_PWRKEY, LOW);
//     delay(10000); 

//     SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);

//     printBanner("[2] Modem Communication");
//     sendAT("AT");
//     sendAT("ATE0");

//     printBanner("[3-8] Network Setup");
//     sendAT("AT+CPIN?", 3000, "READY");
//     sendAT("AT+CSQ");
//     sendAT("AT+CEREG?", 3000); 
//     sendAT("AT+CGATT?", 3000);
//     sendAT("AT+CGDCONT=1,\"IP\",\"" + APN + "\"", 3000);
//     sendAT("AT+CGACT=1,1", 5000);
//     sendAT("AT+CGPADDR=1", 3000); 

//     printBanner("[9] SSL Configuration");
//     // Configure SSL Context 0: TLS 1.2, ignore cert checks, enable SNI
//     sendAT("AT+CSSLCFG=\"sslversion\",0,4", 3000);
//     sendAT("AT+CSSLCFG=\"authmode\",0,0", 3000);
//     sendAT("AT+CSSLCFG=\"sni\",0,1", 3000);

//     printBanner("[10] Initialize HTTP Stack");
//     sendAT("AT+HTTPINIT", 3000);
//     sendAT("AT+HTTPPARA=\"SSLCFG\",0", 3000); // Attach SSL context 0
//     sendAT("AT+HTTPPARA=\"REDIR\",1", 3000);   // Enable auto-redirect

//     printBanner("[11] Set Initial URL");
//     sendAT("AT+HTTPPARA=\"URL\",\"" + GAS_URL + "\"", 3000);

//     printBanner("[12] Google Apps Script Execution");
//     String actionResp = sendAT("AT+HTTPACTION=0", 30000, "+HTTPACTION:");

//     // Check if modem received a 302 and stopped without following it
//     if (actionResp.indexOf("+HTTPACTION: 0,302") != -1) {
//         Serial.println("\n[!] 302 Redirect detected. Fetching Location header...");
        
//         // Retrieve response headers
//         String headerResp = sendAT("AT+HTTPHEAD", 10000);
//         String redirectUrl = extractLocationHeader(headerResp);

//         if (redirectUrl.length() > 0) {
//             Serial.println("\n[!] Redirecting to: " + redirectUrl);
            
//             // Re-initialize request with the redirected Google target URL
//             sendAT("AT+HTTPTERM", 2000);
//             sendAT("AT+HTTPINIT", 3000);
//             sendAT("AT+HTTPPARA=\"SSLCFG\",0", 3000);
//             sendAT("AT+HTTPPARA=\"URL\",\"" + redirectUrl + "\"", 3000);
            
//             // Re-run HTTP GET
//             actionResp = sendAT("AT+HTTPACTION=0", 30000, "+HTTPACTION:");
//         } else {
//             Serial.println("\n[X] Failed to parse Location header from response.");
//         }
//     }

//     printBanner("[13] Read Response");
//     if (actionResp.indexOf("+HTTPACTION: 0,200") != -1) {
//         sendAT("AT+HTTPREAD", 10000);
//     } else {
//         Serial.println("[X] HTTP request finished with non-200 status.");
//     }

//     sendAT("AT+HTTPTERM");
//     Serial.println("\n--- Diagnostics Complete ---");
// }

// void loop() {
//     if (Serial.available()) SerialAT.write(Serial.read());
//     if (SerialAT.available()) Serial.write(SerialAT.read());
// }