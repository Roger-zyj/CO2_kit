// #include <Arduino.h>
// #include <Wire.h>
// #include <Adafruit_BME680.h>
// #define TINY_GSM_MODEM_SIM7600
// #include <TinyGsmClient.h>

// // ================= USER CONFIGURATION =================
// const char APN[]       = "e-ideas";        // Singtel APN
// const char GPRS_USER[] = "";
// const char GPRS_PASS[] = "";

// // Paste your Google Apps Script Deployment ID here
// const char GAS_SCRIPT_ID[] = "AKfycbxGhwmJ-USPqrZp1MUHoBLk97MAPtWl1bBGX7-yO_24VzDoaGQqMl8Ee-I1tQGf4Kulqw";
// // ======================================================

// #define BOARD_POWERON_PIN 12
// #define MODEM_PWRKEY_PIN  4
// #define MODEM_RESET_PIN   5
// #define MODEM_TX_PIN      11
// #define MODEM_RX_PIN      10

// #define SerialAT Serial1
// TinyGsm modem(SerialAT);

// void powerOnModem() {
//     pinMode(BOARD_POWERON_PIN, OUTPUT);
//     digitalWrite(BOARD_POWERON_PIN, HIGH);

//     pinMode(MODEM_PWRKEY_PIN, OUTPUT);
//     digitalWrite(MODEM_PWRKEY_PIN, LOW);
//     delay(100);
//     digitalWrite(MODEM_PWRKEY_PIN, HIGH);
//     delay(1000);
//     digitalWrite(MODEM_PWRKEY_PIN, LOW);
//     delay(3000);
// }

// bool uploadDataToGoogleSheet(float temp, float humidity, const char* statusMsg) {
//     Serial.println("\n[CELL] Connecting to Cellular Network...");

//     if (!modem.isNetworkConnected() && !modem.waitForNetwork(30000)) {
//         Serial.println("[ERR] Network Registration Failed");
//         return false;
//     }

//     if (!modem.isGprsConnected() && !modem.gprsConnect(APN, GPRS_USER, GPRS_PASS)) {
//         Serial.println("[ERR] APN Connection Failed");
//         return false;
//     }

//     Serial.println("[CELL] Connected. Constructing Diagnostic HTTPS Request...");

//     String url = "https://script.google.com/macros/s/" + String(GAS_SCRIPT_ID) + "/exec";
//     url += "?temp=" + String(temp, 2) + "&humidity=" + String(humidity, 2) + "&status=" + String(statusMsg);

//     modem.sendAT(GF("+HTTPINIT"));
//     modem.waitResponse(5000);

//     modem.sendAT(GF("+CSSLCFG=\"sslversion\",1,3"));
//     modem.waitResponse();
//     modem.sendAT(GF("+HTTPPARA=\"SSLCFG\",1"));
//     modem.waitResponse();

//     // 1. Set standard User-Agent to bypass basic bot filtering
//     modem.sendAT(GF("+HTTPPARA=\"UA\",\"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36\""));
//     modem.waitResponse();

//     modem.sendAT(GF("+HTTPPARA=\"URL\",\""), url, GF("\""));
//     modem.waitResponse(5000);

//     // 2. DISABLE Redirect to isolate the exact point of failure
//     modem.sendAT(GF("+HTTPPARA=\"REDIR\",1"));
//     modem.waitResponse();

//     Serial.println("[HTTPS] Sending GET payload (Auto-Redirect OFF)...");
//     modem.sendAT(GF("+HTTPACTION=0"));

//     bool success = false;
//     if (modem.waitResponse(30000, GF("+HTTPACTION: 0,")) == 1) {
//         int httpStatus = modem.stream.parseInt();
//         modem.stream.readStringUntil(','); // Skip the comma
//         int dataLen = modem.stream.parseInt();

//         Serial.printf("[HTTPS] Hop 1 Status: %d, Data Length: %d bytes\n", httpStatus, dataLen);

//         // 3. Read the Raw Response Body if one exists
//         if (dataLen > 0) {
//             Serial.println("\n--- GOOGLE RESPONSE BODY START ---");
//             modem.sendAT(GF("+HTTPREAD=0,"), dataLen);
//             if (modem.waitResponse(10000, GF("+HTTPREAD:")) == 1) {
//                 int readLen = modem.stream.parseInt();
//                 String responseBody = modem.stream.readString();
//                 Serial.println(responseBody);
//             }
//             Serial.println("--- GOOGLE RESPONSE BODY END ---\n");
//         }

