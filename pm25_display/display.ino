void displayNowAqi() {
  screen.clear();
  displayRect();
  displayAqiLabel();
  displayAQI();
  displayNiceMessage();
}

void displayAvgAqi() {
  screen.textSize(1).x(70).y(8).println("10m avg");
  display.drawLine(70, 16, 110, 16, SSD1306_WHITE);
  screen = screen.textSize(2).x(70).y(24);

  if (timer_start_avg.complete()) {
    screen.println(aqi_values.Average(minutes(10)));
  } else {
    screen.println("--");
  }
}

void displayRect() { display.drawRect(0, 16, 62, 33, SSD1306_WHITE); }

void displayAqiLabel() { screen.textSize(1).x(25).y(8).println("AQI"); }

void displayAQI() {
  uint16_t aqi = min(aqi_values.Latest().value, static_cast<uint16_t>(999));
  screen.textSize(3).x(aqi < 10 ? 25 : aqi < 100 ? 15 : 5).y(21).println(aqi);
}

void displayNiceMessage() {
  screen.textSize(1).x(4).y(51).println("Have a lovely day :)");
}
