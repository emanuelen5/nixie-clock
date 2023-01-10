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

// For WiFi Manager
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

#include "timer.hpp"
#include "button.hpp"
#include "nixie.hpp"
#include "wifi.hpp"

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
ESP8266Timer timer;
button_t btn1, btn2;
nixie_t nixie;


#define SECONDS_PER_HOUR 3600
#define MS_PER_SECOND 1000
#define TIMER_INTERVAL_MS (1 * MS_PER_SECOND)


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


void
strip_fill(uint32_t color)
{
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
  }
  strip.show();
}


void print_wifi_status() {
  Serial.println("WiFi connection details:");
  Serial.print("- SSID: ");
  Serial.println(WiFi.SSID());
  Serial.print("- IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("- Signal strength (RSSI): ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");
}


static bool blink_on = false;
void IRAM_ATTR blink_strip()
{
  if (blink_on) {
    strip_fill(strip.Color(255, 255, 255));
  } else {
    strip_fill(strip.Color(0, 0, 0));
  }
  strip.show();
  blink_on = ! blink_on;
}

void config_mode_callback(WiFiManager *wifi_manager) {
  Serial.println("WiFi Manager entered config mode");
  Serial.print("- IP: ");
  Serial.println(WiFi.softAPIP());
  Serial.print("- SSID: ");
  Serial.println(wifi_manager->getConfigPortalSSID());

  strip.setBrightness(10);
  set_timer(&timer, &blink_strip, TIMER_INTERVAL_MS);
}


bool timer_triggered = false;
void IRAM_ATTR tick_handler()
{
  timer_triggered = true;
}


void setup()
{
  Serial.begin(115200);
  Serial.println("Starting");

  strip.begin();
  strip_fill(strip.Color(0, 10, 50));
  strip.show();

  WiFiManager wifi_manager;
  wifi_manager.setDebugOutput(false);
  wifi_manager.setAPCallback(config_mode_callback);
  if (!wifi_manager.autoConnect("Emaus Nixie clock")) {
    Serial.println("Failed to connect and hit timeout");
  }

  strip.setBrightness(255);

  nixie_init(&nixie, CLK_PIN, DIN_PIN, EN_PIN);
  button_init(&btn1, BTN1_PIN, LOW);
  button_init(&btn2, BTN2_PIN, LOW);

  Serial.println("connecting");
  wifi_server_init();

  set_timer(&timer, &tick_handler, TIMER_INTERVAL_MS);
  time_client.begin();
  time_client.setTimeOffset(1 * SECONDS_PER_HOUR);
  time_client.setUpdateInterval(60 * MS_PER_SECOND);
}


void
update_clock_time()
{
  uint8_t h = time_client.getHours();
  uint8_t m = time_client.getMinutes();
  nixie_display_number(&nixie, h / 10, h % 10, m / 10, m % 10);
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
  nixie_display_number(&nixie, counter, counter, counter, counter);
  counter = (counter + 1) % 10;
}


void
loop()
{
  static wl_status_t connection_status = WL_DISCONNECTED;
  if (connection_status == WL_DISCONNECTED && WiFi.status() == WL_CONNECTED) {
    connection_status = WL_CONNECTED;
    Serial.println("WiFi Connected");
    strip_fill(strip.Color(255, 30, 5));
    print_wifi_status();
  }

  if (WiFi.status() == WL_CONNECTED && time_client.update()) {
    Serial.print("Time synchronized: ");
    Serial.println(time_client.getFormattedTime());
  }

  if (timer_triggered) {
    timer_triggered = false;
    if (time_client.isTimeSet())
      update_clock_time();
    else
      update_clock_counter();
  }

  wifi_server_service();

  button_service(&btn1);
  button_service(&btn2);
  if (button_was_pressed(&btn1)) {
    printf("BTN1 pressed\n");
  }
  if (button_was_pressed(&btn2)) {
    printf("BTN2 pressed\n");
  }
}