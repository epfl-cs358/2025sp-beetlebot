#include "beetlebot.h"

Adafruit_PWMServoDriver multiplex = Adafruit_PWMServoDriver(0x40);

void beetlebot::smoothStep(float newPositions[6][3], float prevPositions[6][3], float finalPositions[6][3], int stepHeight, int stepCounter, int stepNumber, bool moveRightGroup, bool moveAllLegs) {
    if (stepCounter > stepNumber) {
        stepCounter = stepNumber;
    }

    int liftedLegs[3] = {1,2,5};
    int groundedLegs[3] = {1,2,5};

    if (!moveRightGroup) {
        liftedLegs[0] = 0;
        liftedLegs[1] = 3;
        liftedLegs[2] = 4;

        groundedLegs[0] = 1;
        groundedLegs[1] = 2;
        groundedLegs[2] = 5;
    }

    if (moveAllLegs) {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                newPositions[groundedLegs[i]][j] = mapF(stepCounter, 0.0, stepNumber * 1.0, prevPositions[groundedLegs[i]][j], finalPositions[groundedLegs[i]][j]);
            }
        }
    }

    int startIter = ceil(stepNumber * 0.2);    
    int targetIter = floor(stepNumber * 0.75); //when the target is reached
    if (stepCounter >= startIter && stepCounter <= targetIter) {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 2; ++j) {
                newPositions[liftedLegs[i]][j] = mapF(stepCounter, startIter * 1.0, targetIter * 1.0, prevPositions[liftedLegs[i]][j], finalPositions[liftedLegs[i]][j]);
            }
        }

    } else if (stepCounter < startIter) {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 2; ++j) {
                newPositions[liftedLegs[i]][j] = prevPositions[liftedLegs[i]][j];
            }
        }
    } else {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 2; ++j) {
                newPositions[liftedLegs[i]][j] = finalPositions[liftedLegs[i]][j];
            }
        }
    }

    // raise and lower the legs based on stepCounter. 
    int upSteps = ceil(stepNumber * 0.15);     // legs are lifted in the first 15% of the step
    int downSteps = floor(stepNumber * 0.85);  // legs are lowered in the last 15% og the step
    if (stepCounter < upSteps) {
        //lift the legs
        for (int i = 0; i < 3; ++i) {
            newPositions[liftedLegs[i]][2] = mapF(stepCounter, 0.0, upSteps * 1.0, prevPositions[liftedLegs[i]][2], finalPositions[liftedLegs[i]][2] - stepHeight);
        }
    } else if (stepCounter > downSteps) {
        //lower the legs
        for (int i = 0; i < 3; ++i) {
            newPositions[liftedLegs[i]][2] = mapF(stepCounter, downSteps * 1.0, stepNumber * 1.0, prevPositions[liftedLegs[i]][2] - stepHeight, finalPositions[liftedLegs[i]][2]);
        }
    } else {
        // keep the legs lifted
        for (int i = 0; i < 3; ++i) {
            newPositions[liftedLegs[i]][2] = prevPositions[liftedLegs[i]][2] - stepHeight;
        }
    }
}

void beetlebot::travelPath(float legPos[6][3], int pathPoints [][2], int numPoints, int iter, int stepHeight) {
    for (int i = 0; i < numPoints; ++i) {

        float distToGoal = (this->getXPos()-pathPoints[i][0])*(this->getXPos()-pathPoints[i][0]) + (this->getYPos()-pathPoints[i][1]) * (this->getYPos()-pathPoints[i][1]);
        distToGoal = sqrt(distToGoal);

        while (distToGoal> accuracy) {
            float goalDirection = atan2(pathPoints[i][1] - this->getYPos(), pathPoints[i][0] - this->getXPos());

            float directionDiff = goalDirection - this->getOrientation();
            if (directionDiff > MYPI) {
                directionDiff -= 2 * MYPI;
            } else if (directionDiff < -MYPI) {
                directionDiff += 2 * MYPI;
            }

            uint8_t radius = min((int) distToGoal / 2, 20);

            // execute the step
            this->step(legPos, directionDiff, radius, iter, stepHeight);

            // recompute the distance to the goal position
            distToGoal = (this->getXPos()-pathPoints[i][0])*(this->getXPos()-pathPoints[i][0]) + (this->getYPos()-pathPoints[i][1]) * (this->getYPos()-pathPoints[i][1]);
            distToGoal = sqrt(distToGoal);
        }
    }
}

