#include "data.h"
#include "leg.h"
#include "movements.h"


void movements::initializeAllServos(float angleC, float angleF, float angleT) {
    for(uint8_t i = 0; i < 6; i++) {
        legs[i]->setAngle(servoPins[i][Coxa], 0, angleC); // 0 for offset bc haven't set it yet
        legs[i]->setAngle(servoPins[i][Femur], 0, angleF);
        if (i==0) legs[i]->setAngle(servoPins[i][Tibia], 0, angleT, 0);
        else if (i==1) legs[i]->setAngle(servoPins[i][Tibia], 0, angleT, 1);
        else legs[i]->setAngle(servoPins[i][Tibia], 0, angleT);

        currAngles[i][Coxa] = angleC;
        currAngles[i][Femur] = angleF;
        currAngles[i][Tibia] = angleT;
    }
    delay(SWEEP_DELAY); 
}

void movements::sitUp () {
    initializeAllServos(sitAngle[Coxa], sitAngle[Femur], sitAngle[Tibia]);

    
}

//helper for a tripod gait
void movements::angleTab2 (int angles[6][3], int walk, int group2 [4][3], int group1 [4][3]) {
    for (int j =0; j<3; ++j) {
        if (j == 0) {
            angles[0][j] = group1[walk][j]; // lf
            angles[1][j] = 180 - group2[walk][j]; // rf
            angles[2][j] = group2[walk][j]; // lm
            angles[3][j] = 180 - group1[walk][j]; // rm
            angles[4][j] = group1[walk][j]; //lb
            angles[5][j] = 180 - group2[walk][j]; //rb
        } else {
            angles[0][j] = group1[walk][j]; // lf
            angles[1][j] = group2[walk][j]; // rf
            angles[2][j] = group2[walk][j]; // lm
            angles[3][j] = group1[walk][j]; // rm
            angles[4][j] = group1[walk][j]; // lb
            angles[5][j] = group2[walk][j]; // rb
        }
    }
}

//middle legs have less range of coxa movements as front and back, so needs to rotate less
void movements::angleTab4(int angles[6][3], int walk, int group1 [4][3], int group2[4][3], 
            int middleGroup1 [4][3], int middleGroup2 [4][3]) {
        for (int j =0; j<3; ++j) {
            if (j == 0) {
                angles[0][j] = group1[walk][j]; // lf
                angles[1][j] = group2[walk][j]; // rf
                angles[2][j] = middleGroup2[walk][j]; // lm
                angles[3][j] = middleGroup1[walk][j]; // rm
                angles[4][j] = group1[walk][j]; //lb
                angles[5][j] = group2[walk][j]; //rb
            } else {
                angles[0][j] = group1[walk][j]; // lf
                angles[1][j] = group2[walk][j]; // rf
                angles[2][j] = middleGroup2[walk][j]; // lm
                angles[3][j] = middleGroup1[walk][j]; // rm
                angles[4][j] = group1[walk][j]; // lb
                angles[5][j] = group2[walk][j]; // rb
            }
        }  
    }

