
#ifndef data_H
#define data_H

#include <Adafruit_PWMServoDriver.h>

/*
    Defines all the necessary constants, measurements

*/

#define SWEEP_DELAY 20

enum legIndex {
    LF = 0, RF = 1, LM = 2, RM = 3, LB = 4, RB = 5
};

const int sitAngle [3] = {90, 150, 145};
const int setAngle [3] = {90, 150, 90};
const int standAngle [3] = {90, 50, 145};

//goes from left to right, front to back
const int servoPins [6][3] = {
    {8, 9, 18}, {6, 7, 23},
    {10, 11, 12}, {3, 4, 5},
    {13, 14, 15}, {2, 1, 0}
};

const int lfCOffset = 0;
const int lfFOffset = 0;
const int lfTOffset = 0;

const int rfCOffset = 0;
const int rfFOffset = -5;
const int rfTOffset = 0;

const int lmCOffset = 0;
const int lmFOffset = -5;
const int lmTOffset = -5;

const int rmCOffset = 0;
const int rmFOffset = 0;
const int rmTOffset = 0;

const int lbCOffset = 0;
const int lbFOffset = -5;
const int lbTOffset = 0;

const int rbCOffset = 0;
const int rbFOffset = 0;
const int rbTOffset = 0;


const int periodMs = 10;

#endif 