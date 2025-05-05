#ifndef beetlebot_H
#define beetlebot_H

#include <WString.h>
#include "leg.h"
#include "data.h"

/* BeetleBot Servo and Leg configurations:

                Claw
            LF        RF
            LM        RM
            LB        RB
            
            X
            ^
            |
           Z. --> Y 

            X = Forward/backward movement
            Y = Left/right movement
            Z = Up/down movement

Body - Coxa - Femur - Tibia (end of feet)

 */

//Mapped Servos Pins: 

/*         front
    18 9  8  ---- 6 7 23
    12 11 10 ---- 3 4 5
    15 14 13 ---- 2 1 0
           back
*/
// where tibia, femur, coxa ---- coxa, femur, tibia





//format for tabs is always coxa, femur, tibia
/*
    Beetlebot is an instance of 6 legs, each with three servos that are controlled by the servoAction class
*/
class beetlebot {
    public:
    
    beetlebot(leg &lf, leg &rf, leg &lm, leg &rm, leg &lb, leg &rb)
    {
        legs[0] = &lf;
        legs[1] = &rf;
        legs[2] = &lm;
        legs[3] = &rm;
        legs[4] = &lb;
        legs[5] = &rb;
    }

    //Getters function
    float getOrientation();
    float getXPos();
    float getYPos();
    int getAction();

    bool moveHome();

    //move a group of legs to a position if reachable
    bool moveLegs(float positions[6][3]);


    //travel to path points
    void travelPath(float legPos[6][3], int pathPoints [][2], int numPoints, int iter, int stepHeight = 20);
    
    //function that perform a step
    void step(float legPos[6][3], float stepDirection, int radius, int stepNumber, int stepHeight = 20);

    //given a target position of the center of the robot : trans values, compute the new positions of the legs to achieve that position
    //will call moveLegs() to finalize the change of positions. 
    void computeBodyMov(float prevPositions[6][3], float newPositions[6][3], float xTrans, float yTrans, float zTrans, uint8_t legMask = 0b111111);

    void smoothStep(float newPositions[6][3], float prevPositions[6][3], float finalPositions[6][3], int stepHeight, int stepCounter, int stepNumber, bool moveRightGroup, bool moveAllLegs = true);

    //Main function that makes the robot walk
    //Called recursively a stepNumber of times. More iterations means a smoother motion for the robot (small changes in position instead of a big jump)
    bool computeStep(float prevPositions[6][3], float newPositions[6][3], float stepDirection, int radius, int stepNumber, int stepHeight = 20);

    //compute the two intersections of the line, and direction of the circle centered around mX, mY where circle : radius^2 = mX^2 + mY^2
    void circleLineIntersect (float mX, float mY, int radius, float pX, float pY, float direction, int intersections[2][2]);
    int getOppositeIntersection (float pX, float pY, float direction, int intersections[2][2]);
    
    void setStartSystem (float x, float y, float orientation);
    void setOrientation (float orientation);

    private:
        //flag indicating the current action of the beetlebot : 0 = at stop, 1 = doing a step
        int action = 0;
        int stepCounter = 1; //keeps track of the number of iterations

        // indicator whether to move legs rf, lm, rb or legs lf, rm, lb
        bool moveRightGroup = true;

        //previous step, used in computeStep
        float prevDirection = 0.0;
        bool prevRightGroup = true;

        float lengthAccuracy = 0.95;


        //for the whole robot
        float orientation = 0.0;
        float xPosition = 0.0;
        float yPosition = 0.0;

        //array to save the end points of a step
        float finalPositions [6][3] = {
            {homePositions[0][0], homePositions[0][1],homePositions[0][2]}, 
            {homePositions[1][0], homePositions[1][1],homePositions[1][2]}, 
            {homePositions[2][0], homePositions[2][1],homePositions[2][2]}, 
            {homePositions[3][0], homePositions[3][1],homePositions[3][2]}, 
            {homePositions[4][0], homePositions[4][1],homePositions[4][2]}, 
            {homePositions[5][0], homePositions[5][1],homePositions[5][2]}, 
        };

        
        float servoAngles [6][3]; //in degrees, 0 to 180
        float currentPositions[6][3];

        leg *legs[6];

        int accuracy = 30; //in mm
        
};
#endif