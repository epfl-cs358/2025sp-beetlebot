#include <Adafruit_PWMServoDriver.h>
#include <beetle_hdr.h>
Adafruit_PWMServoDriver board1 = Adafruit_PWMServoDriver(0x40);

#define SERVOMIN  125
#define SERVOMAX  625
#define SWEEP_DELAY 20  // ms between degree movements
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

// Update leg initialization
Leg legs[6] = {
  {"rf", cRFCoxaPin, cRFFemurPin, cRFTibiaPinESP, COXAS_SIT_ANGLE, FEMURS_SIT_ANGLE, TIBIAS_SIT_ANGLE},
  {"rm", cRMCoxaPin, cRMFemurPin, cRMTibiaPin, COXAS_SIT_ANGLE, FEMURS_SIT_ANGLE, TIBIAS_SIT_ANGLE},
  {"rb", cRBCoxaPin, cRBFemurPin, cRBTibiaPin, COXAS_SIT_ANGLE, FEMURS_SIT_ANGLE, TIBIAS_SIT_ANGLE},
  {"lf", cLFCoxaPin, cLFFemurPin, cLFTibiaPinESP, COXAS_SIT_ANGLE, FEMURS_SIT_ANGLE, TIBIAS_SIT_ANGLE},
  {"lm", cLMCoxaPin, cLMFemurPin, cLMTibiaPin, COXAS_SIT_ANGLE, FEMURS_SIT_ANGLE, TIBIAS_SIT_ANGLE},
  {"lb", cLBCoxaPin, cLBFemurPin, cLBTibiaPin, COXAS_SIT_ANGLE, FEMURS_SIT_ANGLE, TIBIAS_SIT_ANGLE}
};

Leg* currentLeg = &legs[3]; // Default to right middle (rm)

// Function prototypes
void initializeAllServos();
void performWave();
void performSweep(int servoIndex, int &currentAngle);
void printHelp();
void coordinatedSweep(uint8_t coxaPin, int coxaStart, int coxaEnd,
                    uint8_t tibiaPin, int tibiaStart, int tibiaEnd,
                    Leg &leg);
void setServoAngle(uint8_t pin, int angle);
void handleSerialInput();
void handleKeyCommand(char key);
void updateServo(uint8_t servoIndex, int step, int &angle);
void updateServoDirect(int servoIndex, int newAngle, int &storage);
void handleTextCommand(String input);
void smoothMove(int servoIndex, int targetAngle, int &currentAngle, int stepSize);
int angleToPulse(int ang);
void handleSweepCommand(String input);
void printCurrentLegAngles();
void moveToStandPosition();
void moveToSitPosition();
void moveToStandPosition();
void setServoAngle(uint8_t pin, int angle);

void setup() {
  Serial.begin(9600);
  
  // Initialize PCA9685
  board1.begin();
  board1.setPWMFreq(PWM_FREQ);

  // Initialize ESP32 PWM channels for direct-connected servos
  ledcSetup(0, 50, 16); // Channel 0, 50Hz, 16-bit resolution
  ledcSetup(1, 50, 16); // Channel 1, 50Hz, 16-bit resolution
  ledcAttachPin(cLFTibiaPinESP, 0); // Left Front Tibia
  ledcAttachPin(cRFTibiaPinESP, 1); // Right Front Tibia
  
  initializeAllServos();
  printHelp();
}

