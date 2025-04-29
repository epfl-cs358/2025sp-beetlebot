//Contains all main includes and defines
#include <cstdint>
#include <WString.h>
#include <Adafruit_PWMServoDriver.h>
#include <beetle_hdr.h>

#define SERVOMIN  125
#define SERVOMAX  625
#define SWEEP_DELAY 20  // ms between degree movements
#define PWM_FREQ 60

#define STEP_DURATION_MS 500  // Time per phase
#define MAX_STEP_DISTANCE 50  // Max forward reach
#define MAX_LIFT_HEIGHT 40    // Max foot clearance

#define STEP_DISTANCE 40.0
#define LIFT_HEIGHT 30.0
#define GAIT_CYCLE_MS 1000

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

extern float current_X[6];
extern float current_Y[6];
extern float current_Z[6];

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
void smoothMove(uint8_t pin, int targetAngle, int &currentAngle, int stepSize);
int angleToPulse(int ang);
void handleSweepCommand(String input);
void printCurrentLegAngles();
void moveToStandPosition();
void moveToSitPosition();
void moveToCurlPosition();
void setServoAngle(uint8_t pin, int angle);
void tripod_gait();
void moveTripod(int legIndices[], int femurDelta, int tibiaDelta);
  
  
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
Serial.println("curl - Beetlebot curls up for easier storage");
Serial.println("sit - Beetlebot sits down");
Serial.println("stand - Beetlebot stands up");
}