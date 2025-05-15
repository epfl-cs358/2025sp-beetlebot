#include "beetlebot.h"
#include "data.h"
#include "leg.h"
#include "wifi_credentials.h"

const bool WEB_SERIAL = false; // Set to true to enable WebSerial, if false normal serial will be used


void handleWifiSerialInput(uint8_t *data, size_t len);
void handleSerialInput(); // Depreciated serial input function, left for reference
void handleTextCommand(String input);
void handleKeyCommand(char key); 
void initializeAllServos(float angleC, float angleF, float angleT);
void exampleSteps();
void printHelp();
void stupidWalk();
void interpolateAngle(leg body [6], int finalAngles [6][3], int stepNumber);

template<typename T>
void Sprintln(T msg);

template<typename T>
void Sprint(T msg);