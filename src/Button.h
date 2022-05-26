#ifndef Button_h
#define Button_h

#include "Arduino.h"

class Button {
public:
  Button(const int pin) : pin_(pin) {
    pinMode(pin_, INPUT_PULLUP);
    state_ = digitalRead(pin_);
    initial_state_ = state_;
  }

  int UnhandledPresses() {
    const int ret = unhandled_presses_;
    unhandled_presses_ = 0;
    return ret;
  }

  void Isr() {
    const int ms = millis();
    if (ms - last_change_millis_ > 200) {
      state_ = digitalRead(pin_);
      if (state_ != initial_state_) {
        ++unhandled_presses_;
      }
      last_change_millis_ = ms;
    }
  }

private:
  const int pin_;
  bool initial_state_;
  bool state_;
  unsigned long last_change_millis_ = 0;
  int unhandled_presses_ = 0;
};

#endif
