#pragma once
#include "hexapod.h"
//#include "wifi_credentials.h"
#include <Arduino.h>

const bool WEB_SERIAL = false;  // true = WebSerial, false = plain Serial

enum LegIdx   { RF, RM, RB, LF, LM, LB };   // 0-5
enum JointIdx { Coxa, Femur, Tibia };        // 0-2

/*        leg ─┐  joint ─┐ */
const int servoPins[6][3] = {
/* RF */ {  6,  7, 23 },
/* RM */ {  3,  4,  5 },
/* RB */ {  2,  1,  0 },
/* LF */ {  8,  9, 18 },
/* LM */ { 10, 11, 12 },
/* LB */ { 13, 14, 15 }
};

void handleSerialInput();
void handleTextCommand(String input);
