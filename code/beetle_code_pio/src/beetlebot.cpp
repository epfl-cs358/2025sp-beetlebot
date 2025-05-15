#include "beetlebot.h"

Adafruit_PWMServoDriver multiplex = Adafruit_PWMServoDriver(0x40);

void alternateLegGroup (bool moveRightGroup, int lifted[3], int grounded[3]) {
    if (!moveRightGroup) {
        lifted[0] = LF;
        lifted[1] = RM;
        lifted[2] = LB;

        grounded[0] = RF;
        grounded[1] = LM;
        grounded[2] = RB;
    }
    return;
}

void beetlebot::smoothStep(float newPositions[6][3], float prevPositions[6][3], float finalPositions[6][3], int stepHeight, int stepCounter, int stepNumber, bool moveRightGroup, bool moveAllLegs) {
    stepCounter = _min(stepNumber, stepCounter);

    int liftedLegs[3] = {RF,LM,RB};
    int groundedLegs[3] = {LF,RM,LB};
    alternateLegGroup(moveRightGroup, liftedLegs, groundedLegs);

    //grounded legs movement
    if (moveAllLegs) {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) { //x, y, z
                //transitions gradually from prev to final positions thanks to stepCounter ∈ [0, stepNumber]
                newPositions[groundedLegs[i]][j] = mapF(stepCounter, 0.0, (float) stepNumber, prevPositions[groundedLegs[i]][j], finalPositions[groundedLegs[i]][j]);
            }
        }
    }

    //lifted legs movement: 

    float startIter = ceil(stepNumber * 0.2);    
    float targetIter = floor(stepNumber * 0.75);

    //we first move the leg in the x, y plane (no lowering) - target phase
    if (stepCounter >= startIter && stepCounter <= targetIter) {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 2; ++j) { //x, y
                newPositions[liftedLegs[i]][j] = mapF(stepCounter, startIter, targetIter, prevPositions[liftedLegs[i]][j], finalPositions[liftedLegs[i]][j]);
            }
        }
    //after this : movement is vertical so we don't really move in x,y anymore
    } else if (stepCounter < startIter) { //prev horizontal position
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 2; ++j) {
                newPositions[liftedLegs[i]][j] = prevPositions[liftedLegs[i]][j];
            }
        }
    } else { //after targetIter
        for (int i = 0; i < 3; ++i) { //final horizontal position
            for (int j = 0; j < 2; ++j) {
                newPositions[liftedLegs[i]][j] = finalPositions[liftedLegs[i]][j];
            }
        }
    }

    // vertical movement 
    float upSteps = ceil(stepNumber * 0.15);     // legs are lifted in the first 15% of the step
    float downSteps = floor(stepNumber * 0.85);  // legs are lowered in the last 15% og the step
    if (stepCounter < upSteps) {
        //lift the legs - by step height 
        for (int i = 0; i < 3; ++i) {
            newPositions[liftedLegs[i]][2] = mapF(stepCounter, 0.0, upSteps, prevPositions[liftedLegs[i]][2], finalPositions[liftedLegs[i]][2] - stepHeight);
        }
    } else if (stepCounter > downSteps) {
        //lower the legs - by step height
        for (int i = 0; i < 3; ++i) {
            newPositions[liftedLegs[i]][2] = mapF(stepCounter, downSteps, (float) stepNumber, prevPositions[liftedLegs[i]][2] - stepHeight, finalPositions[liftedLegs[i]][2]);
        }
    } else {
        // keep the legs lifted - perform horizontal during this stage
        for (int i = 0; i < 3; ++i) {
            newPositions[liftedLegs[i]][2] = prevPositions[liftedLegs[i]][2] - stepHeight;
        }
    }
}

