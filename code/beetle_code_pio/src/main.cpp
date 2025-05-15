#include "main.h"
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver multiplexer = Adafruit_PWMServoDriver(0x40);
AsyncWebServer server(80);

leg lf = leg(multiplexer, lfCOffset, lfFOffset, lfTOffset, "lf", 0, 
    servoPins[0][Coxa], servoPins[0][Femur], servoPins[0][Tibia]);
leg rf = leg(multiplexer, rfCOffset, rfFOffset, rfTOffset, "rf", 1, 
    servoPins[1][Coxa], servoPins[1][Femur], servoPins[1][Tibia]); 
leg lm = leg(multiplexer, lmCOffset, lmFOffset, lmTOffset, "lm", 2, 
    servoPins[2][Coxa], servoPins[2][Femur], servoPins[2][Tibia]);
leg rm = leg(multiplexer, rmCOffset, rmFOffset, rmTOffset, "rm", 3, 
    servoPins[3][Coxa], servoPins[3][Femur], servoPins[3][Tibia]);
leg lb = leg(multiplexer, lbCOffset, lbFOffset, lbTOffset, "lb", 4, 
    servoPins[4][Coxa], servoPins[4][Femur], servoPins[4][Tibia]);
leg rb = leg(multiplexer, rbCOffset, rbFOffset, rbTOffset, "rb", 5, 
    servoPins[5][Coxa], servoPins[5][Femur], servoPins[5][Tibia]);     

beetlebot bot = beetlebot(lf, rf, lm, rm, lb, rb);


float legPositions[6][3] = {
    {homePositions[0], homePositions[1],homePositions[2]}, 
    {homePositions[0], homePositions[1],homePositions[2]},
    {homePositions[0], homePositions[1],homePositions[2]},
    {homePositions[0], homePositions[1],homePositions[2]},
    {homePositions[0], homePositions[1],homePositions[2]},
    {homePositions[0], homePositions[1],homePositions[2]}};



// preliminary leg positions before applying additional body movement
float currPositions[6][3];
// final leg positions for one iteration, passed to moveLegs()
float newPositions[6][3];

// keeps track of steps taken
int counter = 0;
// keeps track of number of iterations
long loopCounter = 0;

//Default for manual control of joints
unsigned int offsetAngle = 5;

// allows for basic path following
const int numberPoints = 4;
int waypoints[numberPoints][2] = { { 500, 0 },
                                       { 700, 300 },
                                       { 400, 800 },
                                       { 400, 1200 } };
        
//old code
leg* current = &rm;
leg legs [6] = {lf, rf, lm, rm, lb, rb};
int currAngles [6][3] = {
        {90, 90, 90},
        {90, 90, 90},
        {90, 90, 90},
        {90, 90, 90},
        {90, 90, 90},
        {90, 90, 90}};

bool walkMode;

void setup () {
    Serial.begin(9600);
    
    if (WEB_SERIAL) {
        //To check if the password and ssid are correct
        Serial.println("Connecting to WiFi...");
        Serial.println(WIFI_SSID);
        Serial.println(WIFI_PASS);
        
        WiFi.mode(WIFI_STA);
        WiFi.begin(WIFI_SSID, WIFI_PASS);
        
        if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.printf("WiFi Failed!\n");
        return;
        }
        
        // WebSerial is accessible at "<IP Address>/webserial" from your browser
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());

        WebSerial.begin(&server);
        WebSerial.msgCallback(handleWifiSerialInput);
        server.begin();
    }
    

    multiplexer.begin();
    multiplexer.setPWMFreq(60);

    ledcSetup(0, 50, 16);
    ledcSetup(1, 50, 16); 
    ledcAttachPin(servoPins[0][Tibia], 0); 
    ledcAttachPin(servoPins[1][Tibia], 1); 

    initializeAllServos(standAngle[Coxa], standAngle[Femur], standAngle[Tibia]);
    printHelp();
}

