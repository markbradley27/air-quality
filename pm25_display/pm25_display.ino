#include "Adafruit_PM25AQI.h"
#include "config.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include <Wire.h>

#include "Aqi.h"
#include "Timer.h"

// Every X seconds, read sensor and update screen
#define UPDATE_INTERVAL_SECONDS 5
#define NUM_AQI_VALUES 120 // 10min * (60s / UPDATE_INTERVAL_SECONDS)

// AQI sensor
SoftwareSerial aqi_serial(2, 3);
Adafruit_PM25AQI aqi_sensor = Adafruit_PM25AQI();
AqiBuffer aqi_values(NUM_AQI_VALUES);
Aqi aqi;

// OLED screen
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1    // Reset pin # (or -1 if sharing Arduino reset pin)
// See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Timers
Timer timer_read_sensor = {1000 * UPDATE_INTERVAL_SECONDS, 0};
Timer timer_start_avg = {1000 * 60 * 10, 0}; // 10 minutes

#include "Screen.h"
Screen screen;

void setup() {
  Serial.begin(115200);
  aqi_serial.begin(9600);

  initDisplay();
  InitAqiSensor();
}

void loop() {
  if (timer_read_sensor.complete()) {
    timer_read_sensor.reset();

    ReadAqiSensor();
    if (aqi.at_millis != 0) {
      aqi_values.insert(aqi);
    }

    displayNowAqi();
    displayAvgAqi();
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

void ReadAqiSensor() {
  // Clear the serial buffer.
  while (aqi_serial.available()) {
    aqi_serial.read();
  }

  // Wait for a reading.
  unsigned long start_ms = millis();
  PM25_AQI_Data data;
  while (millis() - start_ms < seconds(2)) {
    if (aqi_sensor.read(&data)) {
      aqi = {data.pm25_standard, millis()};
      break;
    }
  }
}