/*
    path points is a list of target position on x,y to follow
*/
void beetlebot::travelPath(float legPos[6][3], int pathPoints [][2], int numPoints, int iter, int stepHeight) {
    for (int i = 0; i < numPoints; ++i) {

        //current position and path point distance
        float distToGoal = (this->getXPos()-pathPoints[i][0])*(this->getXPos()-pathPoints[i][0]) + (this->getYPos()-pathPoints[i][1]) * (this->getYPos()-pathPoints[i][1]);
        distToGoal = sqrt(distToGoal);

        //threshold - how close enough we can get
        while (distToGoal> accuracy) {

            //angle (rad) to the wanted path point (do we need to rotate the direction of the robot ?)
            float goalDirection = atan2(pathPoints[i][1] - this->getYPos(), pathPoints[i][0] - this->getXPos());

            float directionDiff = goalDirection - this->getOrientation();
            //keep the angle direction between -π and π
            if (directionDiff > MYPI) {
                directionDiff -= 2 * MYPI;
            } else if (directionDiff < -MYPI) {
                directionDiff += 2 * MYPI;
            }

            uint8_t radius = min((int) distToGoal / 2, 30);

            // execute the step
            this->step(legPos, directionDiff, radius, iter, stepHeight);

            // recompute the distance to the goal position
            distToGoal = (this->getXPos()-pathPoints[i][0])*(this->getXPos()-pathPoints[i][0]) + (this->getYPos()-pathPoints[i][1]) * (this->getYPos()-pathPoints[i][1]);
            distToGoal = sqrt(distToGoal);
        }
    }
}

void beetlebot::step(float legPos[6][3], float stepDirection, int radius, int stepNumber, int stepHeight) {
  float posArray[6][3];

  //do while step is not completed
  do {
    unsigned long startTime = millis();
    this->computeStep(legPos, posArray, stepDirection, radius, stepNumber, stepHeight);
    this->moveLegs(posArray);
    //leaves time for the esp to compute 
    while (millis() < startTime + periodMs) {}
  } while (this->getAction() != 0);
}

/*
    Selects the leg to move, compute the final positions, and interpolate them
*/
bool beetlebot::computeStep(float prevPositions[6][3], float newPositions[6][3], float stepDirection, int radius, int stepNumber, int stepHeight) {
    //new step
   if (stepCounter ==1) {
        action = 1;
        int liftedLegs[3] = {LF, RM, LB};
        moveRightGroup = false;

        int groundedLegs[3] = {RF,LM,RB};

        //if the robot needs to make a sharp turn, the legs groups are swapped
        if ((prevRightGroup == false && abs(stepDirection - prevDirection) <= MYPI / 2) ||
            (prevRightGroup == true && abs(stepDirection - prevDirection) > MYPI/2)) {
                alternateLegGroup(false, groundedLegs, liftedLegs);
                moveRightGroup = true;
            }
        
        //compute the end positions of the lifted legs
        for (int i = 0; i < 3; ++i) {
            // x value
            finalPositions[liftedLegs[i]][0] = homePositions[0] + radius * cos(stepDirection + legCoords[liftedLegs[i]][2]);
            // y value
            finalPositions[liftedLegs[i]][1] = homePositions[1] + radius * sin(stepDirection + legCoords[liftedLegs[i]][2]);
        }
      
        // Compute final positions for grounded legs, with a curved trajectory, to avoid collision
        for (uint8_t i = 0; i < 3; ++i) {
            int intersections[2][2] = {0};
            circleLineIntersect(
                homePositions[0], homePositions[1], radius,
                prevPositions[groundedLegs[i]][0],
                prevPositions[groundedLegs[i]][1],
                stepDirection + legCoords[groundedLegs[i]][2],
                intersections);
    
            int index = getOppositeIntersection(
                prevPositions[groundedLegs[i]][0],
                prevPositions[groundedLegs[i]][1],
                stepDirection + legCoords[groundedLegs[i]][2],
                intersections);
    
            if (intersections[index][0] != NO_INTERSECTION && intersections[index][1] != NO_INTERSECTION) {
                finalPositions[groundedLegs[i]][0] = intersections[index][0];
                finalPositions[groundedLegs[i]][1] = intersections[index][1];
            } else { //no intersection with the moving legs, don't move the grounded legs
                finalPositions[groundedLegs[i]][0] = prevPositions[groundedLegs[i]][0];
                finalPositions[groundedLegs[i]][1] = prevPositions[groundedLegs[i]][1];
            }
        }

        // Compute how far each grounded leg moves

        float stepLengths[3] = {0.0};
        float avgLength = 0.0;
        for (int i = 0; i < 3; ++i) {
            //calculate the step length in x,y
            float dx = prevPositions[groundedLegs[i]][0] - finalPositions[groundedLegs[i]][0];
            float dy = prevPositions[groundedLegs[i]][1] - finalPositions[groundedLegs[i]][1];
            stepLengths[i] = dx * dx + dy * dy;
            avgLength += stepLengths[i];
        }
        avgLength = avgLength / 3; //3 grounded legs, we want the length to be equal

        // get the shortest step
        float minLength = stepLengths[0];
        for (int i = 1; i < 3; ++i) {
            if (stepLengths[i] < minLength) {
                minLength = stepLengths[i];
            }
        }

        // if a step deviates by 10% compared to others, we need to scale the final legs
        float tolerance = 0.1;
        bool adjustSteps = false;
        for (int i = 0; i < 3; ++i) {
            if (abs(stepLengths[i] - avgLength) > avgLength * tolerance) {
                adjustSteps = true;
                break;
            }
        }

        if (adjustSteps == true) {
            for (int i = 0; i < 3; ++i) {
                //direction x, y
                float dx = finalPositions[groundedLegs[i]][0] - prevPositions[groundedLegs[i]][0];
                float dy = finalPositions[groundedLegs[i]][1] - prevPositions[groundedLegs[i]][1];

                if (dx != 0.0 || dy != 0.0) {
                    float scale = sqrt(max((double) minLength / (dx * dx + dy * dy), 0.0));
                    finalPositions[groundedLegs[i]][0] = prevPositions[groundedLegs[i]][0] + dx * scale;
                    finalPositions[groundedLegs[i]][1] = prevPositions[groundedLegs[i]][1] + dy * scale;
                }
            }

            xPosition += sqrt(minLength) * cos(stepDirection + orientation) * lengthAccuracy;
            yPosition += sqrt(minLength) * sin(stepDirection + orientation) * lengthAccuracy;
        } else {
            xPosition += sqrt(avgLength) * cos(stepDirection + orientation) * lengthAccuracy;
            yPosition += sqrt(avgLength) * sin(stepDirection + orientation) * lengthAccuracy;
        }
    }

    //interpolate the leg movement
    if (stepCounter > 0 && stepCounter <= stepNumber) {
          smoothStep(newPositions, prevPositions, finalPositions, stepHeight, stepCounter, stepNumber, moveRightGroup);
    }

     //step completion
    if (stepCounter == stepNumber) {
        //switch group - tripod gait
        if (moveRightGroup) {
            prevRightGroup = true;
        } else {
            prevRightGroup = false;
        }

        prevDirection = stepDirection;

        for (int i = 0; i < 6; ++i) {
            for (int j = 0; j < 3; ++j) {
              prevPositions[i][j] = finalPositions[i][j];
            }
        }

        //resets
        stepCounter = 1;
        action = 0;
        return true;
    } 
    stepCounter++;
    return false;
}

