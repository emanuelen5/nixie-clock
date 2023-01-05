#include "wifi.hpp"
#include "WiFiServer.h"
#include "WiFiClient.h"


static WiFiServer server(80);
static uint8_t initialized = false;
static unsigned long start_time = 0;
static unsigned long timeout_time = 100;

void
wifi_server_init()
{
    assert(!initialized);
    initialized = true;
    server.begin();
}

void
wifi_server_service() {
    assert(initialized);
    WiFiClient client = server.available();
    if (!client)
        return;

    Serial.println("New WiFi client");
    String header = "";
    String current_line = "";
    start_time = millis();

    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println("Connection: close");
    client.println();

    client.println("<!DOCTYPE html><html><body>");
    client.println("<h1>HEJ</h1>");
    client.println("</body></html>");
    client.println("");

    while (client.connected()) {
        if ((millis() - start_time) >= timeout_time) {
            Serial.println("WiFi Timeout");
            break;
        }
    }

exit:
    client.stop();
    Serial.println("WiFi client disconnected");
}