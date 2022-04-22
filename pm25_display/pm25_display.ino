#include "Adafruit_PM25AQI.h"
#include "DHT.h"
#include "config.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include <Wire.h>

#include "RingBuffer.h"
#include "util.h"

// Every X seconds, read sensor and update screen
#define UPDATE_INTERVAL_SECONDS 5
#define NUM_BUFFERED_VALUES 120 // 10min * 60s / UPDATE_INTERVAL_SECONDS

// OLED screen
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1    // Reset pin # (or -1 if sharing Arduino reset pin)
// See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// AQI sensor
SoftwareSerial aqi_serial(2, 3);
Adafruit_PM25AQI aqi_sensor = Adafruit_PM25AQI();
RingBuffer<uint16_t> aqi_values(NUM_BUFFERED_VALUES);

// DHT22 Temp/Humidity sensor
DHT dht(0, DHT22);
RingBuffer<float> temp_c_values(NUM_BUFFERED_VALUES);
RingBuffer<float> humidity_values(NUM_BUFFERED_VALUES);

// Timers
Timer timer_read_sensor = {1000 * UPDATE_INTERVAL_SECONDS, 0};
Timer timer_start_avg = {1000 * 60 * 10, 0}; // 10 minutes

#include "Screen.h"
Screen screen;

void setup() {
  Serial.begin(115200);
  aqi_serial.begin(9600);

  InitDisplay();
  InitAqiSensor();
  dht.begin();
}

void loop() {
  if (timer_read_sensor.Complete()) {
    timer_read_sensor.Reset();

    PM25_AQI_Data data;
    if (ReadAqiSensor(&data)) {
      aqi_values.Insert(data.pm25_standard, millis());
      Serial.println("AQI: " + String(aqi_values.Latest().value));
    }

    float temp_c = dht.readTemperature();
    if (!isnan(temp_c)) {
      temp_c_values.Insert(temp_c, millis());
      Serial.println("Temp: " + String(temp_c_values.Latest().value) + "°C, " +
                     String(CToF(temp_c_values.Latest().value)) + "°F");
    }

    float humidity = dht.readHumidity();
    if (!isnan(humidity)) {
      humidity_values.Insert(humidity, millis());
      Serial.println("Humidity: " + String(humidity_values.Latest().value) +
                     "%");
    }

    displayNowAqi();
    displayAvgAqi();
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

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.setTextColor(SSD1306_WHITE);
  delay(2000);
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