/*
    Simulates a translation of the robot's body in 3d space, legMask selects each of the leg that should be moved.
*/
void beetlebot::computeBodyMov(float prevPositions[6][3], float newPositions[6][3], float xTrans, 
    float yTrans, float zTrans, uint8_t legMask) {
    
    //copy - only legs selected in legMask will be modified
    for (int i = 0; i<6; i++) {
        for (int j = 0; j<3; ++j) {
            newPositions[i][j] = prevPositions[i][j];
        }
    }    

    for (int i = 0; i<6; ++i) {
        if (legMask & (1 << (5 -i))) { //leg moves

            //rotate to align with the robot frame
            float temp = newPositions[i][0];
            newPositions[i][0] = newPositions[i][0] * cos(-legCoords[i][2]) - newPositions[i][1] * sin(-legCoords[i][2]);
            newPositions[i][1] = temp * sin(-legCoords[i][2]) + newPositions[i][1] * cos(-legCoords[i][2]);

            //shift to the CoM
            newPositions[i][0] += legCoords[i][0];
            newPositions[i][1] += legCoords[i][1];

            //translate to the wanted coordinates
            newPositions[i][0] -= xTrans;
            newPositions[i][1] -= yTrans;
            newPositions[i][2] += zTrans;

            // Shift back to the CoM
            newPositions[i][0] -= legCoords[i][0];
            newPositions[i][1] -= legCoords[i][1];

            // Rotate back to the leg coxa origin frame
            temp = newPositions[i][0];
            newPositions[i][0] = newPositions[i][0] * cos(legCoords[i][2]) - newPositions[i][1] * sin(legCoords[i][2]);
            newPositions[i][1] = temp * sin(legCoords[i][2]) + newPositions[i][1] * cos(legCoords[i][2]);
        }
    }
}

