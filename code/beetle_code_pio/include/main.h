#include "beetlebot.h"
#include "data.h"
#include "leg.h"


void handleSerialInput();
void handleTextCommand(String input);
void handleKeyCommand(char key); 
void initializeAllServos(float angleC, float angleF, float angleT);
void exampleSteps();
void printHelp();
void stupidWalk();
void interpolateAngle(int angle, leg leg, int pin, servoIndex servo);