//         // Analyze the behavior
//         if (httpStatus == 302) {
//             Serial.println("[DIAGNOSTIC] SUCCESS! script.google.com accepted the request and issued a 302 Redirect.");
//             Serial.println("[DIAGNOSTIC] This means your Apps Script permissions are CORRECT.");
//             Serial.println("[DIAGNOSTIC] The previous 403 was caused by the modem mangling the redirect on Hop 2.");
//             success = true; // We consider 302 a success for this diagnostic
//         }
//         else if (httpStatus == 403) {
//             Serial.println("[DIAGNOSTIC] FAILED at Hop 1. script.google.com actively rejected the request.");
//             Serial.println("[DIAGNOSTIC] Check the response body above for clues (e.g. Google Sign-In HTML).");
//         }
//         else {
//             Serial.printf("[DIAGNOSTIC] Unexpected Status: %d\n", httpStatus);
//         }
//     } else {
//         Serial.println("[ERR] HTTPS Request Timed Out");
//     }

//     modem.sendAT(GF("+HTTPTERM"));
//     modem.waitResponse(5000);

//     return success;
// }


// //Sensors 
// HardwareSerial K30Serial(2);

// Adafruit_BME680 bme;

// // K30 packet
// byte requestCO2[] =
// {
//     0x68,
//     0x04,
//     0x00,
//     0x03,
//     0x00,
//     0x01,
//     0xC8,
//     0xF3
// };

// byte response[7];

// int readCO2()
// {
//     while(K30Serial.available())
//         K30Serial.read();

//     K30Serial.write(requestCO2,8);

//     delay(50);

//     if(K30Serial.available() < 7)
//         return -1;

//     for(int i=0;i<7;i++)
//         response[i]=K30Serial.read();

//     int ppm = response[3]*256 + response[4];

//     return ppm;
// }

//     //Sensors init ------------------------------------------------------------------------
//     if(!bme.begin())
//     {
//         Serial.println("BME688 NOT FOUND");
//         while(1);
//     }

//     Serial.println("BME688 OK");

//     // UART2 CO2
//     K30Serial.begin(
//         9600,
//         SERIAL_8N1,
//         32,     // RX
//         33      // TX
//     );

//     Serial.println("K30 UART Started");
//     //Sensors init ------------------------------------------------------------------------
    



// void setup() {
//     Serial.begin(115200);

//     // Wait for USB CDC on ESP32-S3
//     uint32_t startWait = millis();
//     while (!Serial && (millis() - startWait < 5000)) {
//         delay(10);
//     }

//     Serial.println("\n[SYS] Initializing SIM7670G Modem...");
//     powerOnModem();
//     SerialAT.begin(115200, SERIAL_8N1, MODEM_RX_PIN, MODEM_TX_PIN);
//     delay(1000);

//     if (modem.testAT()) {
//         Serial.println("[SYS] Modem Online.");

//         // Example Sensor Data Upload
//         float testTemperature = 26.5;
//         float testHumidity = 72.3;
//         uploadDataToGoogleSheet(testTemperature, testHumidity, "ESP32_OK");
//     } else {
//         Serial.println("[ERR] Modem failed to respond to AT commands.");
//     }
// }

// void loop() {
//     // Send data every 5 minutes
//     delay(300000);

    
//     //---------------------------------------
//     // Read BME688
//     //---------------------------------------

//     if(bme.performReading())
//     {
//         Serial.println("========== BME688 ==========");

//         Serial.print("Temperature : ");
//         Serial.print(bme.temperature);
//         Serial.println(" C");

//         Serial.print("Humidity    : ");
//         Serial.print(bme.humidity);
//         Serial.println(" %");

//         Serial.print("Pressure    : ");
//         Serial.print(bme.pressure/100.0);
//         Serial.println(" hPa");

//         Serial.print("Gas         : ");
//         Serial.print(bme.gas_resistance);
//         Serial.println(" Ohms");
//     }
//     else
//     {
//         Serial.println("BME688 Read Failed");
//     }

//     //---------------------------------------
//     // Read K30
//     //---------------------------------------

//     int co2 = readCO2();

//     Serial.println("========== K30 ==========");

//     if(co2 > 0)
//     {
//         Serial.print("CO2 : ");
//         Serial.print(co2);
//         Serial.println(" ppm");
//     }
//     else
//     {
//         Serial.println("No response from K30");
//     }

//     Serial.println();
//     Serial.println("------------------------------");
//     Serial.println();

//     delay(2000);

    
//     // //Wifi + Upload data
//     // if (WiFi.status() == WL_CONNECTED)
//     // {
//     //     HTTPClient http;