void loop() {
    if (!WEB_SERIAL) {
        handleSerialInput();
    }
    if (walkMode) {
        

        delay(1000);

        unsigned long timeMillis = millis();

        exampleSteps();

        //bot.travelPath(legPositions, waypoints, numberPoints, 12, 35);
        //while(true){}

        bot.moveLegs(newPositions);
        loopCounter++;
        if (loopCounter == 2500) {
            loopCounter = 0;
        }

        unsigned long millisCalc = millis();
        Sprint("Time calculated:");
        Sprintln(millisCalc - timeMillis);
        
        while (millis() < timeMillis + periodMs) {}
        
        Sprint("Time waited:");
        Sprintln(millis() - millisCalc);
    }

}

void handleWifiSerialInput(uint8_t *data, size_t len) {
    String input = "";
    for (size_t i = 0; i < len; i++) {
        input += char(data[i]);
    }
    input.trim();

    if (input.length() == 1) {
        handleKeyCommand(input.charAt(0));
    } else {
        handleTextCommand(input);
    }
}

// Depreciated serial input function, left for reference
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
    
    for(uint8_t i = 0; i < 6; i++) {
        if(input == legs[i].name) {
          current = &legs[i];
          Sprint("Selected leg: ");
          Sprintln(current->name);
          return;
        }
    }
    
    if (input.startsWith("offset ")) {
        String offsetValue = input.substring(7); // Extract the value after "offset "
        int newOffset = offsetValue.toInt(); // Convert to integer
        if (newOffset > 0) { // Ensure it's a valid positive number
            offsetAngle = newOffset;
            Sprint("Offset angle set to: ");
            Sprintln(offsetAngle);
        } else {
            Sprintln("Invalid offset value. Please provide a positive number.");
        }
    } else if (input == "gait") {
        walkMode = true;
        bot.moveHome();
    } else if (input == "test") {
        stupidWalk();
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
        Sprintln("Unknown command. Type 'help' for a list of commands.");
    }
  }

void handleKeyCommand(char key) {
    switch(key) {
      case 'q': 
        current->setAngle(current->pinC, offsetAngle, currAngles[current->index][Coxa]);
        currAngles[current->index][Coxa]= currAngles[current->index][Coxa] + offsetAngle;
        break;
      case 'a': 
        current->setAngle(current->pinC, -offsetAngle, currAngles[current->index][Coxa]); 
        currAngles[current->index][Coxa]= currAngles[current->index][Coxa] -offsetAngle;
        break;
      case 'w': 
        current->setAngle(current->pinF, offsetAngle, currAngles[current->index][Femur]); 
        currAngles[current->index][Femur]= currAngles[current->index][Femur] + offsetAngle;
        break;
      case 's': 
        current->setAngle(current->pinF, -offsetAngle, currAngles[current->index][Femur]); 
        currAngles[current->index][Femur]= currAngles[current->index][Femur] - offsetAngle;
        break;
      case 'e': 
        if (current->pinT>=0 && current->pinT<=15) {
            current->setAngle(current->pinT, offsetAngle, currAngles[current->index][Tibia]); 
        } else {
            current->setAngle(current->pinT, offsetAngle, currAngles[current->index][Tibia], current->index); 
        }
        currAngles[current->index][Tibia]= currAngles[current->index][Tibia] + offsetAngle;
        break;
      case 'd': 
        if (current->pinT>=0 && current->pinT<=15) {
            current->setAngle(current->pinT, -offsetAngle, currAngles[current->index][Tibia]); 
        } else {
            current->setAngle(current->pinT, -offsetAngle, currAngles[current->index][Tibia], current->index); 
        }
        currAngles[current->index][Tibia]= currAngles[current->index][Tibia] -offsetAngle ;
        break;
    }
    Sprint(current->name);
    Sprint(" - Coxa:");
    Sprint(currAngles[current->index][Coxa]);
    Sprint("°, Femur:");
    Sprint(currAngles[current->index][Femur]);
    Sprint("°, Tibia:");
    Sprint(currAngles[current->index][Tibia]);
    Sprintln("°");
}

