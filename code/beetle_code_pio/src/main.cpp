#include "main.h"
#include "sensors.h"
#include <Adafruit_PWMServoDriver.h>
#include "sensor.h"

Adafruit_PWMServoDriver multiplexer = Adafruit_PWMServoDriver(0x40);
AsyncWebServer server(80);
String wifistr;
char lastInput[3] = "C";
int lastDists[3] = {0};
bool satDown = true;
int distSensorFront = 500; // 5 cm
int distSensorSide = 700; // 7 cm
bool movementType = true; // if sets to true, the robot rotates on itself to get to the direction
//false : goes in the direction without rotating, ie crabwalk / backwards

// create an easy-to-use handler
static AsyncWebSocketMessageHandler wsHandler;

// add it to the websocket server
static AsyncWebSocket ws("/ws", wsHandler.eventHandler());

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

//Default for manual control of joints
unsigned int offsetAngle = 5;

static bool multiplexerOk = true;
static unsigned long lastCheck = 0;

movements motion = movements(lf, rf, lm, rm, lb,rb, multiplexer, 20);
leg *current = motion.current;

String processor(const String& var){
  Serial.println(var);
  if(var == "ADDR"){
    return wifistr;
  }  
  return String();
}

void setup () {
    Serial.begin(9600);
    
    if (WEB_SERIAL) {
        //To check if the password and ssid are correct
        Serial.println(PROGMEM "Connecting to WiFi...");
        Serial.println(WIFI_SSID);
        
        WiFi.mode(WIFI_STA);
        WiFi.begin(WIFI_SSID, WIFI_PASS);
        
        if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.printf(PROGMEM "WiFi Failed!\n");
        return;
        }
        
        // WebSerial is accessible at "<IP Address>/webserial" from your browser
        Serial.print(PROGMEM "IP Address: ");
        Serial.println(WiFi.localIP());
        wifistr = WiFi.localIP().toString();

        if(!SPIFFS.begin(true)){
            Serial.println(PROGMEM "An Error has occurred while mounting SPIFFS");
            return;
        }
        
        server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
            request->send(SPIFFS, "/index.html", String(), false, processor);});

        server.on("/joy.js", HTTP_GET, [](AsyncWebServerRequest *request){
            request->send(SPIFFS, "/joy.js","text/javascript");});

        server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
            request->send(SPIFFS, "/style.css","text/css");});

        server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){
            request->send(SPIFFS, "/favicon.ico","image/x-icon");});

        server.on("/bot.png", HTTP_GET, [](AsyncWebServerRequest *request){
            request->send(SPIFFS, "/bot.png","image/png");});

        wsHandler.onMessage([](AsyncWebSocket *server, AsyncWebSocketClient *client, const uint8_t *data, size_t len) {
            if (strcmp((const char*) data, "MC")) {
                movementType = !movementType;
            } else memcpy(lastInput, data, 2);
        });


    server.addHandler(&ws);
    server.begin();
    }

    //Sensors code
    Wire.begin();
    Wire.setClock(400000); // use 400 kHz I2C
    forceSetup(); // after, the sensors should be all setup

    multiplexer.begin();
    multiplexer.setPWMFreq(60);

    ledcSetup(0, 50, 16);
    ledcSetup(1, 50, 16); 
    ledcAttachPin(servoPins[0][Tibia], 0); 
    ledcAttachPin(servoPins[1][Tibia], 1); 

    motion.initializeAllServos(standAngle[Coxa], standAngle[Femur], standAngle[Tibia]);
    printHelp();
}

bool isMultiplexerConnected(uint8_t address = 0x40) {
    Wire.beginTransmission(address);
    return (Wire.endTransmission() == 0);
}

