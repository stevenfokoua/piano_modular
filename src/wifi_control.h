#pragma once
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

// WiFi AP credentials
#define WIFI_SSID "INT7-Piano"
#define WIFI_PASS "00000000"
#define HTTP_PORT 80

// WiFi AP Configuration
#define WIFI_CHANNEL 1       // Use channel 1 (less interference usually)
#define WIFI_MAX_CLIENTS 4   // Maximum number of connected clients

typedef void (*SetColorCallback)(uint8_t button, uint32_t color);

class WiFiControl {
public:
    WiFiControl(SetColorCallback cb) : setColor(cb), server(HTTP_PORT), ws("/ws") {}

    void begin() {
        // Initialize filesystem
        if(!LittleFS.begin(true)) {
            Serial.println("LittleFS Mount Failed");
            return;
        }
        Serial.println("LittleFS mounted successfully");
        
        // Ensure WiFi is disconnected and reset configuration
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
        delay(100);
        
        // Configure AP mode
        WiFi.mode(WIFI_AP);
        WiFi.setSleep(false);  // Disable power saving for better response
        
        // Configure AP settings with maximum power
        if(WiFi.softAP(WIFI_SSID, WIFI_PASS, WIFI_CHANNEL, false, WIFI_MAX_CLIENTS)) {
            Serial.println("\nWiFi Access Point Configuration:");
            Serial.println("--------------------------------");
            Serial.printf("SSID: %s\n", WIFI_SSID);
            Serial.printf("Password: %s\n", WIFI_PASS);
            Serial.printf("AP IP address: %s\n", WiFi.softAPIP().toString().c_str());
            Serial.printf("Channel: %d\n", WIFI_CHANNEL);
            Serial.printf("Max clients: %d\n", WIFI_MAX_CLIENTS);
            Serial.println("--------------------------------\n");
        } else {
            Serial.println("Failed to start WiFi AP!");
            return;
        }

        // Setup HTTP routes for web files
        server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
            request->send(LittleFS, "/index.html", "text/html");
        });

        server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request){
            request->send(LittleFS, "/index.html", "text/html");
        });

        server.on("/app.js", HTTP_GET, [](AsyncWebServerRequest *request){
            request->send(LittleFS, "/app.js", "application/javascript");
        });

        server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
            request->send(LittleFS, "/style.css", "text/css");
        });

        // Handle 404
        server.onNotFound([](AsyncWebServerRequest *request){
            request->send(404, "text/plain", "Not found");
        });

        // Setup WebSocket handler
        ws.onEvent([this](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
            if (type == WS_EVT_CONNECT) {
                Serial.printf("WebSocket client #%u connected from %s\n", 
                    client->id(), client->remoteIP().toString().c_str());
            } else if (type == WS_EVT_DISCONNECT) {
                Serial.printf("WebSocket client #%u disconnected\n", client->id());
            } else if (type == WS_EVT_DATA) {
                AwsFrameInfo *info = (AwsFrameInfo*)arg;
                if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
                    data[len] = 0;
                    StaticJsonDocument<128> doc;
                    DeserializationError err = deserializeJson(doc, (char*)data);
                    if (!err && doc["type"] == "setColor") {
                        uint8_t button = doc["button"];
                        const char* colorStr = doc["color"];
                        uint32_t color = (uint32_t)strtoul(colorStr + 1, nullptr, 16); // skip '#'
                        if (setColor) setColor(button, color);
                    }
                }
            }
        });

        server.addHandler(&ws);
        server.begin();
        Serial.println("HTTP and WebSocket servers started");
    }

    void loop() {
        ws.cleanupClients();
        
        // Print connected clients every 10 seconds
        static unsigned long lastCheck = 0;
        if (millis() - lastCheck > 10000) {
            lastCheck = millis();
            uint8_t stationCount = WiFi.softAPgetStationNum();
            Serial.printf("Connected stations: %d\n", stationCount);
        }
    }

private:
    SetColorCallback setColor;
    AsyncWebServer server;
    AsyncWebSocket ws;
};
