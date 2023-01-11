#include <Arduino.h>


typedef struct button {
    uint8_t _initialized = false;
    uint8_t last_value, has_changed;
    uint8_t pin, press_state;
} button_t;

void button_init(button_t *btn, uint8_t pin, uint8_t on_state);
uint8_t button_service(button_t *btn);
bool button_is_pressed(button_t *btn);
bool button_was_pressed(button_t *btn);
