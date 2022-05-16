#ifndef BlankDisplayer_h
#define BlankDisplayer_h

#include <Adafruit_SSD1306.h>

#include "Displayer.h"

class BlankDisplayer : public Displayer {
public:
  BlankDisplayer(Adafruit_SSD1306 *display) : Displayer(display){};

  void Refresh() override {
    display_->clearDisplay();
    display_->display();
  }

  void Update() override{};
};

#endif
