// All Hardware configurations, body dimensions and other constants

/* BeetleBot Servo and Leg configurations:

                Claw
            LF        RF
            LM        RM
            LB        RB

Body - Coxa - Femur - Tibia (end of feet)

 */

//--------------------------------------------------------------------

//[BODY DIMENSIONS]
#define cCoxaLength     35
#define cFemurLength    80
#define cTibiaLength    120

// Default Coxa setup angle in tenths of a degree (decimals = 1)
#define cRRCoxaAngle1   -600   // Right Rear
#define cRMCoxaAngle1    0     // Right Middle
#define cRFCoxaAngle1    600   // Right Front
#define cLMCoxaAngle1    0       //Default Coxa setup angle in tenths of a degree
#define cLMCoxaAngle1    0     // Left Middle
#define cLFCoxaAngle1    600   // Left Front decimals = 1

#define cRROffsetX      -69     //Distance X from center of the body to the Right Rear coxa
#define cRROffsetZ      119     //Distance Z from center of the body to the Right Rear coxa
#define cRMOffsetX      -138    //Distance X from center of the body to the Right Middle coxa
#define cRMOffsetZ      0       //Distance Z from center of the body to the Right Middle coxa
#define cRFOffsetX      -69     //Distance X from center of the body to the Right Front coxa
#define cRFOffsetZ      -119    //Distance Z from center of the body to the Right Front coxa

#define cLROffsetX      69      //Distance X from center of the body to the Left Rear coxa
#define cLROffsetZ      119     //Distance Z from center of the body to the Left Rear coxa
#define cLMOffsetX      138     //Distance X from center of the body to the Left Middle coxa
#define cLMOffsetZ      0       //Distance Z from center of the body to the Left Middle coxa
#define cLFOffsetX      69      //Distance X from center of the body to the Left Front coxa
#define cLFOffsetZ      -119    //Distance Z from center of the body to the Left Front coxa
//--------------------------------------------------------------------

//[PIN NUMBERS]
#define cLFCoxaPin      0   //Front Left leg Hip Horizontal
#define cLFFemurPin     1   //Front Left leg Hip Vertical
#define cLFTibiaPinESP  23   //Front Left leg Knee (plugged into the esp32)

#define cRFCoxaPin      15   //Front Right leg Hip Horizontal
#define cRFFemurPin     14   //Front Right leg Hip Vertical
#define cRFTibiaPinESP  18   //Front Right leg Knee (plugged into the esp32)

#define cLMCoxaPin      6   //Middle Left leg Hip Horizontal
#define cLMFemurPin     5   //Middle Left leg Hip Vertical
#define cLMTibiaPin     3   //Middle Left leg Knee

#define cRMCoxaPin      4   //Middle Right leg Hip Horizontal
#define cRMFemurPin     12   //Middle Right leg Hip Vertical
#define cRMTibiaPin     13   //Middle Right leg Knee

#define cLBCoxaPin      7   //Rear Left leg Hip Horizontal
#define cLBFemurPin     8   //Rear Left leg Hip Vertical
#define cLBTibiaPin     2   //Rear Left leg Knee

#define cRBCoxaPin      9   //Rear Right leg Hip Horizontal
#define cRBFemurPin     10   //Rear Right leg Hip Vertical
#define cRBTibiaPin     11   //Rear Right leg Knee

//#define cClawPin       18   //Claw Servo // havent implemented this yet
//--------------------------------------------------------------------

//[MIN/MAX ANGLES]
#define cRRCoxaMin1     -650      //Mechanical limits of the Right Rear Leg
#define cRRCoxaMax1     650
#define cRRFemurMin1    -900
#define cRRFemurMax1    550
#define cRRTibiaMin1    -400
#define cRRTibiaMax1    750

#define cRMCoxaMin1     -650      //Mechanical limits of the Right Middle Leg
#define cRMCoxaMax1     650
#define cRMFemurMin1    -900
#define cRMFemurMax1    550
#define cRMTibiaMin1    -400
#define cRMTibiaMax1    750

#define cRFCoxaMin1     -650      //Mechanical limits of the Right Front Leg
#define cRFCoxaMax1     650
#define cRFFemurMin1    -900
#define cRFFemurMax1    550
#define cRFTibiaMin1    -400
#define cRFTibiaMax1    750

#define cLRCoxaMin1     -650      //Mechanical limits of the Left Rear Leg
#define cLRCoxaMax1     650
#define cLRFemurMin1    -900
#define cLRFemurMax1    550
#define cLRTibiaMin1    -400
#define cLRTibiaMax1    750

#define cLMCoxaMin1     -650      //Mechanical limits of the Left Middle Leg
#define cLMCoxaMax1     650
#define cLMFemurMin1    -900
#define cLMFemurMax1    550
#define cLMTibiaMin1    -400
#define cLMTibiaMax1    750

#define cLFCoxaMin1     -650      //Mechanical limits of the Left Front Leg
#define cLFCoxaMax1     650
#define cLFFemurMin1    -900
#define cLFFemurMax1    550
#define cLFTibiaMin1    -400
#define cLFTibiaMax1    750

#define cClawMin1      0         //Mechanical limits of the Claw
#define cClawMax1      180
//--------------------------------------------------------------------
//[START POSITIONS]
#define cHexInitXZ	 90	 
#define CHexInitXZCos60  45
#define CHexInitXZSin60  78
#define CHexInitY	80


#define cRRInitPosX     CHexInitXZCos60      //Start positions of the Right Rear leg
#define cRRInitPosY     CHexInitY
#define cRRInitPosZ     CHexInitXZSin60

#define cRMInitPosX     cHexInitXZ      //Start positions of the Right Middle leg
#define cRMInitPosY     CHexInitY
#define cRMInitPosZ     0

#define cRFInitPosX     CHexInitXZCos60      //Start positions of the Right Front leg
#define cRFInitPosY     CHexInitY
#define cRFInitPosZ     -CHexInitXZSin60

#define cLRInitPosX     CHexInitXZCos60      //Start positions of the Left Rear leg
#define cLRInitPosY     CHexInitY
#define cLRInitPosZ     CHexInitXZSin60

#define cLMInitPosX     cHexInitXZ      //Start positions of the Left Middle leg
#define cLMInitPosY     CHexInitY
#define cLMInitPosZ     0

#define cLFInitPosX     CHexInitXZCos60      //Start positions of the Left Front leg
#define cLFInitPosY     CHexInitY
#define cLFInitPosZ     -CHexInitXZSin60

#define cClawInitPosX     0      //Start positions of the Claw
//--------------------------------------------------------------------
// Add at the bottom of beetle_hdr.h
// In beetle_hdr.h add these constants:
#define COXAS_SIT_ANGLE 90
#define FEMURS_SIT_ANGLE 90
#define TIBIAS_SIT_ANGLE 90

// Existing standing positions remain:
#define COXAS_STAND_ANGLE 90
#define FEMURS_STAND_ANGLE 10
#define TIBIAS_STAND_ANGLE 80