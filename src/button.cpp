#include "button.hpp"


void
button_init(button_t *btn, uint8_t pin, uint8_t on_state)
{
    btn->pin = pin;
    btn->press_state = on_state;
    pinMode(pin, INPUT);
    btn->_initialized = true;
    btn->last_value = button_service(btn);
    btn->has_changed = false;
}

uint8_t
button_service(button_t *btn)
{
    assert(btn->_initialized);
    uint8_t value = digitalRead(btn->pin);
    btn->has_changed = btn->has_changed || btn->last_value != value;
    btn->last_value = value;
    return value;
}

static bool
button_has_changed(button_t *btn)
{
    assert(btn->_initialized);
    uint8_t has_changed = btn->has_changed;
    btn->has_changed = false;
    return has_changed;
}

bool
button_was_pressed(button_t *btn)
{
    assert(btn->_initialized);
    if (!button_has_changed(btn))
        return false;
    
    return btn->last_value == btn->press_state;
}