void loop() {
  handleSerialInput();
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

void handleTextCommand(String input) {
  input.toLowerCase();
  
  // Check for leg selection first
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
  else if(input == "stand") {
    moveToStandPosition();
  }
  else if(input == "sit") {
    moveToSitPosition();
  }
  else if(input.endsWith("sweep")) {
    handleSweepCommand(input);
  }
}

void handleSweepCommand(String input) {
  input.toLowerCase();
  
  if (input == "coxa sweep") {
    performSweep(currentLeg->coxaPin, currentLeg->coxaAngle);
  }
  else if (input == "femur sweep") {
    performSweep(currentLeg->femurPin, currentLeg->femurAngle);
  }
  else if (input == "tibia sweep") {
    performSweep(currentLeg->tibiaPin, currentLeg->tibiaAngle);
  }
  else {
    Serial.println("Unknown sweep command. Use 'coxa sweep', 'femur sweep', or 'tibia sweep'.");
  }
}

void moveToStandPosition() {
  for(uint8_t i = 0; i < 6; i++) {
    smoothMove(legs[i].coxaPin, COXAS_STAND_ANGLE, legs[i].coxaAngle, 30);
    smoothMove(legs[i].femurPin, FEMURS_STAND_ANGLE, legs[i].femurAngle, 30);
    smoothMove(legs[i].tibiaPin, TIBIAS_STAND_ANGLE, legs[i].tibiaAngle, 30);
  }
}

void moveToSitPosition() {
  for(uint8_t i = 0; i < 6; i++) {
    smoothMove(legs[i].coxaPin, COXAS_SIT_ANGLE, legs[i].coxaAngle, 30);
    smoothMove(legs[i].femurPin, FEMURS_SIT_ANGLE, legs[i].femurAngle, 30);
    smoothMove(legs[i].tibiaPin, TIBIAS_SIT_ANGLE, legs[i].tibiaAngle, 30);
  }
}


void performWave() {
  // Save original positions of the middle legs
  Leg* waveLegs[] = {&legs[1], &legs[4]}; // RM and LM legs
  int originalAngles[2][3]; // [rm, lm][coxa, femur, tibia]

  // Store original positions
  for(uint8_t i = 0; i < 2; i++) {
    originalAngles[i][0] = waveLegs[i]->coxaAngle;
    originalAngles[i][1] = waveLegs[i]->femurAngle;
    originalAngles[i][2] = waveLegs[i]->tibiaAngle;
  }

  // Move to wave starting position
  for(auto leg : waveLegs) {
    smoothMove(leg->femurPin, 100, leg->femurAngle, 30);
  }
  
  // Wave 3 times
  for(uint8_t i = 0; i < 3; i++) {
    for(auto leg : waveLegs) {
      coordinatedSweep(leg->coxaPin, 130, 50, leg->tibiaPin, 45, 25, *leg);
    }
    for(auto leg : waveLegs) {
      coordinatedSweep(leg->coxaPin, 50, 130, leg->tibiaPin, 25, 45, *leg);
    }
  }

  // Return to original positions
  for(uint8_t i = 0; i < 2; i++) {
    smoothMove(waveLegs[i]->coxaPin, originalAngles[i][0], waveLegs[i]->coxaAngle, 30);
    smoothMove(waveLegs[i]->femurPin, originalAngles[i][1], waveLegs[i]->femurAngle, 30);
    smoothMove(waveLegs[i]->tibiaPin, originalAngles[i][2], waveLegs[i]->tibiaAngle, 30);
  }
}

void coordinatedSweep(uint8_t coxaPin, int coxaStart, int coxaEnd,
                      uint8_t tibiaPin, int tibiaStart, int tibiaEnd,
                      Leg &leg) {
  const uint8_t steps = 20;
  for(uint8_t i = 0; i <= steps; i++) {
  float ratio = (float)i / steps;

  int coxa = coxaStart + (coxaEnd - coxaStart) * ratio;
  int tibia = tibiaStart + (tibiaEnd - tibiaStart) * ratio;

  setServoAngle(coxaPin, coxa);
  setServoAngle(tibiaPin, tibia);
  leg.coxaAngle = coxa;
  leg.tibiaAngle = tibia;

  delay(SWEEP_DELAY);
  }
}

void smoothMove(int servoIndex, int targetAngle, int &currentAngle, int stepSize) {
  while(abs(currentAngle - targetAngle) > stepSize) {
    currentAngle += (targetAngle > currentAngle) ? stepSize : -stepSize;
    board1.setPWM(servoIndex, 0, angleToPulse(currentAngle));
    delay(SWEEP_DELAY);
  }
  currentAngle = targetAngle;  // Ensure final position is exact
  board1.setPWM(servoIndex, 0, angleToPulse(currentAngle));
}


void performSweep(int servoIndex, int &currentAngle) {
  int originalAngle = currentAngle;
  
  // Sweep down to 0°
  for (int angle = originalAngle; angle >= 0; angle--) {
    updateServoDirect(servoIndex, angle, currentAngle);
    delay(SWEEP_DELAY);
  }
  
  // Sweep up to 180°
  for (int angle = 0; angle <= 180; angle++) {
    updateServoDirect(servoIndex, angle, currentAngle);
    delay(SWEEP_DELAY);
  }
  
  // Return to original position
  for (int angle = 180; angle >= originalAngle; angle--) {
    updateServoDirect(servoIndex, angle, currentAngle);
    delay(SWEEP_DELAY);
  }
  
  printCurrentLegAngles();
}

void setServoAngle(uint8_t pin, int angle) {
  if(pin == cLFTibiaPinESP || pin == cRFTibiaPinESP) { // ESP32 direct pins
    // Convert angle to ESP32 LEDC compatible pulse width
    uint32_t pulse = map(angle, 0, 180, 500, 2500); // Convert angle to microseconds
    ledcWrite((pin == cLFTibiaPinESP) ? 0 : 1, pulse * 65536 / 20000); // Convert to duty cycle
  }
  else if(pin < 16) { // PCA9685 pins
    board1.setPWM(pin, 0, angleToPulse(angle));
  }
}

void updateServo(uint8_t pin, int step, int &angle) {
  int newAngle = constrain(angle + step, 0, 180);
  if(newAngle != angle) {
    angle = newAngle;
    setServoAngle(pin, angle);
    printCurrentLegAngles(); // Changed from printCurrentLeg()
  }
}

void updateServoDirect(int servoIndex, int newAngle, int &storage) {
  storage = newAngle;
  board1.setPWM(servoIndex, 0, angleToPulse(newAngle));
}

void initializeAllServos() {
  for(uint8_t i = 0; i < 6; i++) {
    setServoAngle(legs[i].coxaPin, COXAS_SIT_ANGLE);
    setServoAngle(legs[i].femurPin, FEMURS_SIT_ANGLE);
    setServoAngle(legs[i].tibiaPin, TIBIAS_SIT_ANGLE);
    
    legs[i].coxaAngle = COXAS_SIT_ANGLE;
    legs[i].femurAngle = FEMURS_SIT_ANGLE;
    legs[i].tibiaAngle = TIBIAS_SIT_ANGLE;
  }
  delay(500);
}

void printCurrentLegAngles() {
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
  Serial.println("\nServo Control System");
  Serial.println("Quick commands (single key):");
  Serial.println("Q/A - Coxa +/-");
  Serial.println("W/S - Femur +/-");
  Serial.println("E/D - Tibia +/-");
  Serial.println("\nAdvanced commands:");
  Serial.println("<joint> sweep - Full range motion");
  Serial.println("  (coxa|femur|tibia) sweep");
  Serial.println("wave - Beetlebot waves at you animation");
}

int angleToPulse(int ang) {
  return map(ang, 0, 180, SERVOMIN, SERVOMAX);
}