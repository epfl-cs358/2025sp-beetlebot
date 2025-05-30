#include "data.h"
#include "leg.h"
#include "movements.h"
#include "wifi_credentials.h"
#include "SPIFFS.h"
#include <WiFi.h>                         // ESP32 WiFi support
#include <AsyncTCP.h>                     // Required for AsyncWebServer on ESP32
#include <ESPAsyncWebServer.h>
#include "sensors.h"  

const bool WEB_SERIAL = true; // Set to true to enable WebSerial, if false normal serial will be used

String processor(const String& var);

void handleWifiSerialInput(const uint8_t *data, size_t len);
void handleSerialInput(); // Deprecated serial input function, left for reference
void handleTextCommand(String input);
void handleKeyCommand(char key); 
void printHelp();

void Sprintln(const char* msg);

void Sprint(const char* msg);
void sendJson();