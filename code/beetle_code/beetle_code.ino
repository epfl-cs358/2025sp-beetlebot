#include "beetle_hdr.h"
#include <Adafruit_PWMServoDriver.h>
Adafruit_PWMServoDriver board1 = Adafruit_PWMServoDriver(0x40);

#define SERVOMIN  125
#define SERVOMAX  625
#define SWEEP_DELAY 20
#define PWM_FREQ 60

// Servo control structure for each leg
struct Leg {
  String name;
  uint8_t coxaPin;
  uint8_t femurPin;
  uint8_t tibiaPin;
  int coxaAngle;
  int femurAngle;
  int tibiaAngle;
};

// Initialize all legs with hardware-defined pins
Leg legs[6] = {
  {"rf", cRFCoxaPin, cRFFemurPin, cRFTibiaPinESP, COXAS_START_ANGLE, FEMURS_START_ANGLE, TIBIAS_START_ANGLE},
  {"rm", cRMCoxaPin, cRMFemurPin, cRMTibiaPin, COXAS_START_ANGLE, FEMURS_START_ANGLE, TIBIAS_START_ANGLE},
  {"rb", cRBCoxaPin, cRBFemurPin, cRBTibiaPin, COXAS_START_ANGLE, FEMURS_START_ANGLE, TIBIAS_START_ANGLE},
  {"lf", cLFCoxaPin, cLFFemurPin, cLFTibiaPinESP, COXAS_START_ANGLE, FEMURS_START_ANGLE, TIBIAS_START_ANGLE},
  {"lm", cLMCoxaPin, cLMFemurPin, cLMTibiaPin, COXAS_START_ANGLE, FEMURS_START_ANGLE, TIBIAS_START_ANGLE},
  {"lb", cLBCoxaPin, cLBFemurPin, cLBTibiaPin, COXAS_START_ANGLE, FEMURS_START_ANGLE, TIBIAS_START_ANGLE}
};

Leg* currentLeg = &legs[1]; // Default to right middle (rm)

void setup() {
  Serial.begin(9600);
  
  // Initialize PCA9685
  board1.begin();
  board1.setPWMFreq(PWM_FREQ);
  
  // Initialize all servos to start positions
  initializeAllServos();
  printHelp();
}

void loop() {
  handleSerialInput();
}

//=== Servo Control Functions ===//
void setServoAngle(uint8_t pin, int angle) {
  if(pin < 16) { // PCA9685 pins
    board1.setPWM(pin, 0, angleToPulse(angle));
  }
  // Add ESP32 pin handling here if needed
}

void updateServo(uint8_t pin, int step, int &angle) {
  int newAngle = constrain(angle + step, 0, 180);
  if(newAngle != angle) {
    angle = newAngle;
    setServoAngle(pin, angle);
    printCurrentLeg();
  }
}

//=== Command Handling ===//
void handleSerialInput() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    input.toLowerCase();

    if(input.length() == 1) {
      handleKeyCommand(input.charAt(0));
    } else {
      handleTextCommand(input);
    }
  }
}

void handleTextCommand(String input) {
  // Check for leg selection
  for(uint8_t i = 0; i < 6; i++) {
    if(input == legs[i].name) {
      currentLeg = &legs[i];
      Serial.print("Selected leg: ");
      Serial.println(currentLeg->name);
      return;
    }
  }

  if(input == "wave") {
    performWave();
  }
  else if(input == "help") {
    printHelp();
  }
  else if(input.endsWith("sweep")) {
    handleSweepCommand(input);
  }
}

void handleKeyCommand(char key) {
  switch(key) {
    case 'q': updateServo(currentLeg->coxaPin, 5, currentLeg->coxaAngle); break;
    case 'a': updateServo(currentLeg->coxaPin, -5, currentLeg->coxaAngle); break;
    case 'w': updateServo(currentLeg->femurPin, 5, currentLeg->femurAngle); break;
    case 's': updateServo(currentLeg->femurPin, -5, currentLeg->femurAngle); break;
    case 'e': updateServo(currentLeg->tibiaPin, 5, currentLeg->tibiaAngle); break;
    case 'd': updateServo(currentLeg->tibiaPin, -5, currentLeg->tibiaAngle); break;
  }
}

