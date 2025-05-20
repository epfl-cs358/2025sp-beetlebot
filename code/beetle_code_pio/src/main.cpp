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

//Default for manual control of joints
unsigned int offsetAngle = 5;

movements motion = movements(lf, rf, lm, rm, lb,rb, multiplexer, 20);
leg *current = motion.current;

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

    motion.initializeAllServos(standAngle[Coxa], standAngle[Femur], standAngle[Tibia]);
    printHelp();
}

void loop() {
    if (!WEB_SERIAL) {
        handleSerialInput();
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
        if(input == motion.legs[i].name) {
          current = &motion.legs[i];
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
    } else if (input == "forward") {
        motion.forward();
    } else if(input == "help") {
        printHelp();
    } else if(input == "stand") {
        motion.initializeAllServos(standAngle[Coxa], standAngle[Femur], standAngle[Tibia]);
    } else if(input == "rotate cw") {
        motion.rotation(1);
    } else if(input == "rotate ccw") {
        motion.rotation(0);
    } else {
        Sprintln("Unknown command. Type 'help' for a list of commands.");
    }
  }

void handleKeyCommand(char key) {
    switch(key) {
      case 'q': 
        current->setAngle(current->pinC, offsetAngle, motion.currAngles[current->index][Coxa]);
        motion.currAngles[current->index][Coxa]= motion.currAngles[current->index][Coxa] + offsetAngle;
        break;
      case 'a': 
        current->setAngle(current->pinC, -offsetAngle, motion.currAngles[current->index][Coxa]); 
        motion.currAngles[current->index][Coxa]= motion.currAngles[current->index][Coxa] -offsetAngle;
        break;
      case 'w': 
        current->setAngle(current->pinF, offsetAngle, motion.currAngles[current->index][Femur]); 
        motion.currAngles[current->index][Femur]= motion.currAngles[current->index][Femur] + offsetAngle;
        break;
      case 's': 
        current->setAngle(current->pinF, -offsetAngle, motion.currAngles[current->index][Femur]); 
        motion.currAngles[current->index][Femur]= motion.currAngles[current->index][Femur] - offsetAngle;
        break;
      case 'e': 
        if (current->pinT>=0 && current->pinT<=15) {
            current->setAngle(current->pinT, offsetAngle, motion.currAngles[current->index][Tibia]); 
        } else {
            current->setAngle(current->pinT, offsetAngle, motion.currAngles[current->index][Tibia], current->index); 
        }
        motion.currAngles[current->index][Tibia]= motion.currAngles[current->index][Tibia] + offsetAngle;
        break;
      case 'd': 
        if (current->pinT>=0 && current->pinT<=15) {
            current->setAngle(current->pinT, -offsetAngle, motion.currAngles[current->index][Tibia]); 
        } else {
            current->setAngle(current->pinT, -offsetAngle, motion.currAngles[current->index][Tibia], current->index); 
        }
        motion.currAngles[current->index][Tibia]= motion.currAngles[current->index][Tibia] -offsetAngle ;
        break;
    }
    Sprint(current->name);
    Sprint(" - Coxa:");
    Sprint(motion.currAngles[current->index][Coxa]);
    Sprint("°, Femur:");
    Sprint(motion.currAngles[current->index][Femur]);
    Sprint("°, Tibia:");
    Sprint(motion.currAngles[current->index][Tibia]);
    Sprintln("°");
}


void printHelp() {
    Sprintln("\nServo Control System:");
    Sprintln("Quick commands (single key):");
    Sprintln("Q/A - Coxa +/-");
    Sprintln("W/S - Femur +/-");
    Sprintln("E/D - Tibia +/-");
    Sprintln("\nAdvanced commands:");
    Sprintln("stand - Beetlebot stands up");
    Sprintln("forward - tripod");
    Sprintln("rot cw - clockwise tripod gait rotation");
    Sprintln("rot ccw - counterclockwise tripod gait rotation");

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