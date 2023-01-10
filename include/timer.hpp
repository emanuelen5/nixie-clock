#include "ESP8266TimerInterrupt.h"

void
set_timer(ESP8266Timer *timer, void (*callback)(void), uint32_t timeout);
