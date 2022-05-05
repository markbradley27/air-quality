#include "Adafruit_PM25AQI.h"
#include "DHT.h"
#include "config.h"
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <Wire.h>

#include "RingBuffer.h"
#include "displayers/AqiTempHumidBigNumbersDisplayer.h"
#include "displayers/Displayer.h"
#include "util.h"

// Every X seconds, read sensor and update screen
#define UPDATE_INTERVAL_SECONDS 5
#define UPLOAD_INTERVAL_MINUTES 10
#define NUM_BUFFERED_VALUES 120 // 10min * 60s / UPDATE_INTERVAL_SECONDS

// OLED screen
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1    // Reset pin # (or -1 if sharing Arduino reset pin)
// See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Wifi
#define SSID "henrietta"
#define PASSWORD "notgonnapayrent"
#define HOST "192.168.138.1"
#define PORT 42901

// AQI sensor
SoftwareSerial aqi_serial(2, 3);
Adafruit_PM25AQI aqi_sensor = Adafruit_PM25AQI();
RingBuffer<uint16_t> aqi_values(NUM_BUFFERED_VALUES);

// DHT22 Temp/Humidity sensor
// Value that should be added to each temp reading.
#define TEMP_CALIBRATION_OFFSET -3.53
// Value that should be added to each humidity reading.
#define HUMIDITY_CALIBRATION_OFFSET 12.28
DHT dht(0, DHT22);
RingBuffer<float> temp_c_values(NUM_BUFFERED_VALUES);
RingBuffer<float> humidity_values(NUM_BUFFERED_VALUES);

// Timers
Timer timer_read_sensor = {seconds(UPDATE_INTERVAL_SECONDS)};
Timer timer_upload_data = {minutes(UPLOAD_INTERVAL_MINUTES)};

// Displayers
Displayer *displayer = new AqiTempHumidBigNumbersDisplayer(
    &display, &aqi_values, &temp_c_values, &humidity_values);

void setup() {
  Serial.begin(115200);
  aqi_serial.begin(9600);

  InitDisplay();
  InitWifi();
  InitAqiSensor();
  dht.begin();

  displayer->Refresh();
}

void loop() {
  if (timer_read_sensor.Complete()) {
    timer_read_sensor.Reset();

    PM25_AQI_Data data;
    if (ReadAqiSensor(&data)) {
      aqi_values.Insert(data.pm25_standard, millis());
      Serial.println("AQI: " + String(aqi_values.Latest().value));
    }

    float temp_c = dht.readTemperature() + TEMP_CALIBRATION_OFFSET;
    if (!isnan(temp_c)) {
      temp_c_values.Insert(temp_c, millis());
      Serial.println("Temp: " + String(temp_c_values.Latest().value) + "°C, " +
                     String(CToF(temp_c_values.Latest().value)) + "°F");
    }

    float humidity = dht.readHumidity() + HUMIDITY_CALIBRATION_OFFSET;
    if (!isnan(humidity)) {
      humidity_values.Insert(humidity, millis());
      Serial.println("Humidity: " + String(humidity_values.Latest().value) +
                     "%");
    }

    displayer->Update();
  }

  if (timer_upload_data.Complete()) {
    timer_upload_data.Reset();

    WiFiClient client;
    if (!client.connect(HOST, PORT)) {
      Serial.println("Tried to upload data but TCP client connection failed.");
      return;
    }
    client.println("Temperature: " +
                   String(temp_c_values.Average(minutes(10))));
    client.println("Humidity: " + String(humidity_values.Average(minutes(10))));
    client.println("AQI: " + String(aqi_values.Average(minutes(10))));
    client.stop();
  }
}

void InitDisplay() {

  if (!display.begin(
          SSD1306_SWITCHCAPVCC, // Generate display voltage from 3.3V internally
          SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  // Dim display a bit.
  display.ssd1306_command(SSD1306_SETCONTRAST);
  display.ssd1306_command(0x01);
}

void InitWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
  const int timeout_millis = millis() + 7000;
  Serial.print("Connecting to Wifi(" + String(SSID) + ")");
  while (WiFi.status() != WL_CONNECTED && millis() < timeout_millis) {
    Serial.print(".");
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("connected!");
  } else {
    Serial.println(
        "this is taking too long, will keep trying in the background");
  }
}

void InitAqiSensor() {
  // Wait one second for sensor to boot up.
  delay(1000);

  // Connect to the sensor over software serial.
  if (!aqi_sensor.begin_UART(&aqi_serial)) {
    Serial.println("Could not find PM 2.5 sensor!");
    while (1) {
      delay(10);
    }
  }

  Serial.println("PM25 found!");
}

bool ReadAqiSensor(PM25_AQI_Data *data) {
  // Clear the serial buffer.
  while (aqi_serial.available()) {
    aqi_serial.read();
  }

  // Wait for a reading.
  unsigned long start_ms = millis();
  while (millis() - start_ms < seconds(2)) {
    if (aqi_sensor.read(data)) {
      return true;
    }
  }
  return false;
}
