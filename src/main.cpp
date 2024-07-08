#include <WiFi.h>
#include <Arduino.h>
#include <ThingSpeak.h>
#include "esp_sleep.h"
#include "config.h"

WiFiClient client;

RTC_DATA_ATTR int counter = 0;


#define WIFI_TIMEOUT_MS 20000
#define SLEEP_TIME 120e6 // 1 minute in microseconds

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
  Serial.begin(9600);
  delay(1000); // Adjusted delay for Serial Monitor initialization

  // Increment counter
  counter++;

  connectToWiFi();
  ThingSpeak.begin(client);

  // Check if Wi-Fi is connected
  if (WiFi.status() == WL_CONNECTED)
  {
    ThingSpeak.setField(1, counter);
    ThingSpeak.setField(2, WiFi.RSSI());
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

  // Go to sleep
  Serial.println("Going to sleep now.");
  esp_sleep_enable_timer_wakeup(SLEEP_TIME);
  esp_deep_sleep_start();
}

void loop()
{
  // ne rabis, ker se zbudi v setup
}
