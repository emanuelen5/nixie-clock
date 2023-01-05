#include "nixie.hpp"


void
nixie_init(nixie_t *nixie, uint8_t clk_pin, uint8_t data_pin, uint8_t en_pin)
{
    nixie->pin_clk = clk_pin;
    nixie->pin_data = data_pin;
    nixie->pin_en = en_pin;

    pinMode(data_pin, OUTPUT);
    digitalWrite(data_pin, LOW);
    pinMode(clk_pin, OUTPUT);
    digitalWrite(clk_pin, LOW);
    pinMode(en_pin, OUTPUT);
    digitalWrite(en_pin, LOW);

    nixie->_initialized = true;
}

void
_nixie_digit(nixie_t *nixie, byte digit)
{
    assert(nixie->_initialized);
    // Send data to the nixie driver
    for (int i = 15; i >= 0; i--)
    {
        // Set high only the bit that corresponds to the current nixie cathode
        if(i == digit) digitalWrite(nixie->pin_data, 1);
        else digitalWrite(nixie->pin_data, 0);
        delayMicroseconds(1);

        // Register shifts bits on upstroke of CLK pin
        digitalWrite(nixie->pin_clk, 1);

        delayMicroseconds(1);
        //Set low the data pin after shift to prevent bleed through
        digitalWrite(nixie->pin_clk, 0);
    }
}

void
nixie_display_number(nixie_t *nixie, byte n1, byte n2, byte n3, byte n4)
{
    assert(nixie->_initialized);
    // Ground EN pin and hold low for as long as you are transmitting
    digitalWrite(nixie->pin_en, 0);
    // Clear everything out just in case to
    // prepare shift register for bit shifting
    digitalWrite(nixie->pin_data, 0);
    digitalWrite(nixie->pin_clk, 0);
    delayMicroseconds(1);

    _nixie_digit(nixie, n4);
    _nixie_digit(nixie, n3);
    _nixie_digit(nixie, n2);
    _nixie_digit(nixie, n1);

    // Return the EN pin high to signal chip that it
    // no longer needs to listen for data
    digitalWrite(nixie->pin_en, 1);

    // Stop shifting
    digitalWrite(nixie->pin_clk, 0);
}
