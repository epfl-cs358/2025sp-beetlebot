#include "data.h"
#include "leg.h"
#include "movements.h"


void movements::initializeAllServos(float angleC, float angleF, float angleT) {
    for(uint8_t i = 0; i < 6; i++) {
        legs[i]->setAngle(servoPins[i][Coxa], angleC); // 0 for offset bc haven't set it yet
        legs[i]->setAngle(servoPins[i][Femur], angleF);
        if (i==0) legs[i]->setAngle(servoPins[i][Tibia], angleT, 0);
        else if (i==1) legs[i]->setAngle(servoPins[i][Tibia], angleT, 1);
        else legs[i]->setAngle(servoPins[i][Tibia], angleT);

        currAngles[i][Coxa] = angleC;
        currAngles[i][Femur] = angleF;
        currAngles[i][Tibia] = angleT;
    }
    delay(SWEEP_DELAY); 
}

void movements::standUp() {
    initializeAllServos(sitAngle[Coxa], sitAngle[Femur], sitAngle[Tibia]);
    int sit1[6][3] = {
        {90, 115, 100},
        {90, 115, 100},
        {90, 115, 100},
        {90, 115, 100},
        {90, 115, 100},
        {90, 115, 100}
    };
    int sit2[6][3] = {
        {standAngle[Coxa], standAngle[Femur], standAngle[Tibia]},
        {standAngle[Coxa], standAngle[Femur], standAngle[Tibia]},
        {standAngle[Coxa], standAngle[Femur], standAngle[Tibia]},
        {standAngle[Coxa], standAngle[Femur], standAngle[Tibia]},
        {standAngle[Coxa], standAngle[Femur], standAngle[Tibia]},
        {standAngle[Coxa], standAngle[Femur], standAngle[Tibia]}
    };

    interpolateAngle(legs, sit1, stepCounter);
    interpolateAngle(legs, sit2, stepCounter);

}

