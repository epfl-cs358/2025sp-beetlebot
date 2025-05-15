#include "leg.h"

void leg::setAngle(int servoPin, int offset, float angle, int servoOnEsp) {
    //pins on the multiplexer
    angle = constrain(angle + offset, 0, 180);
    if (servoPin<16) {
        int pulse = angleToPulse(angle);
        multiplexer.setPWM(servoPin, 0, pulse);
    } else {
        uint32_t pulse = map(angle, 0, 180, 500, 2500);
        uint32_t duty = (uint32_t)(pulse * 65536.0 / 20000.0);
        ledcWrite(servoOnEsp, duty);
    }; 
}


/*
    x,y,z specify a point in space that we want the tip of the leg to go to:
        translate the position to servo angles for each of the 3 joints. 

    returns true if the movement was performed. 
*/
bool leg::moveTo (float x, float y, float z) {

    //for the coxa joint
    float distanceBodyPoint = sqrt(x*x + y*y);

    //flattens x to have a 2-coordinate plane
    float H = sqrt(z*z + (distanceBodyPoint - length[Coxa])*(distanceBodyPoint - length[Coxa]));

    float coxaRotDeg = atan2(y, x) * 180.0 / MYPI;
    coxaRotDeg = mapF(coxaRotDeg, 0.0, 180.0, 180.0, 0.0);

    if(!isReachable(H,z)) {
        return false;
    } else if (!((coxaRotDeg + offsetCoxa) > coxaLimits[0] && (coxaRotDeg + offsetCoxa) < coxaLimits[1])) {
        return false;
    }

    //IK 2D planar assuming leg is flat
    float femurRotDeg = (acos(z/H) + 
        acos((length[Tibia]*length[Tibia] - length[Femur]*length[Femur] - H*H) / (-2 * length[Femur]*H)) ) * 180.0 / MYPI;
    float tibiaRotDeg = (acos((H*H - length[Tibia]*length[Tibia] - length[Femur]*length[Femur]) / (-2 * length[Tibia]*length[Femur])))* 180.0 / MYPI;

    if (index % 2 == 1) { //on the rightside, motion needs to be mirrored
        femurRotDeg = mapF(femurRotDeg, 0.0, 180.0, 180.0, 0.0);
        tibiaRotDeg  = mapF(tibiaRotDeg,  0.0, 180.0, 180.0, 0.0);
    }

    setAngle(pinC, offsetCoxa, coxaRotDeg);
    setAngle(pinF, offsetFemur, femurRotDeg);

    //2 tibias are plugged to the esp
    setAngle(pinT, offsetTibia, tibiaRotDeg, index);

    curPos[0] = x;
    curPos[1] = y;
    curPos[2] = z;

    delay(SWEEP_DELAY); //if move-to is called on the same servos in a row
    return true;
}

/*
    checks that the position we want the tip of the leg to go to is reachable with our leg
    based on visual computation moving a singular leg, and plotting its movement on geogebra to get point limits.
*/
bool leg::isReachable (float H, float z) {
    z = -z; //it points down in the coordinates
    float lower = 0.01 * H * H - 0.83 * H - 39.28;
    float upper = 0.01 * H * H - 1.35 * H + 4.18;
    if (H<26 || H >106)  return false;
    else {
        if (lower <= z && z<= upper) {
            return true;
        }
    }
    return false;

}

float leg::getX() {return curPos[0];}
float leg::getY() {return curPos[1];}
float leg::getZ() {return curPos[2];}

int leg::angleToPulse (int pwmAngle) {

    return map(pwmAngle, 0, 180, 125, 625);
}

//maps x from domain in: [min, max] to the corresponding domain out: [min, max]
float mapF(float x, float in_min, float in_max, float out_min, float out_max) {
    return ((x - in_min) * (out_max - out_min) / (in_max - in_min)) + out_min;
}