void loop() {
    sleep(1);
    ws.cleanupClients();
    sendJson();

    for (int i = 0; i < 3; ++i) {
        int test = checkSensor(i);
        lastDists[i] = (test < 0) ? 0: test;
    }
    Serial.println("last Distances:");
    Serial.print(" 0: ");
    Serial.println(lastDists[0]);
    Serial.print(" 1: ");
    Serial.println(lastDists[1]);
    Serial.print(" 2: ");
    Serial.println(lastDists[2]);
    handleSerialInput();

    if (WEB_SERIAL) {
        switch (lastInput[0]){
            case 'N':
                if (satDown || lastDists[1] < distSensorFront) break;
                if (lastInput[1] == '\0') {
                    motion.forward();
                } else if (lastInput[1] == 'E') {
                    motion.forwardCurve(1, 0.5);
                } else if (lastInput[1] == 'W') {
                    motion.forwardCurve(-1, 0.5);
                }
                break;
            case 'S':
                if (satDown) break;
                if (lastInput[1] == '\0') {
                    if (movementType) {
                        motion.rotation(1, 7);
                        motion.forward();
                    } else {
                        motion.backward();
                    }
                } else if (lastInput[1] == 'E') {
                    if (movementType) {
                        motion.rotation(1, 5);
                        motion.forward();
                    } else {
                        motion.backwardCurve(1, 0.5);
                    }
                } else if (lastInput[1] == 'W') {
                    if (movementType) {
                        motion.rotation(-1, 5);
                        motion.forward();
                    } else {
                        motion.backwardCurve(-1, 0.5);
                    }
                }
                break;
            case 'E':
                if (satDown || lastDists[2] < distSensorSide) break;
                if (lastInput[1] == '\0') {
                    if (movementType) {
                        motion.rotation(-1, 4);
                        motion.sideways(0);
                    } else {
                        motion.sideways(0);
                    }
                }
                break;
            case 'W':
                if (satDown || lastDists[0] < distSensorSide) break;
                if (lastInput[1] == '\0') {
                    if (movementType) {
                        motion.rotation(1, 4);
                    motion.sideways(1);
                    } else {
                        //motion.sideways(1);
                    }
                }
                break;
            case 'U': 
                if (satDown) {
                    motion.standUp();
                    satDown = false;
                } else {
                    motion.sitDown();
                    satDown = true;
                }
                break;
            default:
                break;
        }
    } else {
        handleSerialInput();
    }

    
}

void handleWifiSerialInput(const uint8_t *data, size_t len) {
    String input = String((const char*) data);
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
        if(input == motion.legs[i]->name) {
          current = motion.legs[i];
          Sprint(PROGMEM "Selected leg: ");
          Sprintln(current->name.c_str());
          return;
        }
    }
    
    if (input.startsWith(PROGMEM "offset ")) {
        String offsetValue = input.substring(7); // Extract the value after "offset "
        int newOffset = offsetValue.toInt(); // Convert to integer
        if (newOffset > 0) { // Ensure it's a valid positive number
            offsetAngle = newOffset;
            char temp[5] = {0};
            itoa(offsetAngle, temp, 10);
            Sprint(PROGMEM "Offset angle set to: ");
            Sprintln(temp);
        } else {
            Sprintln(PROGMEM "Invalid offset value. Please provide a positive number.");
        }
    } else if (input == "forward") {
        motion.forward();
    } else if(input == "help") {
        printHelp();
    } else if(input == "stand") {
        motion.initializeAllServos(standAngle[Coxa], standAngle[Femur], standAngle[Tibia]);
    } else if(input == "sit") {
        motion.initializeAllServos(sitAngle[Coxa], sitAngle[Femur], sitAngle[Tibia]);
    } else if(input == "test set") {
        motion.initializeAllServos(setAngle[Coxa], setAngle[Femur], setAngle[Tibia]);
    } else if(input == "sit up") {
        motion.standUp();
    } else if(input == "rot cw") {
        motion.rotation(1, 5);
    } else if(input == "rot ccw") {
        motion.rotation(-1, 5);
    } else if(input == "forw cw") {
        motion.forwardCurve(1, 0.5);
    } else if(input == "forw ccw") {
        motion.forwardCurve(-1, 0.5);
    } else if(input == "side left") {
        motion.sideways(-1);
    } else if(input == "side right") {
        motion.sideways(1);
    } else {
        Sprintln(PROGMEM "Unknown command. Type 'help' for a list of commands.");
    }
  }

