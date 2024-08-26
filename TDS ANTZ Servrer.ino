#include <WiFi.h>
#include <WiFiManager.h>  // Include the WiFiManager library
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <esp_sleep.h>

// Server URL
const char* serverName = "http://app.antzsystems.com/api/v1/iot/enclosure/metric/update";

// TDS sensor setup
#define TdsSensorPin 32    // Pin where the TDS sensor is connected

// NTP client setup
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

// Deep sleep setup
#define uS_TO_S_FACTOR 1000000ULL  // Conversion factor for microseconds to seconds
#define TIME_TO_SLEEP  900         // Time ESP32 will go to sleep (in seconds, 900 seconds = 15 minutes)

void setup() {
  Serial.begin(115200);

  // Initialize WiFiManager
  WiFiManager wifiManager;
  wifiManager.autoConnect("ESP32_AP");  // AutoConnect with default parameters

  Serial.println("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Timer set to 15 minutes (deep sleep), it will take 15 minutes before publishing the next reading.");

  pinMode(TdsSensorPin, INPUT);  // Initialize the TDS sensor pin
  timeClient.begin();            // Initialize the NTP client
  timeClient.setTimeOffset(19800); // Set offset time in seconds for IST (5 hours 30 minutes)
}

String getFormattedTime() {
  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime((time_t *)&epochTime);

  char formattedTime[25];
  sprintf(formattedTime, "%04d-%02d-%02dT%02d:%02d:%02d",
          ptm->tm_year + 1900,
          ptm->tm_mon + 1,
          ptm->tm_mday,
          ptm->tm_hour,
          ptm->tm_min,
          ptm->tm_sec);
  return String(formattedTime);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    // Reading TDS value from sensor
    int analogValue = analogRead(TdsSensorPin);  // Read the analog value from the TDS sensor
    float tdsValue = 0.0;
    String sensorStatus = "Not Connected";
    
    if (analogValue > 10) {  // Ignore very low readings
      tdsValue = 0.3101 * analogValue + 15.12;
      Serial.print("Analog Value: ");
      Serial.println(analogValue);
      Serial.print("TDS: ");
      Serial.println(tdsValue);
      sensorStatus = "Connected";
    } else {
      Serial.println("TDS sensor is dry or not connected.");
      tdsValue = 0; // Ensure to send 0 if sensor is not connected
    }

    Serial.println("Sending HTTP request...");

    // Print TDS value and sensor status to Serial Monitor
    Serial.print("TDS Value: ");
    Serial.print(tdsValue);
    Serial.println(" ppm");
    Serial.print("Sensor Status: ");
    Serial.println(sensorStatus);

    // Specify content-type header as application/json
    HTTPClient http;

    // Use WiFiClient for HTTP (insecure)
    WiFiClient client;

    http.begin(client, serverName);

    // Get current time in IST
    String eventDate = getFormattedTime();

    // Create JSON data
    DynamicJsonDocument jsonDoc(256); // Adjust the buffer size as needed
    JsonObject root = jsonDoc.to<JsonObject>();
    root["enclosure_id"] = 113;

    JsonArray values = root.createNestedArray("values");

    JsonObject tdsValueObj = values.createNestedObject();
    tdsValueObj["key"] = "TDS Value";
    tdsValueObj["value"] = tdsValue;
    tdsValueObj["uom"] = "ppm";
    tdsValueObj["event_date"] = eventDate;

    JsonObject statusObj = values.createNestedObject();
    statusObj["key"] = "Sensor Status";
    statusObj["value"] = sensorStatus;
    statusObj["event_date"] = eventDate;

    // Serialize JSON to a string
    String jsonString;
    serializeJson(root, jsonString);
    Serial.println("JSON Data: " + jsonString);

    // Send HTTP POST request with JSON data
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(jsonString);

    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    // Check for a successful HTTP response (200 OK)
    if (httpResponseCode == 200) {
      // Parse and print the response
      String response = http.getString();
      Serial.println("Server response: " + response);
    } else {
      Serial.println("HTTP request failed");
    }

    // Free resources
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }

  // Enter deep sleep
  Serial.println("Entering deep sleep for 15 minutes...");
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}
