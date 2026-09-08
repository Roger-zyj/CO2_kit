#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BME680.h>
#include <WiFi.h>
#include <HTTPClient.h>

// const char* ssid = "rrpj";
// const char* password = "9621097665";
const char* ssid = "Roger's Phone";
const char* password = "006.6261e-34";
const char* scriptURL =
"https://script.google.com/macros/s/AKfycbyCrH6vVaznfHf4bLDbJxBdZjPP7FsokRXKmH5J4fwS1Ypp-JgbEafOhqvAiNBhBeNE/exec";



const int LED_PIN = 25;
const int BAT_ADC_PIN = 35; // LilyGO T-SIM7000G internal battery ADC pin
HardwareSerial K30Serial(2);

Adafruit_BME680 bme;

// K30 packet
byte requestCO2[] =
{
    0x68,
    0x04,
    0x00,
    0x03,
    0x00,
    0x01,
    0xC8,
    0xF3
};

byte response[7];

int readCO2()
{
    while(K30Serial.available())
        K30Serial.read();

    K30Serial.write(requestCO2, 8);

    unsigned long start = millis();

    while (K30Serial.available() < 7)
    {
        if (millis() - start > 200)
            return -1;
    }

    for(int i=0;i<7;i++)
        response[i]=K30Serial.read();

    int ppm = response[3]*256 + response[4];

    return ppm;
}


// Function to read and calculate the current battery voltage
float readBatteryVoltage()
{
    // Read raw ADC value (0 to 4095)
    int adcValue = analogRead(BAT_ADC_PIN);
    
    // Convert ADC to the voltage present at the ESP32 pin (assuming 3.3V reference)
    float pinVoltage = (adcValue / 4095.0) * 3.3;
    
    // Multiply by 2 to reverse the T-SIM7000G's internal 1:2 voltage divider 
    // Note: ESP32 ADCs are slightly non-linear. If your reading is slightly off, 
    // you can adjust this 2.0 multiplier (e.g., to 2.05 or 1.95) to calibrate it.
    float batteryVoltage = pinVoltage * 2.0; 
    
    return batteryVoltage;
}

void setup()
{

    Serial.begin(115200);

    // Connect wifi
    Serial.print("Connecting");

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("Connected!");

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    //LED check code is running

    pinMode(LED_PIN, OUTPUT); 
    digitalWrite(LED_PIN, HIGH);

    //BME688

    Wire.begin();

    Serial.println();
    Serial.println("Starting...");

    if(!bme.begin())
    {
        Serial.println("BME688 NOT FOUND");
        while(1);
    }

    Serial.println("BME688 OK");

    // UART2 CO2
    K30Serial.begin(
        9600,
        SERIAL_8N1,
        32,     // RX
        33      // TX
    );

    Serial.println("K30 UART Started");
}

void loop()
{

    //---------------------------------------
    // Read BME688
    //---------------------------------------

    if(bme.performReading())
    {
        Serial.println("========== BME688 ==========");

        Serial.print("Temperature : ");
        Serial.print(bme.temperature);
        Serial.println(" C");

        Serial.print("Humidity    : ");
        Serial.print(bme.humidity);
        Serial.println(" %");

        Serial.print("Pressure    : ");
        Serial.print(bme.pressure/100.0);
        Serial.println(" hPa");

        Serial.print("Gas         : ");
        Serial.print(bme.gas_resistance);
        Serial.println(" Ohms");
    }
    else
    {
        Serial.println("BME688 Read Failed");
    }

    //---------------------------------------
    // Read K30
    //---------------------------------------

    int co2 = readCO2();

    Serial.println("========== K30 ==========");

    if(co2 > 0)
    {
        Serial.print("CO2 : ");
        Serial.print(co2);
        Serial.println(" ppm");
    }
    else
    {
        Serial.println("No response from K30");
    }

    Serial.println();
    Serial.println("------------------------------");
    Serial.println();

    //---------------------------------------
    // Read Battery Voltage
    //---------------------------------------
    float batteryVoltage = readBatteryVoltage();
    
    Serial.println("========== Battery ==========");
    Serial.print("Voltage     : ");
    Serial.print(batteryVoltage, 2);
    Serial.println(" V");

    Serial.println();
    Serial.println("------------------------------");
    Serial.println();

    // delay(2000);
    delay(200);
    
    //Wifi + Upload data
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;

        String url = String(scriptURL)
            + "?temp=" + String(bme.temperature)
            + "&humidity=" + String(bme.humidity)
            + "&pressure=" + String(bme.pressure / 100.0)
            + "&gas=" + String(bme.gas_resistance)
            + "&co2=" + String(co2)
            + "&vbat=" + String(batteryVoltage, 2);


//         String url = String(scriptURL)
//             + "?temp=" + String(temp)
//             + "&humidity=" + String(humidity)
//             + "&pressure=" + String(pressure)
//             + "&gas=" + String(gasresistance)
//             + "&co2=" + String(co2)
//             + "&vbat=" + String(batteryVoltage, 2);

        http.begin(url);

        int httpCode = http.GET();

        Serial.print("HTTP Code: ");
        Serial.println(httpCode);

        http.end();
    }
}





















