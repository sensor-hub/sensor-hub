#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <BME280_t.h>
#include <Ticker.h>
#include <secret.h>

#define ASCII_ESC 27
#define MYALTITUDE  100.50

float analog;


// real current = (adc - MINIMUM_VOLTAGE) / 1.024 / PROBE_RATIO / CURRENT_RES
const float MINIMUM_VOLTAGE = 0.486; // because
const float CURRENT_RES = 15.0;
const float PROBE_RATIO = 0.0005;

const char* ssid = secret_ssid;
const char* wifi_password = secret_wifi_password;
const char* remote_host = secret_remote_host;
const char* remote_user = secret_remote_user;
const char* remote_password = secret_remote_password;


int pushTrigger = 0; // 1 means that it is time to push the data

BME280<> BMESensor;
Ticker pushTicker;
WiFiServer server(80);
WiFiClient client;
HTTPClient pushClient;

void setup()
{
        Serial.begin(115200);
        Serial.println();

        Serial.printf("Connecting to wifi");
        WiFi.mode(WIFI_AP);
        WiFi.begin(ssid, wifi_password);
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

        pushTicker.attach_ms(5000, []() {
                pushTrigger = 1;
        });
}


float readCurrent(){
    return (analogRead(A0)/1023.0 - MINIMUM_VOLTAGE) / (CURRENT_RES * PROBE_RATIO);
}


String buildSerialAnswer()
{
        BMESensor.refresh();
        float relativepressure = BMESensor.seaLevelForAltitude(MYALTITUDE);
        return String("HTTP/1.1 200 OK\r\n") +
               "Content-Type: text/html\r\n" +
               "Connection: keep-alive\r\n" +
               "Refresh: 1\r\n" +
               "\r\n" +
               "<!DOCTYPE HTML>" +
               "<html>" +
               "Current:  " + String(readCurrent()) + "A</br>"+
               "Temperature: " + String(BMESensor.temperature) + "&deg;C </br>"+
               "Humidity: " + String(BMESensor.humidity) + "% </br>"+
               "Pressure: " + String(relativepressure  / 100.0F) + " hPa </br>"+
               "</html>" +
               "\r\n";
}


String buildPostAnswer(){
    BMESensor.refresh();
    float relativepressure = BMESensor.seaLevelForAltitude(MYALTITUDE);
    return String("intensity,sensor=appart value=") + String(readCurrent()) + "\n" +
            "temperature,sensor=appart value=" + String(BMESensor.temperature) + "\n" +
            "humidity,sensor=appart value=" + String(BMESensor.humidity) + "\n" +
            "pressure,sensor=appart value=" + String(relativepressure  / 100.0F);
}


void loop()
{
        client = server.available();
        // wait for a client (web browser) to connect
        if (client)
        {
                Serial.println("\n[Client connected]");
                while (!client.available()) {
                        delay(1);
                }
                client.flush();
                client.println(buildSerialAnswer());
                // close the connection:
                delay(10);
                Serial.println("[Client disonnected]");
        }
        if (pushTrigger == 1) {
                pushTrigger = 0;
                pushClient.begin(remote_host);
                pushClient.setAuthorization(remote_user, remote_password);
                int httpCode = pushClient.POST(buildPostAnswer());
                String payload = pushClient.getString();
                Serial.println("Server returned code " + String(httpCode));
                Serial.println("Server returned : " + payload);
                pushClient.end();
        }
        delay(1);

}
