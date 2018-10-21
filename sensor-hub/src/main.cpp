#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <BME280_t.h>
#define ASCII_ESC 27
#define MYALTITUDE  100.50

float analog;

// const float MINIMUM_VOLTAGE = ;
// const float MAXIMUM_VOLTAGE = ;
// const float COEFF = ;

const char* ssid = "The Unicorns Sont Nos Amies";
const char* password = "Z47pQchWu9U3FmuG2LNN9YLc";

char bufout[10];

BME280<> BMESensor;

WiFiServer server(80);

void setup()
{
        Serial.begin(115200);
        Serial.println();

        Serial.printf("Connecting to wifi");
        WiFi.mode(WIFI_AP);
        WiFi.begin(ssid, password);
        while (WiFi.status() != WL_CONNECTED)
        {
                delay(500);
                Serial.print(".");
        }
        Serial.println(" connected");

        Wire.begin(5,4);
        BMESensor.begin();

        server.begin();
        Serial.printf("Web server started, open %s in a web browser\n", WiFi.localIP().toString().c_str());
}


String buildAnswer()
{
        BMESensor.refresh();
        float relativepressure = BMESensor.seaLevelForAltitude(MYALTITUDE);
        return String("HTTP/1.1 200 OK\r\n") +
               "Content-Type: text/html\r\n" +
               "Connection: keep-alive\r\n" +
               "Refresh: 3\r\n" +
               "\r\n" +
               "<!DOCTYPE HTML>" +
               "<html>" +
               "Analog input:  " + String(analogRead(A0)) + "</br>"+
               "Temperature: " + String(BMESensor.temperature) + "&deg;C </br>"+
               "Humidity: " + String(BMESensor.humidity) + "% </br>"+
               "Pressure: " + String(relativepressure  / 100.0F) + " hPa </br>"+
               "</html>" +
               "\r\n";
}


void loop()
{
        WiFiClient client = server.available();
        // wait for a client (web browser) to connect
        if (client)
        {
                Serial.println("\n[Client connected]");
                while (!client.available()) {
                        delay(1);
                }
                client.flush();
                client.println(buildAnswer());
                // close the connection:
                delay(10);
                Serial.println("[Client disonnected]");
        }
        delay(1); // give the web browser time to receive the data

}
