#include "main.h"
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver multiplexer = Adafruit_PWMServoDriver(0x40);

leg lf = leg(multiplexer, offsets[Coxa], offsets[Femur], offsets[Tibia], "lf", 0, 
    servoPins[0][Coxa], servoPins[0][Femur], servoPins[0][Tibia]);
leg rf = leg(multiplexer, offsets[Coxa], offsets[Femur], offsets[Tibia], "rf", 1, 
    servoPins[1][Coxa], servoPins[1][Femur], servoPins[1][Tibia]); 
leg lm = leg(multiplexer, offsets[Coxa], offsets[Femur], offsets[Tibia], "lm", 2, 
    servoPins[2][Coxa], servoPins[2][Femur], servoPins[2][Tibia]);
leg rm = leg(multiplexer, offsets[Coxa], offsets[Femur], offsets[Tibia], "rm", 3, 
    servoPins[3][Coxa], servoPins[3][Femur], servoPins[3][Tibia]);
leg lb = leg(multiplexer, offsets[Coxa], offsets[Femur], offsets[Tibia], "lb", 4, 
    servoPins[4][Coxa], servoPins[4][Femur], servoPins[4][Tibia]);
leg rb = leg(multiplexer, offsets[Coxa], offsets[Femur], offsets[Tibia], "rb", 5, 
    servoPins[5][Coxa], servoPins[5][Femur], servoPins[5][Tibia]);     

beetlebot bot = beetlebot(lf, rf, lm, rm, lb, rb);


float legPositions[6][3] = { 
    {homePositions[0][0], homePositions[0][1],homePositions[0][2]}, 
    {homePositions[1][0], homePositions[1][1],homePositions[1][2]}, 
    {homePositions[2][0], homePositions[2][1],homePositions[2][2]}, 
    {homePositions[3][0], homePositions[3][1],homePositions[3][2]}, 
    {homePositions[4][0], homePositions[4][1],homePositions[4][2]}, 
    {homePositions[5][0], homePositions[5][1],homePositions[5][2]}, 
};


// preliminary leg positions before applying additional body movement
float currPositions[6][3];
// final leg positions for one iteration, passed to moveLegs()
float newPositions[6][3];

// keeps track of steps taken
int counter = 0;
// keeps track of number of iterations
long loopCounter = 0;

// allows for basic path following
const int numberPoints = 4;
int waypoints[numberPoints][2] = { { 500, 0 },
                                       { 700, 300 },
                                       { 400, 800 },
                                       { 400, 1200 } };
        
//old code
leg current = rm;
leg legs [6] = {lf, rf, lm, rm, lb, rb};
float currAngles [6][3];
bool walkMode;

void setup () {
    Serial.begin(9600);
  
    multiplexer.begin();
    multiplexer.setPWMFreq(60);

    ledcSetup(0, 50, 16);
    ledcSetup(1, 50, 16); 
    ledcAttachPin(servoPins[0][Tibia], 0); 
    ledcAttachPin(servoPins[1][Tibia], 1); 

    initializeAllServos(sitAngle[Coxa], sitAngle[Femur], sitAngle[Tibia]);
    printHelp();
}

void loop() {
    handleSerialInput();

    if (walkMode) {
        bot.moveHome();

        delay(1000);

        unsigned long timeMillis = millis();

        exampleSteps();

        bot.travelPath(legPositions, waypoints, numberPoints, 12, 35);
        //while(true){}

        bot.moveLegs(newPositions);

        loopCounter++;
        if (loopCounter == 2500) {
            loopCounter = 0;
        }

        unsigned long millisCalc = millis();
        Serial.print("Time calculated:");
        Serial.println(millisCalc - timeMillis);
        while (millis() < timeMillis + periodMs) {}
        Serial.print("Time waited:");
        Serial.println(millis() - millisCalc);
    }

}

void handleSerialInput() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    if (input.length() == 1) {
      handleKeyCommand(input.charAt(0));
    } else {
      handleTextCommand(input);
    }
  }
}

void handleTextCommand(String input) {
    input.toLowerCase();
    
    if (input == "gait") {
        walkMode = true;
    } else if (input == "quit") {
        walkMode = false;
    } else if(input == "help") {
        printHelp();
    } else if(input == "stand") {
        initializeAllServos(standAngle[Coxa], standAngle[Femur], standAngle[Tibia]);
    } else if(input == "sit") {
        initializeAllServos(sitAngle[Coxa], sitAngle[Femur], sitAngle[Tibia]);
    } else if(input == "curl") {
        initializeAllServos(curlAngle[Coxa], curlAngle[Femur], curlAngle[Tibia]);
    } else {
        Serial.println("Unknown command. Type 'help' for a list of commands.");
    }
  }

void handleKeyCommand(char key) {
    switch(key) {
      case 'q': current.setAngle(current.pinC, 5, currAngles[current.index][Coxa]); break;
      case 'a': current.setAngle(current.pinC, -5, currAngles[current.index][Coxa]); break;
      case 'w': current.setAngle(current.pinF, 5, currAngles[current.index][Femur]); break;
      case 's': current.setAngle(current.pinF, -5, currAngles[current.index][Femur]); break;
      case 'e': current.setAngle(current.pinT, 5, currAngles[current.index][Tibia]); break;
      case 'd': current.setAngle(current.pinT, -5, currAngles[current.index][Tibia]); break;
    }
}

void initializeAllServos(float angleC, float angleF, float angleT) {
    for(uint8_t i = 0; i < 6; i++) {
        legs[i].setAngle(servoPins[i][Coxa], 5, angleC);
        legs[i].setAngle(servoPins[i][Femur], 5, angleF);
        if (i==0) legs[i].setAngle(servoPins[i][Tibia], 5, angleT, 0);
        else if (i==1) legs[i].setAngle(servoPins[i][Tibia], 5, angleT, 1);
        else legs[i].setAngle(servoPins[i][Tibia], 5, angleT);

        currAngles[i][Coxa] = angleC;
        currAngles[i][Femur] = angleF;
        currAngles[i][Tibia] = angleT;
    }
    delay(SWEEP_DELAY); 
}

void exampleSteps() {
    if (bot.getAction() == 0) {
        // increase the counter each time the robot is resting
        counter++;
        if (counter == 25) {
          counter = 0;
        }
    }
    
      // do a couple steps in different directions
    if (counter < 5) {
        bot.computeStep(legPositions, currPositions, 0, 30, 120);
    } else if (counter < 15) {
        bot.computeStep(legPositions, currPositions, PI / 2, 20, 120);
    } else if (counter < 20) {
        bot.computeStep(legPositions, currPositions, 0, 20, 120);
    } else {
        bot.computeStep(legPositions, currPositions, PI, 3, 120);
    }

    // Adjust the body position each loop() iteration. This allows for superimposition of body movement independently of the step
    bot.computeBodyMov(currPositions, newPositions, 0, 0, 0);
}


void printHelp() {
    Serial.println("\nServo Control System:");
    Serial.println("Quick commands (single key):");
    Serial.println("Q/A - Coxa +/-");
    Serial.println("W/S - Femur +/-");
    Serial.println("E/D - Tibia +/-");
    Serial.println("\nAdvanced commands:");
    Serial.println("wave - Beetlebot waves at you animation");
    Serial.println("sit - Beetlebot sits down");
    Serial.println("stand - Beetlebot stands up");
    Serial.println("gait - walking gait pattern");
}     