bool beetlebot::moveHome() {
    bool moved = legs[0]->moveTo(homePositions[0], homePositions[1], homePositions[2])
    && legs[1]->moveTo(homePositions[0], homePositions[1], homePositions[2])
    && legs[2]->moveTo(homePositions[0], homePositions[1], homePositions[2])
    && legs[3]->moveTo(homePositions[0], homePositions[1], homePositions[2])
    && legs[4]->moveTo(homePositions[0], homePositions[1], homePositions[2])
    && legs[5]->moveTo(homePositions[0], homePositions[1], homePositions[2]);

    return moved;
}

bool beetlebot::moveLegs(float positions[6][3]) {
    bool moved = legs[0]->moveTo(positions[0][0], positions[0][1], positions[0][2])
    && legs[1]->moveTo(positions[1][0], positions[1][1], positions[1][2])
    && legs[2]->moveTo(positions[2][0], positions[2][1], positions[2][2])
    && legs[3]->moveTo(positions[3][0], positions[3][1], positions[3][2])
    && legs[4]->moveTo(positions[4][0], positions[4][1], positions[4][2])
    && legs[5]->moveTo(positions[5][0], positions[5][1], positions[5][2]);

    return moved;
}

//computes the intersection points between the line (pX, pY) and the circle (mX, mY) of given radius (we can have up to two points)
void beetlebot::circleLineIntersect (float mX, float mY, int radius, float pX, float pY, float direction, int intersections[2][2]) {
    //center the circle at (0,0)
    float shiftedPX = pX - mX;
    float shiftedPY = pY - mY;

    //(x,y) coordinate of the direction vector
    float dx = cos(direction);
    float dy = sin(direction);

    /*  x = shiftedPx + t * dx
        y = shiftedPx + t * dy
        x^2 + y^2 = radius^2
         => a*t^2 + b*t +c = 0
        t is the position along the line that we want to find
    */
    float a = dx * dx + dy * dy;
    float b = 2 * (dx * shiftedPX + dy * shiftedPY);
    float c = shiftedPX * shiftedPX + shiftedPY * shiftedPY - radius * radius;

    float discriminant = b * b - 4 * a * c;

    if (discriminant < -1e-6) { //assume we don't have intersection (in int, would be 0 but account for errors)
        intersections[0][0] = intersections[0][1] = NO_INTERSECTION;
        intersections[1][0] = intersections[1][1] = NO_INTERSECTION;
        return;
    }

    //solve the equation
    float sqrt_discriminant = sqrt(max(0.0, (double)discriminant));

    float t1 = (-b + sqrt_discriminant) / (2.0 * a);
    float t2 = (-b - sqrt_discriminant) / (2.0 * a);

    // compute intersection points back in original coordinates
    intersections[0][0] = shiftedPX + t1 * dx + mX;
    intersections[0][1] = shiftedPY + t1 * dy + mY;

    if (discriminant > 1e-6) {
        intersections[1][0] = shiftedPX + t2 * dx + mX;
        intersections[1][1] = shiftedPY + t2 * dy + mY;
    } else {
        intersections[1][0] = intersections[1][1] = NO_INTERSECTION;
    }
}

/*
    returns which of the two intersection points lies in the opposition direction from (pX, pY), ie "behind" the point
*/
int beetlebot::getOppositeIntersection (float pX, float pY, float direction, int intersections[2][2]) {

    //if the original direction is forward, gives the backward direction vector
    float oppositeDX = -cos(direction);
    float oppositeDY = -sin(direction);

    // gives a vector going from (pX, pY) to (0,0)
    float vector1X = intersections[0][0] - pX;
    float vector1Y = intersections[0][1] - pY;

    //dot product to project vector1 in the opposite direction
    float projection1 = vector1X * oppositeDX + vector1Y * oppositeDY;

    float vector2X = intersections[1][0] - pX;
    float vector2Y = intersections[1][1] - pY;
    float projection2 = vector2X * oppositeDX + vector2Y * oppositeDY;

    // returns 0 if intersection 0 is further back, or 1 
    return (projection1 > projection2) ? 0 : 1;
}

int beetlebot::getAction() {return action;}
float beetlebot::getOrientation() {return orientation;}
float beetlebot::getXPos() {return xPosition;}
float beetlebot::getYPos() {return yPosition;}

void beetlebot::setStartSystem (float x, float y, float orientation) {
    xPosition = x;
    yPosition = y;
    this->orientation = orientation;
}

void beetlebot::setOrientation (float orientation) {this->orientation = orientation;}