//     //     String url = String(scriptURL)
//     //         + "?temp=" + String(bme.temperature)
//     //         + "&humidity=" + String(bme.humidity)
//     //         + "&pressure=" + String(bme.pressure / 100.0)
//     //         + "&gas=" + String(bme.gas_resistance)
//     //         + "&co2=" + String(co2);

//     //     http.begin(url);

//     //     int httpCode = http.GET();

//     //     Serial.print("HTTP Code: ");
//     //     Serial.println(httpCode);

//     //     http.end();
//     // }

//     uploadDataToGoogleSheet(String(bme.temperature), String(bme.humidity), String(bme.pressure / 100.0),String(bme.gas_resistance),String(co2));


// }















// #include <Arduino.h>
// #include <SPI.h>
// #include <Adafruit_BME680.h>
// #define TINY_GSM_MODEM_SIM7600
// #include <TinyGsmClient.h>

// // ================= USER CONFIGURATION =================
// const char APN[]       = "e-ideas";       // Singtel APN
// const char GPRS_USER[] = "";
// const char GPRS_PASS[] = "";

// // Paste your Google Apps Script Deployment ID here
// const char GAS_SCRIPT_ID[] = "AKfycbxGhwmJ-USPqrZp1MUHoBLk97MAPtWl1bBGX7-yO_24VzDoaGQqMl8Ee-I1tQGf4Kulqw";
// // ======================================================

// // SIM7670G / SIM7672G S3 Pin Definitions
// #define BOARD_POWERON_PIN 12
// #define MODEM_PWRKEY_PIN  4
// #define MODEM_RESET_PIN   5
// #define MODEM_TX_PIN      11
// #define MODEM_RX_PIN      10

// #define SerialAT Serial1
// TinyGsm modem(SerialAT);

// // BME680 SPI Pin Definitions (matching your board labels: SCK=13, SDI=21)
// // CS is set to 4 (dummy pin since your module's CS is grounded on-board)
// #define BME_SCK   13
// #define BME_MOSI  21  
// #define BME_CS    4   

// Adafruit_BME680 bme(BME_CS, BME_MOSI, -1, BME_SCK);

// // Sensors 
// HardwareSerial K30Serial(2);

// // K30 packet
// byte requestCO2[] = { 0x68, 0x04, 0x00, 0x03, 0x00, 0x01, 0xC8, 0xF3 };
// byte response[7];

// void powerOnModem() {
//     pinMode(BOARD_POWERON_PIN, OUTPUT);
//     digitalWrite(BOARD_POWERON_PIN, HIGH);

//     pinMode(MODEM_PWRKEY_PIN, OUTPUT);
//     digitalWrite(MODEM_PWRKEY_PIN, LOW);
//     delay(100);
//     digitalWrite(MODEM_PWRKEY_PIN, HIGH);
//     delay(1000);
//     digitalWrite(MODEM_PWRKEY_PIN, LOW);
//     delay(3000);
// }

// bool uploadDataToGoogleSheet(float temp, float humidity, float pressure, float gas, int co2, const char* statusMsg) {
//     Serial.println("\n[CELL] Connecting to Cellular Network...");

//     if (!modem.isNetworkConnected() && !modem.waitForNetwork(30000)) {
//         Serial.println("[ERR] Network Registration Failed");
//         return false;
//     }

//     if (!modem.isGprsConnected() && !modem.gprsConnect(APN, GPRS_USER, GPRS_PASS)) {
//         Serial.println("[ERR] APN Connection Failed");
//         return false;
//     }

//     Serial.println("[CELL] Connected. Constructing HTTPS Request...");

//     String url = "https://script.google.com/macros/s/" + String(GAS_SCRIPT_ID) + "/exec";
//     url += "?temp=" + String(temp, 2) + 
//            "&humidity=" + String(humidity, 2) + 
//            "&pressure=" + String(pressure, 2) + 
//            "&gas=" + String(gas, 2) + 
//            "&co2=" + String(co2) + 
//            "&status=" + String(statusMsg);

//     modem.sendAT(GF("+HTTPINIT"));
//     modem.waitResponse(5000);

//     modem.sendAT(GF("+CSSLCFG=\"sslversion\",1,3"));
//     modem.waitResponse();
//     modem.sendAT(GF("+HTTPPARA=\"SSLCFG\",1"));
//     modem.waitResponse();

//     modem.sendAT(GF("+HTTPPARA=\"UA\",\"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36\""));
//     modem.waitResponse();

//     modem.sendAT(GF("+HTTPPARA=\"URL\",\""), url, GF("\""));
//     modem.waitResponse(5000);

//     modem.sendAT(GF("+HTTPPARA=\"REDIR\",1"));
//     modem.waitResponse();

