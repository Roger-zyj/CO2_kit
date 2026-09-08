#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BME680.h>
#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "rrpj";
const char* password = "9621097665";
const char* scriptURL =
"https://script.google.com/macros/s/AKfycbyCrH6vVaznfHf4bLDbJxBdZjPP7FsokRXKmH5J4fwS1Ypp-JgbEafOhqvAiNBhBeNE/exec";

const int LED_PIN = 25;
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

    K30Serial.write(requestCO2,8);

    delay(50);

    if(K30Serial.available() < 7)
        return -1;

    for(int i=0;i<7;i++)
        response[i]=K30Serial.read();

    int ppm = response[3]*256 + response[4];

    return ppm;
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


    // Sensors init ------------------------------------------------------------------------
    // if(!bme.begin())
    // {
    //     Serial.println("BME688 NOT FOUND");
    //     while(1);
    // }

    // Serial.println("BME688 OK");

    // // UART2 CO2
    // K30Serial.begin(
    //     9600,
    //     SERIAL_8N1,
    //     32,     // RX
    //     33      // TX
    // );

    // Serial.println("K30 UART Started");
    // Sensors init ------------------------------------------------------------------------
    
}

void loop()
{
    // //---------------------------------------
    // // Read BME688
    // //---------------------------------------

    // if(bme.performReading())
    // {
    //     Serial.println("========== BME688 ==========");

    //     Serial.print("Temperature : ");
    //     Serial.print(bme.temperature);
    //     Serial.println(" C");

    //     Serial.print("Humidity    : ");
    //     Serial.print(bme.humidity);
    //     Serial.println(" %");

    //     Serial.print("Pressure    : ");
    //     Serial.print(bme.pressure/100.0);
    //     Serial.println(" hPa");

    //     Serial.print("Gas         : ");
    //     Serial.print(bme.gas_resistance);
    //     Serial.println(" Ohms");
    // }
    // else
    // {
    //     Serial.println("BME688 Read Failed");
    // }

    // //---------------------------------------
    // // Read K30
    // //---------------------------------------

    // int co2 = readCO2();

    // Serial.println("========== K30 ==========");

    // if(co2 > 0)
    // {
    //     Serial.print("CO2 : ");
    //     Serial.print(co2);
    //     Serial.println(" ppm");
    // }
    // else
    // {
    //     Serial.println("No response from K30");
    // }

    // Serial.println();
    // Serial.println("------------------------------");
    // Serial.println();

    // delay(2000);

    
    // //Wifi + Upload data
    // if (WiFi.status() == WL_CONNECTED)
    // {
    //     HTTPClient http;

    //     String url = String(scriptURL)
    //         + "?temp=" + String(bme.temperature)
    //         + "&humidity=" + String(bme.humidity)
    //         + "&pressure=" + String(bme.pressure / 100.0)
    //         + "&gas=" + String(bme.gas_resistance)
    //         + "&co2=" + String(co2);

    //     http.begin(url);

    //     int httpCode = http.GET();

    //     Serial.print("HTTP Code: ");
    //     Serial.println(httpCode);

    //     http.end();
    // }

    //Wifi + Upload data without any real sensors data
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;

        String url = String(scriptURL)
            + "?temp=" + String(1)
            + "&humidity=" + String(2)
            + "&pressure=" + String(3)
            + "&gas=" + String(4)
            + "&co2=" + String(5);

        http.begin(url);

        int httpCode = http.GET();

        Serial.print("HTTP Code: ");
        Serial.println(httpCode);

        http.end();
    }
    delay(5000);
}
