// Nixie Tube Driver by Marcin Saj https://nixietester.com
// https://github.com/marcinsaj/Nixie-Tube-Driver
//
// Driving Nixie Tube Example #1
//
// This example demonstrates how to control a nixie tube.
// The control is carried out using the Nixie Tube Driver.
#include <Arduino.h>
#include <Adafruit_Neopixel.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "timer.hpp"

#include <secrets.h>

#define DIN_PIN   13          // Nixie driver (shift register) serial data input pin             
#define CLK_PIN   14          // Nixie driver clock input pin
#define EN_PIN    15          // Nixie driver enable input pin
#define BTN1_PIN  0
#define BTN2_PIN  2
#define PIXEL_PIN 12 // Pin 16 does not work with Adafruit_NeoPixel!

Adafruit_NeoPixel strip = Adafruit_NeoPixel(6, PIXEL_PIN, NEO_GRB + NEO_KHZ400);
WiFiClient client;
WiFiUDP ntp_udp;
NTPClient time_client(ntp_udp);

ESP8266Timer ITimer;


#define TIMER_INTERVAL_MS 1000


void strip_number(uint8_t v) {
  uint32_t c;
  for(uint16_t i = 0; i < strip.numPixels(); i++) {
    uint8_t bit = i % 3;
    if ((v >> bit) & 1) {
      c = strip.Color(0, 255, 0);
    } else {
      c = strip.Color(255, 0, 0);
    }
    strip.setPixelColor(i, c);
  }
  strip.show();
}

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

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}


bool ntp_synced = false;
bool timer_triggered = false;
unsigned long long int seconds_since_ntp_sync = 0;
void IRAM_ATTR TimerHandler()
{
  if (ntp_synced)
    seconds_since_ntp_sync += 1;
  timer_triggered = true;
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

  strip.begin();
  strip.setBrightness(255);
  strip.show();

  WiFi.begin(ssid, pass);
  Serial.println("connecting");

  init_timer(&ITimer, &TimerHandler, TIMER_INTERVAL_MS);
}

void
update_clock_time()
{
  uint8_t h = time_client.getHours();
  uint8_t m = time_client.getMinutes();
  NixieDisplay(m % 10);
  NixieDisplay(m / 10);
  NixieDisplay(h % 10);
  NixieDisplay(h / 10);
  Serial.print("Updating time to ");
  Serial.println(time_client.getFormattedTime());
}

void
update_clock_counter()
{
  static uint8_t counter = 0;
  Serial.print("Number: ");
  Serial.println(counter);
  strip_number(counter);
  NixieDisplay(counter);
  NixieDisplay(counter);
  NixieDisplay(counter);
  NixieDisplay(counter);
  counter = (counter + 1) % 10;
}

void loop ()
{
  static uint8_t btn1_state = 0, btn2_state = 0;
  uint8_t btn_value;
  static wl_status_t connection_status = WL_DISCONNECTED;
  if (connection_status == WL_DISCONNECTED && WiFi.status() == WL_CONNECTED) {
    connection_status = WL_CONNECTED;
    Serial.println("WiFi Connected");
    printWiFiStatus();
    time_client.begin();
    time_client.update();
    Serial.print("Current time: ");
    Serial.println(time_client.getFormattedTime());
    ntp_synced = true;
  }

  if (timer_triggered) {
    timer_triggered = false;
    if (ntp_synced)
      update_clock_time();
    else
      update_clock_counter();
  }

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
}