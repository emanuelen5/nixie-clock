#include "timer.hpp"


void
set_timer(ESP8266Timer *timer, void (*callback)(void), uint32_t timeout)
{
  if (timer->attachInterruptInterval(timeout * 1000, callback)) {
    Serial.println(F("Starting timer OK"));
  }
  else {
    Serial.println(F("Can't set timer correctly. Select another freq. or interval"));
  }
}
