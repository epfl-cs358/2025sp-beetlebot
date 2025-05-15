
#ifndef data_H
#define data_H

#include <Adafruit_PWMServoDriver.h>

/*
    Defines all the necessary constants, measurements

*/
#define MYPI 3.14159265

#define SWEEP_DELAY 20
#define NO_INTERSECTION -1000

enum legIndex {
    LF = 0, RF = 1, LM = 2, RM = 3, LB = 4, RB = 5
};

const int sitAngle [3] = {90, 90, 90};
const int standAngle [3] = {90, 0, 35};
const int curlAngle [3] = {90, 180, 35};

//goes from left to right, front to back
const int servoPins [6][3] = {
    {8, 9, 18}, {6, 7, 23},
    {10, 11, 12}, {3, 4, 5},
    {13, 14, 15}, {2, 1, 0}
};

const float homePositions [3] = 
    {0.0, 79.36, 41.2};

//origin of the coordinates system in beetlebot is the center of the robot, and
//the origin of the leg's system is the coxa joint, hence this array maps the coordinates to the global (robot center) system
const float legCoords[6][3] = { 
    { 45.748,  -74.326, 5* MYPI / 6 },   //lf
    { 45.748, 74.326,  MYPI / 6 },   //rf
    {     -20.168,  -82.171, MYPI }, //lm
    {     -20.168, 82.171,        0},   
    { -74.254,  -45.768,  7*MYPI / 6 },  //lb 
    { -74.254, 45.768, - MYPI / 6 } }; //rb


const int lfCOffset = 0;
const int lfFOffset = 0;
const int lfTOffset = 0;

const int rfCOffset = 0;
const int rfFOffset = 0;
const int rfTOffset = 0;

const int lmCOffset = 0;
const int lmFOffset = 0;
const int lmTOffset = 0;

const int rmCOffset = 0;
const int rmFOffset = 0;
const int rmTOffset = 0;

const int lbCOffset = 0;
const int lbFOffset = 0;
const int lbTOffset = 0;

const int rbCOffset = 0;
const int rbFOffset = 0;
const int rbTOffset = 0;


const int periodMs = 10;

#endif 