void movements::sitDown() {
    initializeAllServos(standAngle[Coxa], standAngle[Femur], standAngle[Tibia]);
    int sit2[6][3] = {
        {90, 115, 100},
        {90, 115, 100},
        {90, 115, 100},
        {90, 115, 100},
        {90, 115, 100},
        {90, 115, 100}
    };
    int sit1[6][3] = {
        {sitAngle[Coxa], sitAngle[Femur], sitAngle[Tibia]},
        {sitAngle[Coxa], sitAngle[Femur], sitAngle[Tibia]},
        {sitAngle[Coxa], sitAngle[Femur], sitAngle[Tibia]},
        {sitAngle[Coxa], sitAngle[Femur], sitAngle[Tibia]},
        {sitAngle[Coxa], sitAngle[Femur], sitAngle[Tibia]},
        {sitAngle[Coxa], sitAngle[Femur], sitAngle[Tibia]}
    };

    interpolateAngle(legs, sit1, stepCounter);
    interpolateAngle(legs, sit2, stepCounter);
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
void movements::angleTab4(int angles[8][3], int walk, int group1 [8][3], int group2[8][3], 
            int middleGroup1 [8][3], int middleGroup2 [8][3]) {
        for (int j =0; j<3; ++j) {
            if (j == 0) {
                angles[0][j] = 180 - group1[walk][j]; // lf (have to do 180 - angle)
                angles[1][j] = 180 - group2[walk][j]; // rf
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

void movements::angleTabCorner (int angles[6][3], int walk, int group2 [4][3], int group1 [4][3]) {
    for (int j =0; j<3; ++j) {
        if (j == 0) {
            angles[0][j] = group1[walk][j]; // lf
            angles[1][j] = 180 - group2[walk][j]; // rf
            angles[2][j] = standAngle[j]; // lm
            angles[3][j] = 180 - standAngle[j]; // rm
            angles[4][j] = group1[walk][j]; //lb
            angles[5][j] = 180 - group2[walk][j]; //rb
        } else {
            angles[0][j] = group1[walk][j]; // lf
            angles[1][j] = group2[walk][j]; // rf
            angles[2][j] = standAngle[j]; // lm
            angles[3][j] = standAngle[j]; // rm
            angles[4][j] = group1[walk][j]; // lb
            angles[5][j] = group2[walk][j]; // rb
        }
    }
}
void movements::rotation(int way, int cycle){
    initializeAllServos(standAngle[Coxa], standAngle[Femur], standAngle[Tibia]);
    int coxaWideOffset = 25;
    int coxaNarrowOffset = 10;
    int cycleNumber = cycle;

    (way != -1 || way != 1) ? way = 1 : way; //if its neither ig we can make it turn clockwise
    
    int group1[6][3] = { 
        {90 + way*(coxaWideOffset/2), 95, 145},  
        {90 + way*coxaWideOffset, 50, 145},

        {90 - way*(coxaWideOffset/4), 50, 145},
        {90 - way*(coxaWideOffset/2), 50, 145},
    };
    int group2[4][3] = { 
        {90 - way*(coxaWideOffset/4), 50, 145},
        {90 - way*(coxaWideOffset/2), 50, 145},

        {90 + way*(coxaWideOffset/2), 95, 145},
        {90 + way*coxaWideOffset, 50, 145}
    };
    int middleGroup1[4][3] = {
        {90 + way*(coxaNarrowOffset/2), 95, 145},
        {90 + way*coxaNarrowOffset, 50, 145},

        {90 - way*(coxaNarrowOffset/2), 50, 145}, 
        {90 - way*coxaNarrowOffset, 50, 145}
    };
    int middleGroup2[4][3] = {
        {90 - way*(coxaNarrowOffset/2), 50, 145},
        {90 - way*coxaNarrowOffset, 50, 145}, 

        {90 + way*(coxaNarrowOffset/2), 95, 145},
        {90 + way*(coxaNarrowOffset), 50, 145}
    };

    int rotate [6][3];

    for (int i = 0; i<cycleNumber; ++i) {
        if (i == cycleNumber - 1) { //sets the robot back straight, three by three (legs) 
            int stand1 [6][3] = {
                {90, 95, 145},
                {group2[3][0], 50, 145}, //doesn't move
                {middleGroup2[3][0], 50, 145},
                {90, 95, 145},
                {90, 95, 145},
                {group2[3][0], 50, 145}
            };
            interpolateAngle(legs, stand1, stepCounter);
            int stand2 [6][3] = {
                {90, 50, 145},
                {group2[3][0], 50, 145},
                {middleGroup2[3][0], 50, 145},
                {90, 50, 145},
                {90, 50, 145},
                {group2[3][0], 50, 145}
            };
            interpolateAngle(legs, stand2, stepCounter);
            int stand3 [6][3] = {
                {90, 50, 145},
                {90, 95, 145},
                {90, 95, 145},
                {90, 50, 145},
                {90, 50, 145},
                {90, 95, 145}
            };
            interpolateAngle(legs, stand3, stepCounter);
            int stand4 [6][3] = {
                {90, 50, 145},
                {90, 50, 145},
                {90, 50, 145},
                {90, 50, 145},
                {90, 50, 145},
                {90, 50, 145}
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
    int cycle = 6;
    //walk 1-4
    int group1[4][3] = {
        {110, 50, 145},
        {90, 50, 145},

        {70, 50, 145},
        {90, 95, 145}
    };

    int group2[4][3] = {
        {70, 50, 145},
        {90, 95, 145},

        {110, 50, 145},
        {90, 50, 145}
    };

    int  walk [6][3];

    for (int i = 0; i< cycle; ++i) {
        if (i == 0) { //init for group1 : starts cycle in the air
            angleTab2(walk, 3, group2, group1);
            interpolateAngle(legs, walk, stepCounter);
        } else if (i == cycle - 1) { //restore for group1 : return to ground
            int standing [6][3]= {
                {90, 50, 145},
                {90, 50, 145},
                {90, 50, 145},
                {90, 50, 145},
                {90, 50, 145},
                {90, 50, 145}
            };
            interpolateAngle(legs, standing, stepCounter);
        } else {
            angleTab2(walk, 0, group2, group1);
            //walk1
            interpolateAngle(legs, walk, stepCounter); 
            
            angleTab2(walk, 1, group2, group1);
            //walk2
            interpolateAngle(legs, walk, stepCounter); 
        
            angleTab2(walk, 2, group2, group1);
            //walk 3
            interpolateAngle(legs, walk, stepCounter); 

            angleTab2(walk, 3, group2, group1);
            //walk4
            interpolateAngle(legs, walk, stepCounter);  

        }
    }
}

void movements::backward() {
    initializeAllServos(standAngle[Coxa], standAngle[Femur], standAngle[Tibia]);
    int cycle = 6;
    //walk 1-4
    int group1[4][3] = {
        {70, 50, 145},
        {90, 50, 145},

        {110, 50, 145},
        {90, 95, 145}
    };

    int group2[4][3] = {
        {110, 50, 145},
        {90, 95, 145},

        {70, 50, 145},
        {90, 50, 145}
    };

    int  walk [6][3];

    for (int i = 0; i< cycle; ++i) {
        if (i == 0) { //init for group1 : starts cycle in the air
            angleTab2(walk, 3, group2, group1);
            interpolateAngle(legs, walk, stepCounter);
        } else if (i == cycle - 1) { //restore for group1 : return to ground
            int standing [6][3]= {
                {90, 50, 145},
                {90, 50, 145},
                {90, 50, 145},
                {90, 50, 145},
                {90, 50, 145},
                {90, 50, 145}
            };
            interpolateAngle(legs, standing, stepCounter);
        } else {
            angleTab2(walk, 0, group2, group1);
            //walk1
            interpolateAngle(legs, walk, stepCounter); 
            
            angleTab2(walk, 1, group2, group1);
            //walk2
            interpolateAngle(legs, walk, stepCounter); 
        
            angleTab2(walk, 2, group2, group1);
            //walk 3
            interpolateAngle(legs, walk, stepCounter); 

            angleTab2(walk, 3, group2, group1);
            //walk4
            interpolateAngle(legs, walk, stepCounter);  

        }
    }
}
//we turn while forward, -1 ou 1 for way
//turn is a turning factor thats 0 < turn < 1
void movements::forwardCurve(int way, float turn, int cycle){
    if (turn > 1) turn = 1;
    if (turn < 0) turn = 0;
    (way != -1 || way != 1) ? way = 1 : way; //if its neither ig we can make it turn clockwise

    initializeAllServos(standAngle[Coxa], standAngle[Femur], standAngle[Tibia]);
    int coxaWideOffset = 25;
    int coxaNarrowOffset = 10;
    int cycleNumber = cycle;

    //gait definition copied from forward()
    int group1[4][3] = {
        {110, 50, 145},
        { 90, 50, 145},
        { 70, 50, 145},
        { 90, 95, 145}
    };
    int group2[4][3] = {
        { 70, 50, 145},
        { 90, 95, 145},
        {110, 50, 145},
        { 90, 50, 145}
    };
    
    //rotation tables copied from rotation()
    const int coxaWide  = 25;
    const int coxaNarrow= 10;
    int rot_g1[4][3] = {
        {90+way*(coxaWide/2), 95,145},
        {90+way* coxaWide   , 50,145},
        {90-way*(coxaWide/4), 50,145},
        {90-way*(coxaWide/2), 50,145}
    };
    int rot_g2[4][3] = {
        {90-way*(coxaWide/4), 50,145},
        {90-way*(coxaWide/2), 50,145},
        {90+way*(coxaWide/2),95,145},
        {90+way* coxaWide   , 50,145}
    };
    int rot_mg1[4][3] = {
        {90+way*(coxaNarrow/2),95,145},
        {90+way* coxaNarrow   , 50,145},
        {90-way*(coxaNarrow/2), 50,145},
        {90-way* coxaNarrow   , 50,145}
    };
    int rot_mg2[4][3] = {
        {90-way*(coxaNarrow/2), 50,145},
        {90-way* coxaNarrow   , 50,145},
        {90+way*(coxaNarrow/2),95,145},
        {90+way* coxaNarrow   , 50,145}
    };

    
    //thy work buffers
    int walkFwd [6][3]; //forward table
    int walkRot [6][3]; //rotation table for 1 step
    int walkMix [6][3]; //mixed output of forward and rotation

    //il y a 4 phases de marche, chaque phase correspond à un sous-étape
    //1 lift tripod A, swing forward
    //2 lift tripod B, swing forward
    //3 lower tripod A, push ground
    //4 lower tripod B, push ground
    const uint8_t SUBSTEPS = 4;
    for (int i = 0; i< 5; ++i){
        for (uint8_t phase = 0; phase < SUBSTEPS; ++phase) {
        angleTab2(walkFwd, phase, group2, group1);
        angleTab4(walkRot, phase, rot_g1, rot_g2, rot_mg1, rot_mg2);
        mixTables(walkMix, walkFwd, walkRot, turn * way);
        interpolateAngle(legs, walkMix, stepCounter);
    }
    }
    
}

void movements::backwardCurve(int way, float turn, int cycle){
    if (turn > 1) turn = 1;
    if (turn < 0) turn = 0;
    (way != -1 || way != 1) ? way = 1 : way;

    initializeAllServos(standAngle[Coxa], standAngle[Femur], standAngle[Tibia]);
    int coxaWideOffset = 25;
    int coxaNarrowOffset = 10;
    int cycleNumber = cycle;

    int group1[4][3] = {
        {70, 50, 145},
        {90, 50, 145},

        {110, 50, 145},
        {90, 95, 145}
    };
    int group2[4][3] = {
        {110, 50, 145},
        {90, 95, 145},

        {70, 50, 145},
        {90, 50, 145}
    };
    
    const int coxaWide  = 25;
    const int coxaNarrow= 10;
    int rot_g1[4][3] = {
        {90+way*(coxaWide/2), 95,145},
        {90+way* coxaWide   , 50,145},
        {90-way*(coxaWide/4), 50,145},
        {90-way*(coxaWide/2), 50,145}
    };
    int rot_g2[4][3] = {
        {90-way*(coxaWide/4), 50,145},
        {90-way*(coxaWide/2), 50,145},
        {90+way*(coxaWide/2),95,145},
        {90+way* coxaWide   , 50,145}
    };
    int rot_mg1[4][3] = {
        {90+way*(coxaNarrow/2),95,145},
        {90+way* coxaNarrow   , 50,145},
        {90-way*(coxaNarrow/2), 50,145},
        {90-way* coxaNarrow   , 50,145}
    };
    int rot_mg2[4][3] = {
        {90-way*(coxaNarrow/2), 50,145},
        {90-way* coxaNarrow   , 50,145},
        {90+way*(coxaNarrow/2),95,145},
        {90+way* coxaNarrow   , 50,145}
    };

    
    //thy work buffers
    int walkFwd [6][3]; //forward table
    int walkRot [6][3]; //rotation table for 1 step
    int walkMix [6][3]; //mixed output of forward and rotation

    //il y a 4 phases de marche, chaque phase correspond à un sous-étape
    //1 lift tripod A, swing forward
    //2 lift tripod B, swing forward
    //3 lower tripod A, push ground
    //4 lower tripod B, push ground
    const uint8_t SUBSTEPS = 4;

    for (uint8_t phase = 0; phase < SUBSTEPS; ++phase) {
        angleTab2(walkFwd, phase, group2, group1);
        angleTab4(walkRot, phase, rot_g1, rot_g2, rot_mg1, rot_mg2);
        mixTables(walkMix, walkFwd, walkRot, turn * way);
        interpolateAngle(legs, walkMix, stepCounter);
    }
    
}

void movements::sideways(int direction){
    // direction = 1 for left, 0 for right
    initializeAllServos(standAngle[Coxa], standAngle[Femur], standAngle[Tibia]);
    int cycle = 5; // coxa angles set for front ones, back have to do -180
    int group1[8][3] = { // lf and lb
        {90, 50, 145}, // 1st part
        {90, 50, 145},
        {115, 65, 145}, // 2nd part
        {135, 45, 140},
        {115, 50, 140}, // 3rd part
        {90, 50, 145},
        {90, 50, 145}, // 4th part
        {90, 50, 145}
    };
    int group2[8][3] = { // rf and rb
        {90, 50, 145}, // 1st part
        {90, 50, 145},
        {115, 50, 140}, // 2nd part
        {135, 45, 140},
        {115, 65, 145}, // 3rd part
        {90, 50, 145},
        {90, 50, 145}, // 4th part
        {90, 50, 145}
    };
    int middleGroup2[8][3] = { // lm
        {90, 70, 135}, // 1st part
        {90, 50, 125},
        {90, 50, 135}, // 2nd part
        {90, 50, 145},
        {90, 70, 145}, // 3rd part
        {90, 50, 145},
        {90, 50, 145}, // 4th part
        {90, 50, 145}
    };
    int middleGroup1[8][3] = { // rm
        {90, 50, 145}, // 1st part
        {90, 50, 145},
        {90, 70, 145}, // 2nd part
        {90, 50, 145},
        {90, 50, 135}, // 3rd part
        {90, 50, 125},
        {90, 70, 135}, // 4th part
        {90, 50, 145}
    };
    
    int crabWalk [8][3];
    for (int i = 0; i<cycle; i++) {
        if (direction == 1) { //left 
            angleTab4(crabWalk, 0, group1, group2, middleGroup1, middleGroup2);
            interpolateAngle(legs, crabWalk, stepCounter); 
        
            angleTab4(crabWalk, 1, group1, group2, middleGroup1, middleGroup2);
            interpolateAngle(legs, crabWalk, stepCounter); 
    
            angleTab4(crabWalk, 2, group1, group2, middleGroup1, middleGroup2);
            interpolateAngle(legs, crabWalk, stepCounter); 

            angleTab4(crabWalk, 3, group1, group2, middleGroup1, middleGroup2);
            interpolateAngle(legs, crabWalk, stepCounter);
        } else { //right
            angleTab4(crabWalk, 0, group2, group1, middleGroup2, middleGroup1);
            interpolateAngle(legs, crabWalk, stepCounter); 
        
            angleTab4(crabWalk, 1, group2, group1, middleGroup2, middleGroup1);
            interpolateAngle(legs, crabWalk, stepCounter); 
    
            angleTab4(crabWalk, 2, group2, group1, middleGroup2, middleGroup1);
            interpolateAngle(legs, crabWalk, stepCounter); 

            angleTab4(crabWalk, 3, group2, group1, middleGroup2, middleGroup1);
            interpolateAngle(legs, crabWalk, stepCounter);
        }
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
                body[j]->setAngle(body[j]->pinC, finalAngles[j][Coxa]);
                body[j]->setAngle(body[j]->pinF, finalAngles[j][Femur]);
                body[j]->setAngle(body[j]->pinT, finalAngles[j][Tibia], j);

            } else {
                body[j]->setAngle(body[j]->pinC, angles[j][Coxa]);
                body[j]->setAngle(body[j]->pinF, angles[j][Femur]);
                body[j]->setAngle(body[j]->pinT, angles[j][Tibia], j);
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

// mix two 6×3 angle tables:  dst = (1-λ)*a + λ*b
inline void mixTables(int dst[6][3], const int a[6][3], const int b[6][3], float lambda){
    for (uint8_t i = 0; i < 6; ++i){
        for (uint8_t j = 0; j < 3; ++j){
            dst[i][j] = a[i][j] + lambda * (b[i][j] - a[i][j]);
        }
    }
}
