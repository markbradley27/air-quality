void initDisplay() {
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.setTextColor(SSD1306_WHITE);
  //display.display();
  delay(2000);
}

void initSensor() {
  // Wait one second for sensor to boot up!
  delay(1000);

  // connect to the sensor over software serial
  if (!aqi.begin_UART(&pmSerial)) {
    Serial.println("Could not find PM 2.5 sensor!");
    while (1) {
      delay(10);
    }
  }

  Serial.println("PM25 found!");
}

bool connectWiFi() {
  WiFi.begin(SECRET_SSID, SECRET_PASS);

  Serial.print("Connecting to ");
  Serial.println(SECRET_SSID);

  display.setCursor(10, 10);
  display.println("Connecting to ");
  display.println(SECRET_SSID);
  display.display();

  int cursorX = 20;
  display.setCursor(cursorX, 30);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 10) {
    delay(500);
    Serial.print(".");
    display.print(".");
    display.display();

    cursorX += 5;
    display.setCursor(cursorX, 30);

    attempts++;
  }

  display.setCursor(10, 50);

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected!");
    Serial.println(WiFi.localIP());
    display.println("Connected!");
    display.display();
    delay(1000);
    return true;
  }

  Serial.println("Failed to connect to WiFi");
  display.println("Failed to connect.");
  display.display();
  delay(1000);
  return false;
}

void connectAdafruitIO() {
  Serial.print("Connecting to Adafruit IO");

  // connect to io.adafruit.com
  io.connect();

  // wait for a connection
  int attempts = 0;
  while (io.status() < AIO_CONNECTED && attempts < 10) {
    Serial.print(".");
    delay(500);
    attempts++;
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());
}
