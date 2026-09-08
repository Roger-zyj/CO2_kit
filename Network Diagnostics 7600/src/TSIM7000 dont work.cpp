// #include <Arduino.h>

// #define MODEM_TX        27
// #define MODEM_RX        26
// #define MODEM_PWRKEY    4
// #define MODEM_POWER_ON  12 

// #define SerialAT Serial1

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

// bool waitForNetwork(uint32_t timeoutMs = 180000) {
//     Serial.println("\n[!] Waiting for Network Registration...");
//     uint32_t start = millis();
//     while (millis() - start < timeoutMs) {
//         String reg = sendAT("AT+CGREG?", 2000);
//         String ereg = sendAT("AT+CEREG?", 2000);
        
//         if (reg.indexOf(",1") != -1 || reg.indexOf(",5") != -1 || 
//             ereg.indexOf(",1") != -1 || ereg.indexOf(",5") != -1) {
//             Serial.println("\n[+] Network Registered Successfully!");
//             return true;
//         }
//         delay(3000);
//     }
//     Serial.println("\n[X] Network Registration Timed Out!");
//     return false;
// }

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
    
//     printBanner("[1] Board & Power Setup");
//     pinMode(MODEM_POWER_ON, OUTPUT);
//     digitalWrite(MODEM_POWER_ON, HIGH);
    
//     pinMode(MODEM_PWRKEY, OUTPUT);
//     digitalWrite(MODEM_PWRKEY, LOW);
//     delay(100);
//     digitalWrite(MODEM_PWRKEY, HIGH);
//     delay(1000);
//     digitalWrite(MODEM_PWRKEY, LOW);
//     delay(4000); 

//     SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);

//     printBanner("[2] Modem Handshake & RF Band Configuration");
//     sendAT("AT");
//     sendAT("ATE0");
//     sendAT("AT+CPIN?", 3000, "READY");

//     sendAT("AT+CNMP=2", 3000); 
//     sendAT("AT+CMNB=1", 3000); 
//     sendAT("AT+CBANDCFG=\"CAT-M\",1,2,3,4,5,8,12,13,18,19,20,26,28", 3000);

//     printBanner("[3] Network Registration");
//     sendAT("AT+CSQ");
//     if (!waitForNetwork(180000)) {
//         Serial.println("\n[X] Cellular registration failed.");
//         return;
//     }

//     printBanner("[4] Clean GPRS Bearer & DNS Setup");
//     sendAT("AT+CGATT=1", 5000);
//     sendAT("AT+SAPBR=0,1", 3000); 
//     sendAT("AT+SAPBR=3,1,\"Contype\",\"GPRS\"", 3000);
//     sendAT("AT+SAPBR=3,1,\"APN\",\"" + APN + "\"", 3000);
//     sendAT("AT+SAPBR=1,1", 10000);
//     sendAT("AT+SAPBR=2,1", 3000);

//     // Assign DNS servers explicitly to resolve script.google.com
//     sendAT("AT+DNSSERVER=\"8.8.8.8\",\"8.8.4.4\"", 3000);
//     sendAT("AT+CDNSGIP=\"script.google.com\"", 5000);

//     printBanner("[5] HTTPS & SSL Setup");
//     // Configure SSL Context 0
//     sendAT("AT+CSSLCFG=\"sslversion\",0,4", 3000);
//     sendAT("AT+CSSLCFG=\"authmode\",0,0", 3000);
//     sendAT("AT+CSSLCFG=\"sni\",0,1", 3000);

//     sendAT("AT+HTTPTERM", 2000);
//     sendAT("AT+HTTPINIT", 3000);
//     sendAT("AT+HTTPPARA=\"CID\",1", 3000);
//     sendAT("AT+HTTPPARA=\"SSLCFG\",0", 3000); // Bind SSL Context 0
//     sendAT("AT+HTTPPARA=\"REDIR\",1", 3000); 

//     printBanner("[6] Request Target URL");
//     sendAT("AT+HTTPPARA=\"URL\",\"" + GAS_URL + "\"", 3000);

//     printBanner("[7] Execute HTTP GET");
//     String actionResp = sendAT("AT+HTTPACTION=0", 30000, "+HTTPACTION:");

//     if (actionResp.indexOf("+HTTPACTION: 0,302") != -1 || actionResp.indexOf("+HTTPACTION:0,302") != -1) {
//         Serial.println("\n[!] 302 Redirect detected. Fetching Location header...");
        
//         String headerResp = sendAT("AT+HTTPHEAD", 10000);
//         String redirectUrl = extractLocationHeader(headerResp);

//         if (redirectUrl.length() > 0) {
//             Serial.println("\n[!] Redirecting to: " + redirectUrl);
            
//             sendAT("AT+HTTPTERM", 2000);
//             sendAT("AT+HTTPINIT", 3000);
//             sendAT("AT+HTTPPARA=\"CID\",1", 3000);
//             sendAT("AT+HTTPPARA=\"SSLCFG\",0", 3000);
//             sendAT("AT+HTTPPARA=\"URL\",\"" + redirectUrl + "\"", 3000);
            
//             actionResp = sendAT("AT+HTTPACTION=0", 30000, "+HTTPACTION:");
//         }
//     }

//     printBanner("[8] Read HTTP Response");
//     if (actionResp.indexOf("+HTTPACTION: 0,200") != -1 || actionResp.indexOf("+HTTPACTION:0,200") != -1) {
//         sendAT("AT+HTTPREAD", 10000);
//     } else {
//         Serial.println("[X] Request finished with non-200 status.");
//     }

//     sendAT("AT+HTTPTERM");
//     Serial.println("\n--- Diagnostics Complete ---");
// }

// void loop() {
//     if (Serial.available()) SerialAT.write(Serial.read());
//     if (SerialAT.available()) Serial.write(SerialAT.read());
// }