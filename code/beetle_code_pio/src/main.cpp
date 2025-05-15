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
float currAngles [6][3] = {
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

    initializeAllServos(sitAngle[Coxa], sitAngle[Femur], sitAngle[Tibia]);
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
        currAngles[current->index][Coxa]= currAngles[current->index][Coxa] - offsetAngle;
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

void stupidWalk() {
    
    
}

void interpolateAngle (int angle, leg leg, int pin, servoIndex servo) {
    int from = currAngles[leg.index][servo];
    if (from > angle) {
        if (pin<16) {
            for (int i = from-1; i>=angle; i--){
                leg.setAngle(pin, 0, i);
                delay(20);
            }
        } else {
            for (int i = from-1; i>=angle; i--){
                leg.setAngle(pin, 0, i, leg.index);
                delay(20);
            }
        }
    } else {
        if (pin<16) {
            for (int i = from+1; i<=angle; i++){
                leg.setAngle(pin, 0, i);
                delay(20);
            }
        } else {
            for (int i = from+1; i<=angle; i++){
                leg.setAngle(pin, 0, i, leg.index);
                delay(20);
            }
        }
    }
    currAngles[leg.index][servo] = angle;
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