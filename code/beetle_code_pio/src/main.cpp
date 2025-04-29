#include <main.h>
Adafruit_PWMServoDriver board1 = Adafruit_PWMServoDriver(0x40);

// Update leg initialization
Leg legs[6] = {
  {"rf", cRFCoxaPin, cRFFemurPin, cRFTibiaPinESP, COXAS_SIT_ANGLE, FEMURS_SIT_ANGLE, TIBIAS_SIT_ANGLE},
  {"rm", cRMCoxaPin, cRMFemurPin, cRMTibiaPin, COXAS_SIT_ANGLE, FEMURS_SIT_ANGLE, TIBIAS_SIT_ANGLE},
  {"rb", cRBCoxaPin, cRBFemurPin, cRBTibiaPin, COXAS_SIT_ANGLE, FEMURS_SIT_ANGLE, TIBIAS_SIT_ANGLE},
  {"lf", cLFCoxaPin, cLFFemurPin, cLFTibiaPinESP, COXAS_SIT_ANGLE, FEMURS_SIT_ANGLE, TIBIAS_SIT_ANGLE},
  {"lm", cLMCoxaPin, cLMFemurPin, cLMTibiaPin, COXAS_SIT_ANGLE, FEMURS_SIT_ANGLE, TIBIAS_SIT_ANGLE},
  {"lb", cLBCoxaPin, cLBFemurPin, cLBTibiaPin, COXAS_SIT_ANGLE, FEMURS_SIT_ANGLE, TIBIAS_SIT_ANGLE}
};

// Home positions (modify from beetle_hdr.h if needed) RF,RM,RB,LB,LM,LF
const float HOME_X[6] = {155.0, 0.0, -155.0, -155.0, 0.0, 155.0};
const float HOME_Y[6] = {82.0, 116.0, 82.0, -82.0, -116.0, -82.0};
const float HOME_Z[6] = {-80.0, -80.0, -80.0, -80.0, -80.0, -80.0};

Leg* currentLeg = &legs[3]; // Default to right middle (rm)
bool walkMode = false;
int walkDirection = 0; // 0:stop, 1:forward, 2:back, 3:left, 4:right
unsigned long prevGaitTime = 0;
int gaitStep = 0;

int tripodStepPhase = 0;  // 0-3 phase counter

float current_X[6] = {0};
float current_Y[6] = {0};
float current_Z[6] = {0};

