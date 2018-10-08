#include "DHT.h"
#include <ESP8266WiFi.h>
#include <Base64.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFiMulti.h>

#define DHTPIN 2
#define DHTTYPE DHT11

//AP definitions
#define AP_SSID "varmar"
#define AP_PASSWORD "XXX"

// EMONCMS
#define HOST "http://192.168.1.248/emoncms"
#define NODE "TEMP1"
#define APIKEY "101673a46ef99df676d3660a3b30ea39"

ESP8266WiFiMulti WiFiMulti;

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);

void wifiConnect()
{
    Serial.print("Connecting to AP");
    WiFi.begin(AP_SSID, AP_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
}

void sendDataToEmon(int inputState, String key)
{

  if((WiFiMulti.run() == WL_CONNECTED)) {
   HTTPClient http;

  String url;

  Serial.print("Send to emon: ");
  Serial.println(inputState);

  url = String(HOST) + "/input/post.json?node="+ String(NODE) +"&csv={"+key+":"+ String(inputState) +"}&apikey="+String(APIKEY);

  http.begin(url);
  int httpCode = http.GET();
        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            Serial.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                Serial.println(payload);
            }
        } else {
            Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

    http.end();
    delay(10000);
  }
}

void setup() {
  Serial.begin(115200);
  delay(10);

  // Connect to WiFi network
  wifiConnect();

  dht.begin();
}

void loop() {

  delay(200000);
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) ) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // This sends off your payload.
  Serial.print("Temperature: ");
  Serial.println(t);

  sendDataToEmon(t, "temperature");
  sendDataToEmon(h, "humidity");

}
