#include "data.h"
#include "leg.h"
#include "movements.h"


void movements::initializeAllServos(float angleC, float angleF, float angleT) {
    for(uint8_t i = 0; i < 6; i++) {
        legs[i].setAngle(servoPins[i][Coxa], 0, angleC); // 0 for offset bc haven't set it yet
        legs[i].setAngle(servoPins[i][Femur], 0, angleF);
        if (i==0) legs[i].setAngle(servoPins[i][Tibia], 0, angleT, 0);
        else if (i==1) legs[i].setAngle(servoPins[i][Tibia], 0, angleT, 1);
        else legs[i].setAngle(servoPins[i][Tibia], 0, angleT);

        currAngles[i][Coxa] = angleC;
        currAngles[i][Femur] = angleF;
        currAngles[i][Tibia] = angleT;
    }
    delay(SWEEP_DELAY); 
}

//helper for forward walk - tripod gait
void movements::angleTab (int angles[6][3], int walk, int group2 [4][3], int group1 [4][3]) {
    for (int j =0; j<3; ++j) {
        if (j == 0) {
            angles[0][j] = group1[walk][j];
            angles[1][j] = 180 - group2[walk][j];
            angles[2][j] = group2[walk][j];
            angles[3][j] = 180 - group1[walk][j];
            angles[4][j] = group1[walk][j];
            angles[5][j] = 180 - group2[walk][j];
        } else {
            angles[0][j] = group1[walk][j];
            angles[1][j] = group2[walk][j];
            angles[2][j] = group2[walk][j];
            angles[3][j] = group1[walk][j];
            angles[4][j] = group1[walk][j];
            angles[5][j] = group2[walk][j];
        }
    }
}

void movements::forward() {
    initializeAllServos(standAngle[Coxa], standAngle[Femur], standAngle[Tibia]);
    int coxaInterval = 25; //for legs that need to move towards each other, can do +- 25 from default pos
    int stepCounter = 35;

    leg body [6] = {lf, rf, lm, rm, lb, rb}; 
    //walk 1-5
    int group1[4][3] = {
        {100, 115, 145},
        {110, 70, 145},

        {100, 65, 145},
        {90, 70, 145}
    };

    int group2[4][3] = {
        {80, 65, 145},
        {70, 70, 145},

        {80, 115, 145},
        {90, 70, 145}
    };

    int  walk [6][3];

    for (int i = 0; i< 4; ++i) {

        angleTab(walk, 0, group2, group1);
        //walk1
        interpolateAngle(body, walk, stepCounter); 
        
        angleTab(walk, 1, group2, group1);
        //walk2
        interpolateAngle(body, walk, stepCounter); 

        delay(SWEEP_DELAY);
    
        angleTab(walk, 2, group2, group1);
        //walk 3
        interpolateAngle(body, walk, stepCounter); 

        angleTab(walk, 3, group2, group1);
        //walk4
        interpolateAngle(body, walk, stepCounter);  


    }
}

void movements::interpolateAngle(leg body [6], int finalAngles [6][3], int stepNumber) {
    
    //current angles of the legs
    int from [6][3] = 
        {
            {currAngles[0][Coxa], currAngles[0][Femur], currAngles[0][Tibia]},
            {currAngles[1][Coxa], currAngles[1][Femur], currAngles[1][Tibia]},
            {currAngles[2][Coxa], currAngles[2][Femur], currAngles[2][Tibia]},
            {currAngles[3][Coxa], currAngles[3][Femur], currAngles[3][Tibia]},
            {currAngles[4][Coxa], currAngles[4][Femur], currAngles[4][Tibia]},
            {currAngles[5][Coxa], currAngles[5][Femur], currAngles[5][Tibia]},
        };


    for (int i = 0; i<=stepNumber; ++i) {

        int angles [6][3] = {
            //lf
            {map(i, 0, stepNumber, from[0][Coxa], finalAngles[0][Coxa]),
            map(i, 0, stepNumber, from[0][Femur], finalAngles[0][Femur]),
            map(i, 0, stepNumber, from[0][Tibia], finalAngles[0][Tibia])},
            //rf
            {map(i, 0, stepNumber, from[1][Coxa], finalAngles[1][Coxa]),
            map(i, 0, stepNumber, from[1][Femur], finalAngles[1][Femur]),
            map(i, 0, stepNumber, from[1][Tibia], finalAngles[1][Tibia])},
            
            {map(i, 0, stepNumber, from[2][Coxa], finalAngles[2][Coxa]),
            map(i, 0, stepNumber, from[2][Femur], finalAngles[2][Femur]),
            map(i, 0, stepNumber, from[2][Tibia], finalAngles[2][Tibia])},
            {map(i, 0, stepNumber, from[3][Coxa], finalAngles[3][Coxa]),
            map(i, 0, stepNumber, from[3][Femur], finalAngles[3][Femur]),
            map(i, 0, stepNumber, from[3][Tibia], finalAngles[3][Tibia])},
            {map(i, 0, stepNumber, from[4][Coxa], finalAngles[4][Coxa]),
            map(i, 0, stepNumber, from[4][Femur], finalAngles[4][Femur]),
            map(i, 0, stepNumber, from[4][Tibia], finalAngles[4][Tibia])},
            {map(i, 0, stepNumber, from[5][Coxa], finalAngles[5][Coxa]),
            map(i, 0, stepNumber, from[5][Femur], finalAngles[5][Femur]),
            map(i, 0, stepNumber, from[5][Tibia], finalAngles[5][Tibia])},
        };

        for (int j = 0; j < 6; ++j) {
            if (i == stepNumber) {
                body[j].setAngle(body[j].pinC, 0, finalAngles[j][Coxa]);
                body[j].setAngle(body[j].pinF, 0, finalAngles[j][Femur]);
                body[j].setAngle(body[j].pinT, 0, finalAngles[j][Tibia], j);

            } else {
                body[j].setAngle(body[j].pinC, 0, angles[j][Coxa]);
                body[j].setAngle(body[j].pinF, 0, angles[j][Femur]);
                body[j].setAngle(body[j].pinT, 0, angles[j][Tibia],j);
            }
        }
        delay(SWEEP_DELAY);
    
    }

    for (int i = 0; i<6; ++i) {
        currAngles[i][Coxa] = finalAngles[i][Coxa];
        currAngles[i][Femur] = finalAngles[i][Femur];
        currAngles[i][Tibia] = finalAngles[i][Tibia];
    }

}