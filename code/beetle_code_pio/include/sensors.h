// The initial code has been taken from the Pololu example that uses multiple sensors.
// The original file is available at https://github.com/pololu/vl53l1x-arduino/blob/master/examples/ContinuousMultipleSensors/ContinuousMultipleSensors.ino

#include "Wire.h"
#include "VL53L1X.h"

// try to initiate the sensors until all are good
void forceSetup();

// Try to setup the sensors, return true iff all are good
bool trySetup();

// Check once if sensor i has a new value in mm (non blocking)
// -1 <=> Any error status from .ranging_data.range_status
// -2 <=> New data wasn't available
int checkSensor(int i);

// Get a new value from sensor i in mm (blocking call)
int getFromSensor(int i);

// Simple getter
VL53L1X getSensor(int i);