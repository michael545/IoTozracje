#include <WiFi.h>
#include <Arduino.h>
#include <ThingSpeak.h>
#include "esp_sleep.h"
#include "config.h"
#include "DHTesp.h" // DHT sensor library for ESP32

#define DHTPIN 4 // Digital pin connected to the DHT sensor

DHTesp dht;

WiFiClient client;

#define WIFI_TIMEOUT_MS 20000
#define SLEEP_TIME 120e6 // 2 minutes in microseconds

void connectToWiFi()
{
  Serial.print("Connecting to WiFi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);

  unsigned long startAttemptTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_TIMEOUT_MS)
  {
    Serial.print(".");
    delay(100);
  }

  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println(" Failed!");
  }
  else
  {
    Serial.print("Connected! IP address: ");
    Serial.println(WiFi.localIP());
  }
}

void setup()
{
  Serial.begin(115200);
  delay(1000); // inicializcija

  dht.setup(DHTPIN, DHTesp::DHT22);
  connectToWiFi();
  ThingSpeak.begin(client);

  // Read data from DHT22
  TempAndHumidity lastValues = dht.getTempAndHumidity();

  // Check if any reads failed
  if (dht.getStatus() != 0)
  {
    Serial.println("DHT22 error status: " + String(dht.getStatusString()));
  }
  else
  {
    Serial.print("Temperature: ");
    Serial.print(lastValues.temperature);
    Serial.print("°C, Humidity: ");
    Serial.print(lastValues.humidity);
    Serial.println("%");

    if (WiFi.status() == WL_CONNECTED)
    {
      ThingSpeak.setField(1, lastValues.temperature);
      ThingSpeak.setField(2, lastValues.humidity);
      int x = ThingSpeak.writeFields(CHANNEL_ID, CHANNEL_API_KEY);
      if (x == 200)
      {
        Serial.println("Channel update successful.");
      }
      else
      {
        Serial.println("Problem updating channel. HTTP error code " + String(x));
      }
    }
    else
    {
      Serial.println("Skipping ThingSpeak update because Wi-Fi is not connected.");
    }
  }

  Serial.println("Going to sleep now.");
  esp_sleep_enable_timer_wakeup(SLEEP_TIME);
  esp_deep_sleep_start();
}

void loop()
{
  // Not needed, as ESP32 wakes up into setup()
}