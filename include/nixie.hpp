#include "Arduino.h"

typedef struct nixie {
    uint8_t _initialized = false;
    uint8_t pin_clk, pin_data, pin_en;
} nixie_t;

void nixie_init(nixie_t *nixie, uint8_t clk_pin, uint8_t data_pin, uint8_t en_pin);
void nixie_display_number(nixie_t *nixie, byte n1, byte n2, byte n3, byte n4);