//=== Wave Function ===//
void performWave() {
  // Save original positions
  int originalAngles[6][3];
  for(uint8_t i = 0; i < 6; i++) {
    originalAngles[i][0] = legs[i].coxaAngle;
    originalAngles[i][1] = legs[i].femurAngle;
    originalAngles[i][2] = legs[i].tibiaAngle;
  }

  // Reset all legs to start positions
  initializeAllServos();

  // Wave both middle legs
  Leg* waveLegs[] = {&legs[1], &legs[4]}; // RM and LM
  
  for(uint8_t i = 0; i < 3; i++) {
    // Raise femur
    for(auto leg : waveLegs) {
      smoothMove(leg->femurPin, 100, leg->femurAngle, 30);
    }
    
    // Wave motion
    coordinatedSweep(130, 50, 45, 25, waveLegs);
    coordinatedSweep(50, 130, 25, 45, waveLegs);
    
    // Lower femur
    for(auto leg : waveLegs) {
      smoothMove(leg->femurPin, FEMURS_START_ANGLE, leg->femurAngle, 30);
    }
  }

  // Restore original positions
  for(uint8_t i = 0; i < 6; i++) {
    smoothMove(legs[i].coxaPin, originalAngles[i][0], legs[i].coxaAngle, 30);
    smoothMove(legs[i].femurPin, originalAngles[i][1], legs[i].femurAngle, 30);
    smoothMove(legs[i].tibiaPin, originalAngles[i][2], legs[i].tibiaAngle, 30);
  }
}

//=== Utility Functions ===//
void initializeAllServos() {
  for(uint8_t i = 0; i < 6; i++) {
    setServoAngle(legs[i].coxaPin, COXAS_START_ANGLE);
    setServoAngle(legs[i].femurPin, FEMURS_START_ANGLE);
    setServoAngle(legs[i].tibiaPin, TIBIAS_START_ANGLE);
    legs[i].coxaAngle = COXAS_START_ANGLE;
    legs[i].femurAngle = FEMURS_START_ANGLE;
    legs[i].tibiaAngle = TIBIAS_START_ANGLE;
  }
  delay(500);
}

void coordinatedSweep(int coxaStart, int coxaEnd, int tibiaStart, int tibiaEnd, Leg** waveLegs) {
  const uint8_t steps = 40;
  for(uint8_t i = 0; i <= steps; i++) {
    float ratio = (float)i / steps;
    
    for(auto leg : waveLegs) {
      int coxa = coxaStart + (coxaEnd - coxaStart) * ratio;
      int tibia = tibiaStart + (tibiaEnd - tibiaStart) * ratio;
      
      setServoAngle(leg->coxaPin, coxa);
      setServoAngle(leg->tibiaPin, tibia);
      leg->coxaAngle = coxa;
      leg->tibiaAngle = tibia;
    }
    delay(SWEEP_DELAY);
  }
}

void smoothMove(uint8_t pin, int targetAngle, int &currentAngle, uint8_t stepSize) {
  while(abs(currentAngle - targetAngle) > stepSize) {
    currentAngle += (targetAngle > currentAngle) ? stepSize : -stepSize;
    setServoAngle(pin, currentAngle);
    delay(SWEEP_DELAY);
  }
  currentAngle = targetAngle;
  setServoAngle(pin, currentAngle);
}

//=== Helper Functions ===//
int angleToPulse(int ang) {
  return map(ang, 0, 180, SERVOMIN, SERVOMAX);
}

void printCurrentLeg() {
  Serial.print(currentLeg->name);
  Serial.print(" - Coxa:");
  Serial.print(currentLeg->coxaAngle);
  Serial.print("°, Femur:");
  Serial.print(currentLeg->femurAngle);
  Serial.print("°, Tibia:");
  Serial.print(currentLeg->tibiaAngle);
  Serial.println("°");
}

void printHelp() {
  Serial.println("\nBeetleBot Control System");
  Serial.println("Leg Selection Commands:");
  Serial.println("rf, rm, rb, lf, lm, lb");
  Serial.println("\nMovement Commands (after selecting leg):");
  Serial.println("Q/A - Coxa +/-");
  Serial.println("W/S - Femur +/-");
  Serial.println("E/D - Tibia +/-");
  Serial.println("\nAdvanced Commands:");
  Serial.println("<leg> sweep - Full range motion");
  Serial.println("wave - Middle legs waving animation");
  Serial.println("help - Show this message");
}