// #include <Arduino.h>
// #include <Wire.h>
// #include <Adafruit_BME680.h>
// #include <WiFi.h>
// #include <HTTPClient.h>


// // const char* ssid = "rrpj";
// // const char* password = "9621097665";
// const char* ssid = "Roger's Phone";
// const char* password = "006.6261e-34";
// const char* scriptURL = "https://script.google.com/macros/s/AKfycbyyZ-5CpINFAX-Nk0PWd5c_gZNIlroQKqK6tVO5j3c_Ktc2fVATUM0o-OD2TFucfTgd/exec";

// const int LED_PIN = 25;
// const int BAT_ADC_PIN = 35; // LilyGO T-SIM7000G internal battery ADC pin
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

// // Function to read and calculate the current battery voltage
// float readBatteryVoltage()
// {
//     // Read raw ADC value (0 to 4095)
//     int adcValue = analogRead(BAT_ADC_PIN);
    
//     // Convert ADC to the voltage present at the ESP32 pin (assuming 3.3V reference)
//     float pinVoltage = (adcValue / 4095.0) * 3.3;
    
//     // Multiply by 2 to reverse the T-SIM7000G's internal 1:2 voltage divider 
//     // Note: ESP32 ADCs are slightly non-linear. If your reading is slightly off, 
//     // you can adjust this 2.0 multiplier (e.g., to 2.05 or 1.95) to calibrate it.
//     float batteryVoltage = pinVoltage * 2.0; 
    
//     return batteryVoltage;
// }

// void setup()
// {
//     Serial.begin(115200);

//     // Connect wifi
//     Serial.print("Connecting");

//     WiFi.begin(ssid, password);

//     while (WiFi.status() != WL_CONNECTED)
//     {
//         delay(500);
//         Serial.print(".");
//     }

//     Serial.println();
//     Serial.println("Connected!");

//     Serial.print("IP Address: ");
//     Serial.println(WiFi.localIP());

//     // LED check code is running
//     pinMode(LED_PIN, OUTPUT); 
//     digitalWrite(LED_PIN, HIGH);

//     // Ensure the ADC is set to full range (0-3.3V) for the battery pin
//     analogSetAttenuation(ADC_11db);

//     // BME688
//     Wire.begin();

//     Serial.println();
//     Serial.println("Starting...");

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
// }

// void loop()
// {
//     float temp = -1;
//     float humidity = -1;
//     float pressure = -1;
//     float gasresistance = -1;
//     //---------------------------------------
//     // Read BME688
//     //---------------------------------------
//     if(bme.performReading())
//     {

//         temp = bme.temperature;
//         humidity = bme.humidity;
//         pressure = bme.pressure/100.0;
//         gasresistance = bme.gas_resistance;


//         Serial.println("========== BME688 ==========");

//         Serial.print("Temperature : ");
//         Serial.print(temp);
//         Serial.println(" C");

//         Serial.print("Humidity    : ");
//         Serial.print(humidity);
//         Serial.println(" %");

//         Serial.print("Pressure    : ");
//         Serial.print(pressure);
//         Serial.println(" hPa");

//         Serial.print("Gas         : ");
//         Serial.print(gasresistance);
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

//     //---------------------------------------
//     // Read Battery Voltage
//     //---------------------------------------
//     float batteryVoltage = readBatteryVoltage();
    
//     Serial.println("========== Battery ==========");
//     Serial.print("Voltage     : ");
//     Serial.print(batteryVoltage, 2);
//     Serial.println(" V");

//     Serial.println();
//     Serial.println("------------------------------");
//     Serial.println();

//     delay(2000);

//     // Wifi + Upload data
//     if (WiFi.status() == WL_CONNECTED)
//     {
//         HTTPClient http;

//         // Added "&vbat=" parameter to the end of the URL string

        
//         String url = String(scriptURL)
//             + "?temp=" + String(temp)
//             + "&humidity=" + String(humidity)
//             + "&pressure=" + String(pressure)
//             + "&gas=" + String(gasresistance)
//             + "&co2=" + String(co2)
//             + "&vbat=" + String(batteryVoltage, 2);

//         http.begin(url);

//         int httpCode = http.GET();

//         Serial.print("HTTP Code: ");
//         Serial.println(httpCode);

//         http.end();
//     }
// }