// The initial code has been taken from the Pololu example that uses multiple sensors.
// The original file is available at https://github.com/pololu/vl53l1x-arduino/blob/master/examples/ContinuousMultipleSensors/ContinuousMultipleSensors.ino

#include "sensors.h"

const uint8_t sensorCount = 3;

// The Arduino pin connected to the XSHUT pin of each sensor.
const uint8_t xshutPins[sensorCount] = { 25, 26, 27 };

VL53L1X sensors[sensorCount];


// try to initiate the sensors until all are good
void forceSetup() {
  while (!trySetup()) {};
}

// Try to setup the sensors, return true iff all are good
bool trySetup() {
  // Disable/reset all sensors by driving their XSHUT pins low.
  for (uint8_t i = 0; i < sensorCount; i++) {
    pinMode(xshutPins[i], OUTPUT);
    digitalWrite(xshutPins[i], LOW);
  }

  // Enable, initialize, and start each sensor, one by one.
  for (uint8_t i = 0; i < sensorCount; i++) {
    // Stop driving this sensor's XSHUT low. This should allow the carrier
    // board to pull it high. (We do NOT want to drive XSHUT high since it is
    // not level shifted.) Then wait a bit for the sensor to start up.
    pinMode(xshutPins[i], INPUT);
    delay(10);

    sensors[i].setTimeout(500);
    if (!sensors[i].init()) return false;

    // Each sensor must have its address changed to a unique value other than
    // the default of 0x29 (except for the last one, which could be left at
    // the default). To make it simple, we'll just count up from 0x2A.
    sensors[i].setAddress(0x2A + i);
    // We decided on Short distance mode since its the least variable and should still be big enough (1.3m no matter the conditions)
    sensors[i].setDistanceMode(VL53L1X::Short);
    sensors[i].setMeasurementTimingBudget(20000); // Minimum for short distance mode (in microseconds)
    sensors[i].startContinuous(20); // Minimum since it must be >= to timing budget (in ms)

  }
    return true;
}

// Check once if sensor i has a new value in mm (non blocking)
// -1 <=> Any error status from .ranging_data.range_status
// -2 <=> New data wasn't available
int checkSensor(int i) {
  //read(false) is non blocking but reading with no data ready has undefined behavior
  if (sensors[i].dataReady()) {
      sensors[i].read(false);
      if (sensors[i].ranging_data.range_status == VL53L1X::RangeValid) {
        return sensors[i].ranging_data.range_mm;
      } else {
        return -1;
      }
    } else {
      return -2;
    }
}

// Get a new value from sensor i in mm (blocking call)
int getFromSensor(int i) {
  int val;
  do {
    val = checkSensor(i);
  } while(val < 0);

  return val;
}

// Simple getter
VL53L1X getSensor(int i) {
  return sensors[i];
}