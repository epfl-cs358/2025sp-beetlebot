#include <Adafruit_PWMServoDriver.h>
Adafruit_PWMServoDriver board1 = Adafruit_PWMServoDriver(0x40);

#define SERVOMIN  125
#define SERVOMAX  625
#define SWEEP_DELAY 20  // ms between degree movements

// Customizable starting angles
#define COXA_START_ANGLE  90
#define FEMUR_START_ANGLE 35 
#define TIBIA_START_ANGLE 40

int coxaAngle = COXA_START_ANGLE;
int femurAngle = FEMUR_START_ANGLE;
int tibiaAngle = TIBIA_START_ANGLE;

void performWave();

void setup() {
  Serial.begin(9600);
  board1.begin();
  board1.setPWMFreq(60);
  
  initializeServos();
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
    case 'q': updateServo(0, 5, coxaAngle); break;
    case 'a': updateServo(0, -5, coxaAngle); break;
    case 'w': updateServo(1, 5, femurAngle); break;
    case 's': updateServo(1, -5, femurAngle); break;
    case 'e': updateServo(2, 5, tibiaAngle); break;
    case 'd': updateServo(2, -5, tibiaAngle); break;
  }
}

void handleTextCommand(String input) {
  input.toLowerCase();
  
  if (input == "wave") {
    performWave();
    return;
  }

  int spaceIndex = input.indexOf(' ');
  
  if (spaceIndex != -1) {
    String joint = input.substring(0, spaceIndex);
    String command = input.substring(spaceIndex + 1);
    
    if (command == "sweep") {
      if (joint == "coxa") performSweep(0, coxaAngle);
      else if (joint == "femur") performSweep(1, femurAngle);
      else if (joint == "tibia") performSweep(2, tibiaAngle);
      else Serial.println("Invalid joint");
    }
  }
}

void performWave() {
  // Save original positions
  int originalCoxa = coxaAngle;
  int originalFemur = femurAngle;
  int originalTibia = tibiaAngle;

  // Move to wave starting position
  smoothMove(1, 100, femurAngle, 30);  // Move femur to 100°
  
  // Wave 3 times
  for(int i = 0; i < 3; i++) {
    // Coxa sweep from 130 to 50 while tibia moves from 45 to 25
    coordinatedSweep(130, 50, 45, 25);
    // Coxa sweep back from 50 to 130 while tibia moves back
    coordinatedSweep(50, 130, 25, 45);
  }

  // Return to original positions
  smoothMove(0, originalCoxa, coxaAngle, 30);
  smoothMove(1, originalFemur, femurAngle, 30);
  smoothMove(2, originalTibia, tibiaAngle, 30);
}

void coordinatedSweep(int coxaStart, int coxaEnd, int tibiaStart, int tibiaEnd) {
  int steps = 20;  // Number of steps for the movement
  for(int i = 0; i <= steps; i++) {
    float ratio = (float)i / steps;
    int currentCoxa = coxaStart + (coxaEnd - coxaStart) * ratio;
    int currentTibia = tibiaStart + (tibiaEnd - tibiaStart) * ratio;
    
    board1.setPWM(0, 0, angleToPulse(currentCoxa));
    board1.setPWM(2, 0, angleToPulse(currentTibia));
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
  
  printAngles();
}

void updateServo(int servoIndex, int step, int &angle) {
  int newAngle = constrain(angle + step, 0, 180);
  if (newAngle != angle) {
    angle = newAngle;
    board1.setPWM(servoIndex, 0, angleToPulse(angle));
    printAngles();
  }
}

void updateServoDirect(int servoIndex, int newAngle, int &storage) {
  storage = newAngle;
  board1.setPWM(servoIndex, 0, angleToPulse(newAngle));
}

void initializeServos() {
  coxaAngle = COXA_START_ANGLE;
  femurAngle = FEMUR_START_ANGLE;
  tibiaAngle = TIBIA_START_ANGLE;
  
  board1.setPWM(0, 0, angleToPulse(coxaAngle));
  board1.setPWM(1, 0, angleToPulse(femurAngle));
  board1.setPWM(2, 0, angleToPulse(tibiaAngle));
  delay(500);
}

void printAngles() {
  Serial.print("Coxa:");
  Serial.print(coxaAngle);
  Serial.print("° Femur:");
  Serial.print(femurAngle);
  Serial.print("° Tibia:");
  Serial.print(tibiaAngle);
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