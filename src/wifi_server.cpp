#include "wifi_server.hpp"
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


typedef enum html_status_code {
    HTTP_OK = 200,
    HTTP_NOT_FOUND = 404,
} html_status_code_t;


static char*
status_code_to_str(int status_code)
{
    switch(status_code) {
        case HTTP_OK:
            return (char*) "OK";
            break;
        case HTTP_NOT_FOUND:
            return (char*) "Not Found";
            break;
        default:
            assert(false);
    }
}


static void
respond(WiFiClient &client, int status, String &html)
{
    html += "\r\n\r\n";
    client.printf(
        "HTTP/1.1 %d %s\n"
        "Content-type: text/html\n"
        "Content-Length: %d\r\n"
        "\r\n"
        "%s",
        status, status_code_to_str(status), html.length(), html.c_str()
    );
}


static bool was_connected = false;


void
wifi_server_service() {
    assert(initialized);

    WiFiClient client = server.available();
    bool is_connected = client.connected();
    if (!is_connected && !was_connected) {
        return;
    } else if (is_connected && !was_connected) {
        Serial.println("New WiFi client");
        start_time = millis();
        String html = "<!DOCTYPE html><html><body>"
            "<h1>HEJ</h1>"
            "</body></html>";
        respond(client, 200, html);
    } else if (is_connected && was_connected || !is_connected) {
        Serial.println("Checking for timeout");
        if ((millis() - start_time) >= timeout_time) {
            Serial.println("WiFi Timeout");
            client.stop();
        }
    }

    was_connected = client.connected();
}