//     Serial.println("[HTTPS] Sending GET payload...");
//     modem.sendAT(GF("+HTTPACTION=0"));

//     bool success = false;
//     if (modem.waitResponse(30000, GF("+HTTPACTION: 0,")) == 1) {
//         int httpStatus = modem.stream.parseInt();
//         modem.stream.readStringUntil(','); 
//         int dataLen = modem.stream.parseInt();

//         Serial.printf("[HTTPS] Status: %d, Data Length: %d bytes\n", httpStatus, dataLen);

//         if (dataLen > 0) {
//             Serial.println("\n--- GOOGLE RESPONSE BODY START ---");
//             modem.sendAT(GF("+HTTPREAD=0,"), dataLen);
//             if (modem.waitResponse(10000, GF("+HTTPREAD:")) == 1) {
//                 int readLen = modem.stream.parseInt();
//                 String responseBody = modem.stream.readString();
//                 Serial.println(responseBody);
//             }
//             Serial.println("--- GOOGLE RESPONSE BODY END ---\n");
//         }

//         if (httpStatus == 302 || httpStatus == 200) {
//             Serial.println("[DIAGNOSTIC] SUCCESS! Data sent successfully.");
//             success = true;
//         } else {
//             Serial.printf("[DIAGNOSTIC] Unexpected Status: %d\n", httpStatus);
//         }
//     } else {
//         Serial.println("[ERR] HTTPS Request Timed Out");
//     }

//     modem.sendAT(GF("+HTTPTERM"));
//     modem.waitResponse(5000);

//     return success;
// }

// int readCO2() {
//     while(K30Serial.available())
//         K30Serial.read();

//     K30Serial.write(requestCO2, 8);
//     delay(50);

//     if(K30Serial.available() < 7)
//         return -1;

//     for(int i=0; i<7; i++)
//         response[i] = K30Serial.read();

//     int ppm = response[3] * 256 + response[4];
//     return ppm;
// }

// void setup() {
//     Serial.begin(115200);

//     uint32_t startWait = millis();
//     while (!Serial && (millis() - startWait < 5000)) {
//         delay(10);
//     }

//     Serial.println("\n[SYS] Initializing SIM7670G/7672G Modem...");
//     powerOnModem();
    
//     // Modem Serial port initialization
//     SerialAT.begin(115200, SERIAL_8N1, MODEM_RX_PIN, MODEM_TX_PIN);
//     delay(1000);

//     if (modem.testAT()) {
//         Serial.println("[SYS] Modem Online.");
//     } else {
//         Serial.println("[ERR] Modem failed to respond to AT commands.");
//     }

//     // Initialize BME688 in SPI Mode
//     if(!bme.begin()) {
//         Serial.println("BME688 NOT FOUND (Check SCK and SDI wiring)");
//         while(1);
//     }
//     Serial.println("BME688 OK (SPI Mode)");

//     // Initialize UART2 for K30 CO2 sensor (RX = 15, TX = 16)
//     K30Serial.begin(9600, SERIAL_8N1, 15, 16);
//     Serial.println("K30 UART Started");

//     // Initial test upload
//     uploadDataToGoogleSheet(26.5, 72.3, 1013.25, 50000.0, 400, "ESP32_BOOT");
// }

// void loop() {
//     // Send data every 5 minutes
//     delay(300000);

//     float temp = 0.0;
//     float humidity = 0.0;
//     float pressure = 0.0;
//     float gas = 0.0;
//     int co2 = -1;

//     // Read BME688
//     if(bme.performReading()) {
//         temp = bme.temperature;
//         humidity = bme.humidity;
//         pressure = bme.pressure / 100.0;
//         gas = bme.gas_resistance;

//         Serial.println("========== BME688 ==========");
//         Serial.printf("Temperature : %.2f C\n", temp);
//         Serial.printf("Humidity    : %.2f %%\n", humidity);
//         Serial.printf("Pressure    : %.2f hPa\n", pressure);
//         Serial.printf("Gas         : %.2f Ohms\n", gas);
//     } else {
//         Serial.println("BME688 Read Failed");
//     }

//     // Read K30 CO2
//     co2 = readCO2();
//     Serial.println("========== K30 ==========");
//     if(co2 > 0) {
//         Serial.printf("CO2 : %d ppm\n", co2);
//     } else {
//         Serial.println("No response from K30");
//     }

//     Serial.println("\n------------------------------\n");
//     delay(2000);

//     // Upload payload via cellular module
//     uploadDataToGoogleSheet(temp, humidity, pressure, gas, co2, "Sensor_Data");
// }