float L0, L3;

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
  if (walkMode) {
    if (walkDirection != 0) tripod_gait();
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

void handleKeyCommand(char key) {
  if (walkMode) {
    switch(key) {
      case 'w': tripod_gait(); break;
      case 's': walkDirection = 2; break;
      case 'a': walkDirection = 3; break;
      case 'd': walkDirection = 4; break;
      default: walkDirection = 0; break;
    }
  } else {
  switch(key) {
    case 'q': updateServo(currentLeg->coxaPin, 5, currentLeg->coxaAngle); break;
    case 'a': updateServo(currentLeg->coxaPin, -5, currentLeg->coxaAngle); break;
    case 'w': updateServo(currentLeg->femurPin, 5, currentLeg->femurAngle); break;
    case 's': updateServo(currentLeg->femurPin, -5, currentLeg->femurAngle); break;
    case 'e': updateServo(currentLeg->tibiaPin, 5, currentLeg->tibiaAngle); break;
    case 'd': updateServo(currentLeg->tibiaPin, -5, currentLeg->tibiaAngle); break;
  }
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
  } else if (input == "walk") {
    walkMode = true;
    moveToStandPosition();
    Serial.println("Walk mode: Press W to step forward");
  } else if (input == "quit") {
    walkMode = false;
    moveToStandPosition();
    Serial.println("Exited Walk Mode");
  } else if(input == "help") {
    printHelp();
  }
  else if(input == "stand") {
    moveToStandPosition();
  }
  else if(input == "sit") {
    moveToSitPosition();
  } else if (input == "curl") {
    moveToCurlPosition();
  }
  else if(input.endsWith("sweep")) {
    handleSweepCommand(input);
  } 
  else {
    Serial.println("Unknown command. Type 'help' for a list of commands.");
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

void moveToCurlPosition(){
  for(uint8_t i = 0; i < 6; i++) {
    smoothMove(legs[i].coxaPin, COXAS_CURL_ANGLE, legs[i].coxaAngle, 30);
    smoothMove(legs[i].femurPin, FEMURS_CURL_ANGLE, legs[i].femurAngle, 30);
    smoothMove(legs[i].tibiaPin, TIBIAS_CURL_ANGLE, legs[i].tibiaAngle, 30);
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

void smoothMove(uint8_t pin, int targetAngle, int &currentAngle, int stepSize) {
  while(abs(currentAngle - targetAngle) > 0) {
    int step = constrain(targetAngle - currentAngle, -stepSize, stepSize);
    currentAngle += step;
    setServoAngle(pin, currentAngle);
    delay(SWEEP_DELAY);
  }
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
    // Add calibration offset if needed
  angle = constrain(angle + 5, 0, 180); // Example +5° offset
  uint32_t pulse = map(angle, 0, 180, 500, 2500);
  ledcWrite((pin == cLFTibiaPinESP) ? 0 : 1, pulse * 65536 / 20000);
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
  delay(300); // Allow servos time to settle
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

int angleToPulse(int ang) {
  return map(ang, 0, 180, SERVOMIN, SERVOMAX);
}

void tripod_gait() {
  const int tripod1[] = {0, 2, 4}; // RF, RB, LM
  const int tripod2[] = {1, 3, 5}; // RM, LF, LB
  static int phase = 0;
  static unsigned long lastStepTime = 0;

  if (millis() - lastStepTime < GAIT_CYCLE_MS / 4) return;
  lastStepTime = millis();

  float stepX = STEP_DISTANCE;
  float liftZ = LIFT_HEIGHT;

  switch (phase) {
      case 0: // Lift tripod1
          for (int i : tripod1) {
              current_Z[i] = HOME_Z[i] + liftZ;
          }
          phase = 1;
          break;
      case 1: // Move tripod1 forward
          for (int i : tripod1) {
              current_X[i] = HOME_X[i] + stepX;
          }
          phase = 2;
          break;
      case 2: // Lower tripod1
          for (int i : tripod1) {
              current_Z[i] = HOME_Z[i];
          }
          phase = 3;
          break;
      case 3: // Move tripod2 back
          for (int i : tripod2) {
              current_X[i] = HOME_X[i] - stepX;
          }
          phase = 0;
          break;
  }

  // Update all legs via IK
  for (int i = 0; i < 6; i++) {
      leg_IK(i, current_X[i], current_Y[i], current_Z[i]);
  }
}

void moveTripod(int legIndices[], int femurDelta, int tibiaDelta) {
  for (int i = 0; i < 3; i++) {
    int legIdx = legIndices[i];
    smoothMove(legs[legIdx].femurPin, legs[legIdx].femurAngle + femurDelta, legs[legIdx].femurAngle, 10);
    smoothMove(legs[legIdx].tibiaPin, legs[legIdx].tibiaAngle + tibiaDelta, legs[legIdx].tibiaAngle, 10);
  }
}

void leg_IK(int leg_num, float X, float Y, float Z) {
  // Using dimensions from beetle_hdr.h
  const float COXA_LENGTH = cCoxaLength;
  const float FEMUR_LENGTH = cFemurLength;
  const float TIBIA_LENGTH = cTibiaLength;

  // Calculate angles
  float L0 = sqrt(sq(X) + sq(Y)) - COXA_LENGTH;
  float L3 = sqrt(sq(L0) + sq(Z));
  
  if (L3 >= (TIBIA_LENGTH + FEMUR_LENGTH)) return; // Skip if unreachable

  // Tibia angle
  float phi_tibia = acos((sq(FEMUR_LENGTH) + sq(TIBIA_LENGTH) - sq(L3)) / (2 * FEMUR_LENGTH * TIBIA_LENGTH));
  float theta_tibia = degrees(phi_tibia) - 23.0; // Adjust with calibration if needed

  // Femur angle
  float gamma_femur = atan2(Z, L0);
  float phi_femur = acos((sq(FEMUR_LENGTH) + sq(L3) - sq(TIBIA_LENGTH)) / (2 * FEMUR_LENGTH * L3));
  float theta_femur = degrees(phi_femur + gamma_femur) + 14.0 + 90.0;

  // Coxa angle
  float theta_coxa = degrees(atan2(X, Y));

  // Apply calibration offsets (define in beetle_hdr.h if needed)
  theta_coxa += 0;   // Add calibration offset
  theta_femur += 0;
  theta_tibia += 0;

  // Constrain angles to servo limits (0-180)
  theta_coxa = constrain(theta_coxa, 0, 180);
  theta_femur = constrain(theta_femur, 0, 180);
  theta_tibia = constrain(theta_tibia, 0, 180);

  // Update servo positions (using your servo driver)
  Leg* leg = &legs[leg_num];
  setServoAngle(leg->coxaPin, theta_coxa);
  setServoAngle(leg->femurPin, theta_femur);
  setServoAngle(leg->tibiaPin, theta_tibia);

  // Store current angles
  leg->coxaAngle = theta_coxa;
  leg->femurAngle = theta_femur;
  leg->tibiaAngle = theta_tibia;
}