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

// // --- Cellular HTTP GET Execution ---
// void sendDataToGoogle(String fullUrl) {
//     Serial.println("\n[HTTP] Starting upload...");
    
//     // Ensure HTTP stack is closed from any previous failed runs
//     sendAT("AT+HTTPTERM", 2000); 
    
//     sendAT("AT+HTTPINIT", 3000);
//     sendAT("AT+HTTPPARA=\"SSLCFG\",0", 3000);
//     sendAT("AT+HTTPPARA=\"REDIR\",1", 3000);
//     sendAT("AT+HTTPPARA=\"URL\",\"" + fullUrl + "\"", 3000);

//     String actionResp = sendAT("AT+HTTPACTION=0", 30000, "+HTTPACTION:");

//     // Handle 302 Redirect
//     if (actionResp.indexOf("+HTTPACTION: 0,302") != -1) {
//         Serial.println("[HTTP] 302 Redirect detected. Following...");
//         String headerResp = sendAT("AT+HTTPHEAD", 10000);
//         String redirectUrl = extractLocationHeader(headerResp);

//         if (redirectUrl.length() > 0) {
//             sendAT("AT+HTTPTERM", 2000);
//             sendAT("AT+HTTPINIT", 3000);
//             sendAT("AT+HTTPPARA=\"SSLCFG\",0", 3000);
//             sendAT("AT+HTTPPARA=\"URL\",\"" + redirectUrl + "\"", 3000);
            
//             actionResp = sendAT("AT+HTTPACTION=0", 30000, "+HTTPACTION:");
//         }
//     }

//     if (actionResp.indexOf("+HTTPACTION: 0,200") != -1) {
//         Serial.println("[SUCCESS] Data sent to Google Sheets!");
//     } else {
//         Serial.println("[ERROR] Failed to send data.");
//     }

//     // Cleanup
//     sendAT("AT+HTTPTERM", 2000);
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

//     sendAT("ATE0"); // Disable echo
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
// }

// void loop() {
//     // //---------------------------------------
//     // // Read BME688
//     // //---------------------------------------
//     // if(bme.performReading())
//     // {
//     //     Serial.println("========== BME688 ==========");
//     //     Serial.print("Temperature : ");
//     //     Serial.print(bme.temperature);
//     //     Serial.println(" C");
//     //     Serial.print("Humidity    : ");
//     //     Serial.print(bme.humidity);
//     //     Serial.println(" %");
//     //     Serial.print("Pressure    : ");
//     //     Serial.print(bme.pressure/100.0);
//     //     Serial.println(" hPa");
//     //     Serial.print("Gas         : ");
//     //     Serial.print(bme.gas_resistance);
//     //     Serial.println(" Ohms");
//     // }
//     // else
//     // {
//     //     Serial.println("BME688 Read Failed");
//     // }

//     // //---------------------------------------
//     // // Read K30
//     // //---------------------------------------
//     // int co2 = readCO2();
//     // Serial.println("========== K30 ==========");
//     // if(co2 > 0)
//     // {
//     //     Serial.print("CO2 : ");
//     //     Serial.print(co2);
//     //     Serial.println(" ppm");
//     // }
//     // else
//     // {
//     //     Serial.println("No response from K30");
//     // }
//     // Serial.println();
//     // Serial.println("------------------------------");
//     // Serial.println();

    
//     // // --- Cellular + Upload REAL data ---
//     // String url = String(scriptURL)
//     //     + "?temp=" + String(bme.temperature)
//     //     + "&humidity=" + String(bme.humidity)
//     //     + "&pressure=" + String(bme.pressure / 100.0)
//     //     + "&gas=" + String(bme.gas_resistance)
//     //     + "&co2=" + String(co2);
//     // sendDataToGoogle(url);


//     // --- Cellular + Upload DUMMY data ---
//     String dummyUrl = String(scriptURL)
//         + "?temp=" + String(1)
//         + "&humidity=" + String(2)
//         + "&pressure=" + String(3)
//         + "&gas=" + String(4)
//         + "&co2=" + String(5);
        
//     sendDataToGoogle(dummyUrl);

//     // Delay before next cycle
//     delay(5000);
// }