void handleKeyCommand(char key) {
    switch(key) {
      case 'q': 
        current->setAngle(current->pinC, motion.currAngles[current->index][Coxa] + offsetAngle);
        motion.currAngles[current->index][Coxa]= motion.currAngles[current->index][Coxa] + offsetAngle;
        break;
      case 'a': 
        current->setAngle(current->pinC, motion.currAngles[current->index][Coxa] - offsetAngle); 
        motion.currAngles[current->index][Coxa]= motion.currAngles[current->index][Coxa] -offsetAngle;
        break;
      case 'w': 
        current->setAngle(current->pinF, motion.currAngles[current->index][Femur] + offsetAngle); 
        motion.currAngles[current->index][Femur]= motion.currAngles[current->index][Femur] + offsetAngle;
        break;
      case 's': 
        current->setAngle(current->pinF, motion.currAngles[current->index][Femur] - offsetAngle); 
        motion.currAngles[current->index][Femur]= motion.currAngles[current->index][Femur] - offsetAngle;
        break;
      case 'e': 
        if (current->pinT>=0 && current->pinT<=15) {
            current->setAngle(current->pinT, motion.currAngles[current->index][Tibia] +offsetAngle); 
        } else {
            current->setAngle(current->pinT, motion.currAngles[current->index][Tibia] +offsetAngle, current->index); 
        }
        motion.currAngles[current->index][Tibia]= motion.currAngles[current->index][Tibia] + offsetAngle;
        break;
      case 'd': 
        if (current->pinT>=0 && current->pinT<=15) {
            current->setAngle(current->pinT, motion.currAngles[current->index][Tibia] -offsetAngle); 
        } else {
            current->setAngle(current->pinT, motion.currAngles[current->index][Tibia] -offsetAngle, current->index); 
        }
        motion.currAngles[current->index][Tibia]= motion.currAngles[current->index][Tibia] -offsetAngle ;
        break;
    }
    Sprint(current->name.c_str());
    Sprint(" - Coxa:");
    char temp[5] = {0};
    itoa(motion.currAngles[current->index][Coxa], temp, 10);
    Sprint(temp);
    Sprint("°, Femur:");
    itoa(motion.currAngles[current->index][Femur], temp, 10); // itoa creates null-terminated string so no need to clean
    Sprint(temp);
    Sprint("°, Tibia:");
    itoa(motion.currAngles[current->index][Tibia], temp, 10);
    Sprint(temp);
    Sprintln("°");
}


void printHelp() {
    Sprintln(PROGMEM "\nServo Control System:");
    Sprintln(PROGMEM "Quick commands (single key):");
    Sprintln(PROGMEM "Q/A - Coxa +/-");
    Sprintln(PROGMEM "W/S - Femur +/-");
    Sprintln(PROGMEM "E/D - Tibia +/-");
    Sprintln(PROGMEM "\nAdvanced commands:");
    Sprintln(PROGMEM "stand - Beetlebot in stand position [not a smooth movement]");
    Sprintln(PROGMEM "sit - Beetlebot in sit position [not a smooth movement]");
    Sprintln(PROGMEM "sit up - Beetlebot from sit to stand");
    Sprintln(PROGMEM "forward - tripod");
    Sprintln(PROGMEM "rot cw - clockwise tripod gait rotation");
    Sprintln(PROGMEM "rot ccw - counterclockwise tripod gait rotation");
    Sprintln(PROGMEM "forw cw - forward curve clockwise tripod gait");
    Sprintln(PROGMEM "forw ccw - forward curve counterclockwise tripod gait");
    Sprintln(PROGMEM "side left - sideways tripod gait to the left");
    Sprintln(PROGMEM "side right - sideways tripod gait to the right");
}

void Sprintln(const char* msg) {
    if (WEB_SERIAL) {
        ws.printfAll(msg);
    } else {
        Serial.println(msg);
    }
}

void Sprint(const char* msg) {
    if (WEB_SERIAL) {
        ws.printfAll(msg);
    } else {
        Serial.print(msg);
    }
    
}

void sendJson() {
    String temp = String(R"({"act":")") + String(lastInput) +
        String(R"(", "movement_type":)") + String(movementType) + 
        String(R"(", "dist0":)") + String(lastDists[0]) + 
        String(R"(, "dist1":)") + String(lastDists[1]) + 
        String(R"(, "dist2":)") + String(lastDists[2]) +  String(R"(})");
    ws.printfAll(temp.c_str());
}