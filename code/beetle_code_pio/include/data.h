
#ifndef data_H
#define data_H

#include <Adafruit_PWMServoDriver.h>

/*
    Defines all the necessary constants, measurements

*/
#define MYPI 3.14159265

#define SWEEP_DELAY 20


const int sitAngle [3] = {90, 90, 90};
const int standAngle [3] = {90, 0, 35};
const int curlAngle [3] = {90, 180, 35};

//goes from left to right, front to back
const int servoPins [6][3] = {
    {8, 9, 18}, {6, 7, 23},
    {10, 11, 12}, {3, 4, 5},
    {13, 14, 15}, {2, 1, 0}
};

//in mm
const float bodyCoxaOffset [6][2] = {
    {79.26, -49.26}, {79.26, 49.26},
    {0, -69}, {0, 69},
    {-79.26, -49.26}, {-79.26, 49.26}
};


const float homePositions [6][3] = {
    {155.0, 82.0, -80.0}, {0.0, 116.0, -80.0},
    {-155.0, 82.0, -80.0}, {-155.0, -82.0, -80.0},
    {0.0, -116.0, -80.0}, {155.0, -82.0, -80.0}
};

//origin of the coordinates system in beetlebot is the center of the robot, and
//the origin of the leg's system is the coxa joint, hence this array maps the coordinates to the global (robot center) system
const float legCoords[6][3] = { 
    {  71.3,  43.0,     MYPI / 6 },   
    {  71.3, -43.0, 5 * MYPI / 6 },  
    {     0,  53.0,          0 },   
    {     0, -53.0,         MYPI },   
    { -71.3,  43.0,    -MYPI / 6 },   
    { -71.3, -43.0, 7 * MYPI / 6 } };

const int offsets [3] = {0,0,0};

const int periodMs = 10;

#endif 