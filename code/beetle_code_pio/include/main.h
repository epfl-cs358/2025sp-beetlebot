#include "data.h"
#include "leg.h"
#include "movements.h"
#include "wifi_credentials.h"

const bool WEB_SERIAL = false; // Set to true to enable WebSerial, if false normal serial will be used


void handleWifiSerialInput(uint8_t *data, size_t len);
void handleSerialInput(); // Depreciated serial input function, left for reference
void handleTextCommand(String input);
void handleKeyCommand(char key); 
void printHelp();

template<typename T>
void Sprintln(T msg);

template<typename T>
void Sprint(T msg);