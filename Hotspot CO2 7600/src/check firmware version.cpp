// #include <Arduino.h>
// #include <Wire.h>
// #include <Adafruit_BME680.h>

// // --- SIM7600 Modem Pins ---
// #define MODEM_TX      27
// #define MODEM_RX      26
// #define MODEM_PWRKEY  4
// #define SerialAT      Serial1

// const char* APN = "e-ideas";
// const char* scriptURL = "https://script.google.com/macros/s/AKfycbyCrH6vVaznfHf4bLDbJxBdZjPP7FsokRXKmH5J4fwS1Ypp-JgbEafOhqvAiNBhBeNE/exec";

// const int LED_PIN = 25;
// HardwareSerial K30Serial(2);

// Adafruit_BME680 bme;

// // K30 packet
// byte requestCO2[] = {
//     0x68, 0x04, 0x00, 0x03, 0x00, 0x01, 0xC8, 0xF3
// };
// byte response[7];

// int readCO2() {
//     while(K30Serial.available()) K30Serial.read();
//     K30Serial.write(requestCO2,8);
//     delay(50);
//     if(K30Serial.available() < 7) return -1;
//     for(int i=0;i<7;i++) response[i]=K30Serial.read();
//     return response[3]*256 + response[4];
// }

// // --- AT Command Helpers ---
// String sendAT(String command, uint32_t timeout = 2000, String expected1 = "OK", String expected2 = "ERROR") {
//     Serial.print("---> ");
//     Serial.println(command);
//     SerialAT.println(command);

//     String resp = "";
//     uint32_t start = millis();

//     while (millis() - start < timeout) {
//         while (SerialAT.available()) {
//             char c = SerialAT.read();
//             resp += c;
//         }
//         if (expected1 != "" && resp.indexOf(expected1) != -1) break;
//         if (expected2 != "" && resp.indexOf(expected2) != -1) break;
//     }

//     Serial.println("<--- " + resp);   // <— add this
//     return resp;
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

//     // --- Modem Power & Initialization ---
//     Serial.println("\nInitializing Cellular Modem...");
//     pinMode(MODEM_PWRKEY, OUTPUT);
//     digitalWrite(MODEM_PWRKEY, HIGH);
//     delay(300);
//     digitalWrite(MODEM_PWRKEY, LOW);
    
//     Serial.println("Waiting 10 seconds for modem to boot...");
//     delay(10000); 

//     SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);

//     sendAT("ATE1"); // Disable echo
//     sendAT("AT+CPIN?", 3000, "READY");
//     sendAT("AT+CGDCONT=1,\"IP\",\"" + String(APN) + "\"", 3000);
//     sendAT("AT+CGACT=1,1", 5000);
    
//     // SSL configuration (needs to run only once at startup)
//     sendAT("AT+CSSLCFG=\"sslversion\",0,4", 3000);
//     sendAT("AT+CSSLCFG=\"authmode\",0,0", 3000);
//     sendAT("AT+CSSLCFG=\"sni\",0,1", 3000);
    
//     Serial.println("Cellular Setup Complete!");

//     // LED check code is running
//     pinMode(LED_PIN, OUTPUT); 
//     digitalWrite(LED_PIN, HIGH);

//     Wire.begin();
//     Serial.println();
//     Serial.println("Starting...");

//     // Sensors init ------------------------------------------------------------------------
//     // if(!bme.begin())
//     // {
//     //     Serial.println("BME688 NOT FOUND");
//     //     while(1);
//     // }
//     // Serial.println("BME688 OK");

//     // // UART2 CO2
//     // K30Serial.begin(
//     //     9600,
//     //     SERIAL_8N1,
//     //     32,     // RX
//     //     33      // TX
//     // );
//     // Serial.println("K30 UART Started");
//     // Sensors init ------------------------------------------------------------------------

//     sendAT("AT+GMR", 3000);
//     sendAT("AT+SIMCOMATI", 3000);
// }

// void loop() {

//     // Delay before next cycle
//     delay(5000);
// }