#ifndef Displayer_h
#define Displayer_h

#include <Adafruit_SSD1306.h>

class Displayer {
public:
  Displayer(Adafruit_SSD1306 *display) : display_(display){};

  // Clears and refreshes the entire display.
  virtual void Refresh() = 0;

  // Updates display to reflect new readings.
  virtual void Update() = 0;

protected:
  // Does not take ownership.
  Adafruit_SSD1306 *display_;
};

#endif