void beetlebot::step(float legPos[6][3], float stepDirection, int radius, int stepNumber, int stepHeight) {
    // array to save the leg positions for each iteration
  float posArray[6][3];

  do {
    unsigned long startTime = millis();
    this->computeStep(legPos, posArray, stepDirection, radius, stepNumber, stepHeight);
    this->moveLegs(posArray);
    while (millis() < startTime + periodMs) {}
  } while (this->getAction() != 0);
}

bool beetlebot::computeStep(float prevPositions[6][3], float newPositions[6][3], float stepDirection, int radius, int stepNumber, int stepHeight) {
   if (stepCounter ==1) {
        action = 1;
        int liftedLegs[3] = {0, 3, 4};
        moveRightGroup = false;

        int groundedLegs[3] = {1,2,5};

        //direction based swap (previous step)
        if ((prevRightGroup == false && abs(stepDirection - prevDirection) <= MYPI / 2) ||
            (prevRightGroup == true && abs(stepDirection - prevDirection) > MYPI/2)) {
                liftedLegs[0] = 1;
                liftedLegs[1] = 2;
                liftedLegs[2] = 5;

                groundedLegs[0] = 0;
                groundedLegs[1] = 3;
                groundedLegs[2] = 4;
                moveRightGroup = true;
            }
        
        //compute the end positions of the lifted legs
        for (int i = 0; i < 3; ++i) {
            // x value
            finalPositions[liftedLegs[i]][0] = homePositions[liftedLegs[i]][0] + radius * cos(stepDirection + legCoords[liftedLegs[i]][2]) * 1.0;
            // y value
            finalPositions[liftedLegs[i]][1] = homePositions[liftedLegs[i]][1] + radius * sin(stepDirection + legCoords[liftedLegs[i]][2]) * 1.0;
        }
      
        // Compute final positions for stationary legs
        for (uint8_t i = 0; i < 3; ++i) {
            int intersections[2][2] = {{0}};
            circleLineIntersect(
                homePositions[groundedLegs[i]][0], homePositions[groundedLegs[i]][1], radius,
                prevPositions[groundedLegs[i]][0],
                prevPositions[groundedLegs[i]][1],
                stepDirection + legCoords[groundedLegs[i]][2],
                intersections);
    
            int index = getOppositeIntersection(
                prevPositions[groundedLegs[i]][0],
                prevPositions[groundedLegs[i]][1],
                stepDirection + legCoords[groundedLegs[i]][2],
                intersections);
    
            if (intersections[index][0] != -1000 && intersections[index][1] != -1000) {
                finalPositions[groundedLegs[i]][0] = intersections[index][0];
                finalPositions[groundedLegs[i]][1] = intersections[index][1];
            } else { //no intersection with the moving legs
                finalPositions[groundedLegs[i]][0] = prevPositions[groundedLegs[i]][0];
                finalPositions[groundedLegs[i]][1] = prevPositions[groundedLegs[i]][1];
            }
        }

        // Normalize step lengths inside the circle

        float stepLengths[3] = {0.0};
        float avgLength = 0.0;
        for (uint8_t i = 0; i < 3; ++i) {
            float dx = prevPositions[groundedLegs[i]][0] - finalPositions[groundedLegs[i]][0];
            float dy = prevPositions[groundedLegs[i]][1] - finalPositions[groundedLegs[i]][1];
            stepLengths[i] = dx * dx + dy * dy;
            avgLength += stepLengths[i];
        }
        avgLength /= 3.0;

        // get the shortest distance
        float minLength = stepLengths[0];
        for (uint8_t i = 1; i < 3; ++i) {
        if (stepLengths[i] < minLength) {
            minLength = stepLengths[i];
        }
        }

        // Check whether one leg moves a significantly shorter distance
        const float tolerance = 0.1;  // 10%
        bool adjustSteps = false;
        for (uint8_t i = 0; i < 3; ++i) {
        if (abs(stepLengths[i] - avgLength) > avgLength * tolerance) {
            adjustSteps = true;
            break;
            }
        }

        if (adjustSteps == true) {
        // Adjust finalPositions, based on the shortest step length
        for (uint8_t i = 0; i < 3; ++i) {
            // get directional vector
            float dx = finalPositions[groundedLegs[i]][0] - prevPositions[groundedLegs[i]][0];
            float dy = finalPositions[groundedLegs[i]][1] - prevPositions[groundedLegs[i]][1];

            if (dx != 0.0 || dy != 0.0) {  // avoid division by zero
                // scale factor based on minLength
                float scale = sqrt(max((double) minLength / (dx * dx + dy * dy), 0.0));

                // calc new final positions based on minLength
                finalPositions[groundedLegs[i]][0] = prevPositions[groundedLegs[i]][0] + dx * scale;
                finalPositions[groundedLegs[i]][1] = prevPositions[groundedLegs[i]][1] + dy * scale;
            }
        }

        // Add the current step to the global position
        xPosition += sqrt(minLength) * cos(stepDirection + orientation) * lengthAccuracy;
        yPosition += sqrt(minLength) * sin(stepDirection + orientation) * lengthAccuracy;
        } else {
            xPosition += sqrt(avgLength) * cos(stepDirection + orientation) * lengthAccuracy;
            yPosition += sqrt(avgLength) * sin(stepDirection + orientation) * lengthAccuracy;
        }
    }

    //move the legs to the mapped position
    if (stepCounter > 0 && stepCounter <= stepNumber) {
          // interpolate the current leg position. Using the map() function will result in a linear motion to the final position
          smoothStep(newPositions, prevPositions, finalPositions, stepHeight, stepCounter, stepNumber, moveRightGroup);
    }

     //max iter is reached
    if (stepCounter == stepNumber) {
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
        stepCounter = 1;
        action = 0;
        return true;
    } 
    stepCounter++;
    return false;
}

