#include <Arduino.h>
#include <WiFi.h>

const int PORT = 80;
const char *SSID = "";
const char *PASSWORD = "";
const char* RESPONSE_HTML = "<!DOCTYPE html>\n<html>\n<head><title>Simple HTTP Server</title></head>\n<body><h1>Hello, World!</h1></body>\n</html>";

WiFiServer server(PORT);

void handleRequest(WiFiClient client) {
    String request = client.readStringUntil('\r');
    Serial.println(request);

    String response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: text/html\r\n";
    response += "Content-Length: " + String(strlen(RESPONSE_HTML)) + "\r\n";
    response += "\r\n" + String(RESPONSE_HTML);

    client.print(response);
    client.stop();
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.print("Connecting to WiFi");
    WiFi.begin(SSID, PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }

    Serial.println("\nConnected to WiFi");
    Serial.println("IP Address: " + WiFi.localIP().toString());

    server.begin();
}

void loop() {
    WiFiClient client = server.available();
    if (client) {
        handleRequest(client);
    }
}