void initializeAllServos(float angleC, float angleF, float angleT) {
    for(uint8_t i = 0; i < 6; i++) {
        legs[i].setAngle(servoPins[i][Coxa], 0, angleC); // 0 for offset bc haven't set it yet
        legs[i].setAngle(servoPins[i][Femur], 0, angleF);
        if (i==0) legs[i].setAngle(servoPins[i][Tibia], 0, angleT, 0);
        else if (i==1) legs[i].setAngle(servoPins[i][Tibia], 0, angleT, 1);
        else legs[i].setAngle(servoPins[i][Tibia], 0, angleT);

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
    
    //40 : radius around the coxa that the leg can't leave
    bot.computeStep(legPositions, currPositions, 0, 40, 120);

    // Adjust the body position each loop() iteration. This allows for superimposition of body movement independently of the step
    bot.computeBodyMov(currPositions, newPositions, 0, 0, 0);
}

//helper for stupid walk
void angleTab (int angles[6][3], int walk, int group2 [4][3], int group1 [4][3], bool moveLeft) {
    if (!moveLeft) {
        for (int j = 0; j < 3; ++j) {
            angles[0][j] = group2[walk][j];
            angles[1][j] = group1[walk][j];
            angles[2][j] = group1[walk][j];
            angles[3][j] = group2[walk][j];
            angles[4][j] = group2[walk][j];
            angles[5][j] = group1[walk][j];
        }
    } else {
        for (int j = 0; j < 3; ++j) {
            angles[0][j] = group1[walk][j];
            angles[1][j] = group2[walk][j];
            angles[2][j] = group2[walk][j];
            angles[3][j] = group1[walk][j];
            angles[4][j] = group1[walk][j];
            angles[5][j] = group2[walk][j];
        }
    }
}

void stupidWalk() {
    initializeAllServos(standAngle[Coxa], standAngle[Femur], standAngle[Tibia]);
    int coxaInterval = 25; //for legs that need to move towards each other, can do +- 25 from default pos
    int stepCounter = 35;
    bool movingLeft = true;

    leg body [6] = {lf, rf, lm, rm, lb, rb}; 
    //walk 1-5
    int group1[5][3] = {
        {100, 115, 145},
        {110, 70, 145},

        {100, 70, 145},
        {90, 70, 145}
    };

    int group2[5][3] = {
        {80, 70, 145},
        {70, 70, 145},

        {80, 115, 145},
        {90, 70, 145}
    };

    int  walk [6][3];

    for (int i = 0; i< 4; ++i) {

        angleTab(walk, 0, group2, group1, movingLeft);
        //walk1
        interpolateAngle(body, walk, stepCounter); 
        
        angleTab(walk, 1, group2, group1, movingLeft);
        //walk2
        interpolateAngle(body, walk, stepCounter); 
    
        angleTab(walk, 2, group2, group1, movingLeft);
        //walk 3
        interpolateAngle(body, walk, stepCounter); 

        angleTab(walk, 3, group2, group1, movingLeft);
        //walk4
        interpolateAngle(body, walk, stepCounter);  
        
        angleTab(walk, 4, group2, group1, movingLeft);
        //walk5
        interpolateAngle(body, walk, stepCounter);  

        movingLeft = !movingLeft;

    }
}

void interpolateAngle(leg body [6], int finalAngles [6][3], int stepNumber) {
    
    //current angles of the legs
    int from [6][3] = 
        {
            {currAngles[0][Coxa], currAngles[0][Femur], currAngles[0][Tibia]},
            {currAngles[1][Coxa], currAngles[1][Femur], currAngles[1][Tibia]},
            {currAngles[2][Coxa], currAngles[2][Femur], currAngles[2][Tibia]},
            {currAngles[3][Coxa], currAngles[3][Femur], currAngles[3][Tibia]},
            {currAngles[4][Coxa], currAngles[4][Femur], currAngles[4][Tibia]},
            {currAngles[5][Coxa], currAngles[5][Femur], currAngles[5][Tibia]},
        };


    for (int i = 0; i<=stepNumber; ++i) {

        int angles [6][3] = {
            //lf
            {map(i, 0, stepNumber, from[0][Coxa], finalAngles[0][Coxa]),
            map(i, 0, stepNumber, from[0][Femur], finalAngles[0][Femur]),
            map(i, 0, stepNumber, from[0][Tibia], finalAngles[0][Tibia])},
            //rf
            {map(i, 0, stepNumber, from[1][Coxa], 180 - finalAngles[1][Coxa]),
            map(i, 0, stepNumber, from[1][Femur], finalAngles[1][Femur]),
            map(i, 0, stepNumber, from[1][Tibia], finalAngles[1][Tibia])},
            
            {map(i, 0, stepNumber, from[2][Coxa], finalAngles[2][Coxa]),
            map(i, 0, stepNumber, from[2][Femur], finalAngles[2][Femur]),
            map(i, 0, stepNumber, from[2][Tibia], finalAngles[2][Tibia])},
            {map(i, 0, stepNumber, from[3][Coxa], 180 - finalAngles[3][Coxa]),
            map(i, 0, stepNumber, from[3][Femur], finalAngles[3][Femur]),
            map(i, 0, stepNumber, from[3][Tibia], finalAngles[3][Tibia])},
            {map(i, 0, stepNumber, from[4][Coxa], finalAngles[4][Coxa]),
            map(i, 0, stepNumber, from[4][Femur], finalAngles[4][Femur]),
            map(i, 0, stepNumber, from[4][Tibia], finalAngles[4][Tibia])},
            {map(i, 0, stepNumber, from[5][Coxa], 180 - finalAngles[5][Coxa]),
            map(i, 0, stepNumber, from[5][Femur], finalAngles[5][Femur]),
            map(i, 0, stepNumber, from[5][Tibia], finalAngles[5][Tibia])},
        };

        for (int j = 0; j < 6; ++j) {
            if (i == stepNumber) {
                body[j].setAngle(body[j].pinC, 0, finalAngles[j][Coxa]);
                body[j].setAngle(body[j].pinF, 0, finalAngles[j][Femur]);
                body[j].setAngle(body[j].pinT, 0, finalAngles[j][Tibia], j);
            } else {
                body[j].setAngle(body[j].pinC, 0, angles[j][Coxa]);
                body[j].setAngle(body[j].pinF, 0, angles[j][Femur]);
                body[j].setAngle(body[j].pinT, 0, angles[j][Tibia],j);
            }
        }
        delay(SWEEP_DELAY);
    
    }

    for (int i = 0; i<6; ++i) {
        currAngles[i][Coxa] = finalAngles[i][Coxa];
        currAngles[i][Femur] = finalAngles[i][Femur];
        currAngles[i][Tibia] = finalAngles[i][Tibia];
    }

}


void printHelp() {
    Sprintln("\nServo Control System:");
    Sprintln("Quick commands (single key):");
    Sprintln("Q/A - Coxa +/-");
    Sprintln("W/S - Femur +/-");
    Sprintln("E/D - Tibia +/-");
    Sprintln("\nAdvanced commands:");
    Sprintln("wave - Beetlebot waves at you animation");
    Sprintln("sit - Beetlebot sits down");
    Sprintln("stand - Beetlebot stands up");
    Sprintln("gait - walking gait pattern");
    Sprintln("test - stupid tripod");
}

template<typename T>
void Sprintln(T msg) {
    if (WEB_SERIAL) {
        WebSerial.println(msg);
    } else {
        Serial.println(msg);
    }
}

template<typename T>
void Sprint(T msg) {
    if (WEB_SERIAL) {
        WebSerial.print(msg);
    } else {
        Serial.print(msg);
    }
}