void beetlebot::computeBodyMov(float prevPositions[6][3], float newPositions[6][3], float xTrans, 
    float yTrans, float zTrans, uint8_t legMask) {
    
    //copy 
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

            // Rotate back
            temp = newPositions[i][0];
            newPositions[i][0] = newPositions[i][0] * cos(legCoords[i][2]) - newPositions[i][1] * sin(legCoords[i][2]);
            newPositions[i][1] = temp * sin(legCoords[i][2]) + newPositions[i][1] * cos(legCoords[i][2]);
        }
    }
}

bool beetlebot::moveHome() {
    bool moved = legs[0]->moveTo(homePositions[0][0], homePositions[0][1], homePositions[0][2])
    && legs[1]->moveTo(homePositions[1][0], homePositions[1][1], homePositions[1][2])
    && legs[2]->moveTo(homePositions[2][0], homePositions[2][1], homePositions[2][2])
    && legs[3]->moveTo(homePositions[3][0], homePositions[3][1], homePositions[3][2])
    && legs[4]->moveTo(homePositions[4][0], homePositions[4][1], homePositions[4][2])
    && legs[5]->moveTo(homePositions[5][0], homePositions[5][1], homePositions[5][2]);

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

void beetlebot::circleLineIntersect (float mX, float mY, int radius, float pX, float pY, float direction, int intersections[2][2]) {
    // Shift the point (pX, pY) to the origin of the coordinate frame
    float shiftedPX = pX - mX;
    float shiftedPY = pY - mY;

    // Split the direction in x and y components.
    float dx = cos(direction);
    float dy = sin(direction);

    // coefficents for qudratic formula
    float a = dx * dx + dy * dy;
    float b = 2 * (dx * shiftedPX + dy * shiftedPY);
    float c = shiftedPX * shiftedPX + shiftedPY * shiftedPY - radius * radius;

    float discriminant = b * b - 4 * a * c;

    if (discriminant < -1e-6) {
        // if there are no coefficients, set intersections to -1000
        intersections[0][0] = intersections[0][1] = -1000;
        intersections[1][0] = intersections[1][1] = -1000;
        return;
    }
    // approximate the sqrt
    float sqrt_discriminant = sqrt(max(0.0, (double)discriminant));

    // calculate the quadratic formula
    float t1 = (-b + sqrt_discriminant) / (2.0 * a);
    float t2 = (-b - sqrt_discriminant) / (2.0 * a);

    // calculate and shift the intersections back
    intersections[0][0] = shiftedPX + t1 * dx + mX;
    intersections[0][1] = shiftedPY + t1 * dy + mY;

    // calculate the second intersection if there is one
    if (discriminant > 1e-6) {
        intersections[1][0] = shiftedPX + t2 * dx + mX;
        intersections[1][1] = shiftedPY + t2 * dy + mY;
    } else {
        intersections[1][0] = intersections[1][1] = -1000;
    }
}

int beetlebot::getOppositeIntersection (float pX, float pY, float direction, int intersections[2][2]) {

    // reverse direction
    float oppositeDX = -cos(direction);
    float oppositeDY = -sin(direction);

    // project the vector from (pX, pY) to the first intersection onto the reverse direction
    float vector1X = intersections[0][0] - pX;
    float vector1Y = intersections[0][1] - pY;
    float projection1 = vector1X * oppositeDX + vector1Y * oppositeDY;

    // project the vector from (pX, pY) to the second intersectiononto the reverse direction
    float vector2X = intersections[1][0] - pX;
    float vector2Y = intersections[1][1] - pY;
    float projection2 = vector2X * oppositeDX + vector2Y * oppositeDY;

    // choose the point whose projection onto the direction is larger
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