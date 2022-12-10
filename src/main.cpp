// Nixie Tube Driver by Marcin Saj https://nixietester.com
// https://github.com/marcinsaj/Nixie-Tube-Driver
//
// Driving Nixie Tube Example #1
//
// This example demonstrates how to control a nixie tube.
// The control is carried out using the Nixie Tube Driver.
#include <Arduino.h>

#define DIN_PIN   13          // Nixie driver (shift register) serial data input pin             
#define CLK_PIN   14          // Nixie driver clock input pin
#define EN_PIN    15          // Nixie driver enable input pin
#define BTN1_PIN  0
#define BTN2_PIN  2

void NixieDisplay(byte digit)
{
  // Ground EN pin and hold low for as long as you are transmitting
  digitalWrite(EN_PIN, 0); 
  // Clear everything out just in case to
  // prepare shift register for bit shifting
  digitalWrite(DIN_PIN, 0);
  digitalWrite(CLK_PIN, 0);  
  delayMicroseconds(1);
  
  // Send data to the nixie driver 
  for (int i = 15; i >= 0; i--)
  {
    // Set high only the bit that corresponds to the current nixie cathode
    if(i == digit) digitalWrite(DIN_PIN, 1); 
    else digitalWrite(DIN_PIN, 0);
    delayMicroseconds(1);
    
    // Register shifts bits on upstroke of CLK pin 
    digitalWrite(CLK_PIN, 1);

    delayMicroseconds(1);
    //Set low the data pin after shift to prevent bleed through
    digitalWrite(CLK_PIN, 0);  
  }  
  delayMicroseconds(1);

  // Return the EN pin high to signal chip that it 
  // no longer needs to listen for data
  digitalWrite(EN_PIN, 1);
    
  // Stop shifting
  digitalWrite(CLK_PIN, 0);        
}

void setup() 
{
  Serial.begin(115200);
  Serial.println("Starting");
  pinMode(DIN_PIN, OUTPUT);
  digitalWrite(DIN_PIN, LOW);    
    
  pinMode(CLK_PIN, OUTPUT);
  digitalWrite(CLK_PIN, LOW);         
  
  pinMode(EN_PIN, OUTPUT);
  digitalWrite(EN_PIN, LOW);

  pinMode(BTN1_PIN, INPUT);
  pinMode(BTN2_PIN, INPUT);

  strip.setBrightness(25);
  strip.show();
}

void loop ()
{
  static uint8_t btn1_state = 0, btn2_state = 0;
  uint8_t btn_value;
  for(int i = 0; i <= 9; i++)
  {
    Serial.print("Number: ");
    Serial.println(i);
    NixieDisplay(i);                // Do simple counting
    NixieDisplay(i);                // Do simple counting
    NixieDisplay(i);                // Do simple counting
    NixieDisplay(i);                // Do simple counting
    for (int j = 0; j < 100; j++) {
      btn_value = digitalRead(BTN1_PIN);
      if (btn1_state && btn_value != btn1_state) {
        printf("BTN1 pressed\n");
      }
      btn1_state = btn_value;
      btn_value = digitalRead(BTN2_PIN);
      if (btn2_state && btn_value != btn2_state) {
        printf("BTN2 pressed\n");
      }
      btn2_state = btn_value;
      delay(1);
    }
  }
}