void movements::rotation(int direction){
    initializeAllServos(standAngle[Coxa], standAngle[Femur], standAngle[Tibia]);
    int coxaWideOffset = 25;
    int coxaNarrowOffset = 10;
    int cycleNumber = 5;

    int way = 1; //clockwise
    if (!direction) way = -1; //counterclockwise
    
    int group1[6][3] = { 
        {90 + way*(coxaWideOffset/2), 115, 145},  
        {90 + way*coxaWideOffset, 70, 145},

        {90 - way*(coxaWideOffset/4), 70, 145},
        {90 - way*(coxaWideOffset/2), 70, 145},
    };
    int group2[4][3] = { 
        {90 - way*(coxaWideOffset/4), 70, 145},
        {90 - way*(coxaWideOffset/2), 70, 145},

        {90 + way*(coxaWideOffset/2), 115, 145},
        {90 + way*coxaWideOffset, 70, 145}
    };
    int middleGroup1[4][3] = {
        {90 + way*(coxaNarrowOffset/2), 115, 145},
        {90 + way*coxaNarrowOffset, 70, 145},

        {90 - way*(coxaNarrowOffset/2), 70, 145}, 
        {90 - way*coxaNarrowOffset, 70, 145}
    };
    int middleGroup2[4][3] = {
        {90 - way*(coxaNarrowOffset/2), 70, 145},
        {90 - way*coxaNarrowOffset, 70, 145}, 

        {90 + way*(coxaNarrowOffset/2), 115, 145},
        {90 + way*(coxaNarrowOffset), 70, 145}
    };

    int rotate [6][3];

    for (int i = 0; i<cycleNumber; ++i) {
        if (i == cycleNumber - 1) { //sets the robot back straight, three by three (legs) 
            int stand1 [6][3] = {
                {90, 115, 145},
                {group2[3][0], 70, 145}, //doesn't move
                {middleGroup2[3][0], 70, 145},
                {90, 115, 145},
                {90, 115, 145},
                {group2[3][0], 70, 145}
            };
            interpolateAngle(legs, stand1, stepCounter);
            int stand2 [6][3] = {
                {90, 70, 145},
                {group2[3][0], 70, 145},
                {middleGroup2[3][0], 70, 145},
                {90, 70, 145},
                {90, 70, 145},
                {group2[3][0], 70, 145}
            };
            interpolateAngle(legs, stand2, stepCounter);
            int stand3 [6][3] = {
                {90, 70, 145},
                {90, 115, 145},
                {90, 115, 145},
                {90, 70, 145},
                {90, 70, 145},
                {90, 115, 145}
            };
            interpolateAngle(legs, stand3, stepCounter);
            int stand4 [6][3] = {
                {90, 70, 145},
                {90, 70, 145},
                {90, 70, 145},
                {90, 70, 145},
                {90, 70, 145},
                {90, 70, 145}
            };
            interpolateAngle(legs, stand4, stepCounter);
        } else {
            angleTab4(rotate, 0, group1, group2, middleGroup1, middleGroup2);
            //rotate1
            interpolateAngle(legs, rotate, stepCounter); 
            
            angleTab4(rotate, 1, group1, group2, middleGroup1, middleGroup2);
            //rotate2
            interpolateAngle(legs, rotate, stepCounter); 
        
            angleTab4(rotate, 2, group1, group2, middleGroup1, middleGroup2);

            //rotate3
            interpolateAngle(legs, rotate, stepCounter); 

            angleTab4(rotate, 3, group1, group2, middleGroup1, middleGroup2);
            //rotate4
            interpolateAngle(legs, rotate, stepCounter);
        }
    }
}

void movements::forward() {
    initializeAllServos(standAngle[Coxa], standAngle[Femur], standAngle[Tibia]);

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

        angleTab2(walk, 0, group2, group1);
        //walk1
        interpolateAngle(legs, walk, stepCounter); 
        
        angleTab2(walk, 1, group2, group1);
        //walk2
        interpolateAngle(legs, walk, stepCounter); 

        delay(SWEEP_DELAY);
    
        angleTab2(walk, 2, group2, group1);
        //walk 3
        interpolateAngle(legs, walk, stepCounter); 

        angleTab2(walk, 3, group2, group1);
        //walk4
        interpolateAngle(legs, walk, stepCounter);  


    }
}

void movements::sideways(int direction){
    // direction = 1 for left, 0 for right
    initializeAllServos(standAngle[Coxa], standAngle[Femur], standAngle[Tibia]);
    if (direction == 1) { //left

    } else { //right
        int groupA[4][3] = { // lf and lb
            
        };

        int groupB[4][3] = { // rf and rb
            
        };
        int groupC[4][3] = { // lm
            
        };
        int groupD[4][3] = { // rm
            
        };
        
    }


}

void movements::interpolateAngle(leg* body[6], int finalAngles[6][3], int stepNumber) {
    
    //current angles of the legs
    int from [6][3] = 
        {
            {this->currAngles[0][Coxa], this->currAngles[0][Femur], this->currAngles[0][Tibia]},
            {this->currAngles[1][Coxa], this->currAngles[1][Femur], this->currAngles[1][Tibia]},
            {this->currAngles[2][Coxa], this->currAngles[2][Femur], this->currAngles[2][Tibia]},
            {this->currAngles[3][Coxa], this->currAngles[3][Femur], this->currAngles[3][Tibia]},
            {this->currAngles[4][Coxa], this->currAngles[4][Femur], this->currAngles[4][Tibia]},
            {this->currAngles[5][Coxa], this->currAngles[5][Femur], this->currAngles[5][Tibia]},
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
                body[j]->setAngle(body[j]->pinC, 0, finalAngles[j][Coxa]);
                body[j]->setAngle(body[j]->pinF, 0, finalAngles[j][Femur]);
                body[j]->setAngle(body[j]->pinT, 0, finalAngles[j][Tibia], j);

            } else {
                body[j]->setAngle(body[j]->pinC, 0, angles[j][Coxa]);
                body[j]->setAngle(body[j]->pinF, 0, angles[j][Femur]);
                body[j]->setAngle(body[j]->pinT, 0, angles[j][Tibia], j);
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