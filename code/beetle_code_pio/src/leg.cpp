#include "leg.h"

/*
    Every computation is done in the coordinate frame of one leg, the coxa follows the y axis,
    x is perpendicular to this axis, and z-axis goes down
    We calculate the joint angles of the three servos of each leg, so that the end point
    of the leg reaches the desired point (the end point is where the tip of the leg should be at).
    X+ coordinate means the leg will move forward, while Y+ coordinate means the
    leg will extend. 
*/

void leg::setAngle(int servoPin, int offset, float angle, int servoOnEsp) {
    //pins on the multiplexer
    int pulse = angleToPulse(offset + angle);
    if (servoPin>=0 && servoPin<=15) {
        multiplexer.setPWM(servoPin, 0, pulse);
    } else {
        uint32_t duty = (uint32_t)(pulse * 65536.0 / 20000.0);
        ledcWrite(servoOnEsp, duty);
    };  
}

//a bit of inverse kinematics:
/*
    Moves the leg to the given (x,y) coordinates in mm
    Returns true if the leg moved to the point, else false
*/
bool leg::moveTo (float x, float y, float z) {
    bool moved = true;

    float distanceBodyPoint = sqrt(x*x + y*y);
    float H = sqrt(z*z + (distanceBodyPoint - length[Coxa])*(distanceBodyPoint - length[Coxa]));

    if(!isReachable(H,z)) return false;

    float coxaRotDeg = atan2(y, x) * 180.0 / MYPI;
    coxaRotDeg = mapF(coxaRotDeg, 0.0, 180.0, 180.0, 0.0);

    //IK 2D planar assuming leg is flat
    float femurRotDeg = (acos(z/H) + 
        acos((length[Tibia]*length[Tibia] - length[Femur]*length[Femur] - H*H) / (-2 * length[Femur]*H)) ) * 180.0 / MYPI;
    float tibiaRotDeg = (acos((H*H - length[Tibia]*length[Tibia] - length[Femur]*length[Femur]) / (-2 * length[Tibia]*length[Femur])))* 180.0 / MYPI;

    if (index % 2 == 0) { //on the rightside, computation needs to be mirrored (axis center is the robot center)
        femurRotDeg = mapF(femurRotDeg, 0.0, 180.0, 180.0, 0.0);
        tibiaRotDeg  = mapF(tibiaRotDeg,  0.0, 180.0, 180.0, 0.0);
    }

    //check coxa bounds - isReachable checks tibia and femur
    if ((coxaRotDeg + offsetCoxa) > coxaLimits[0] && (coxaRotDeg + offsetCoxa) < coxaLimits[1]) {
        multiplexer.setPWM(pinC, 0, angleToPulse(coxaRotDeg + offsetCoxa));
        delay(SWEEP_DELAY);
    } else {
        moved = false;
    }

    multiplexer.setPWM(pinF, 0, angleToPulse(femurRotDeg + offsetFemur));
    delay(SWEEP_DELAY);
    //2 tibias are plugged to the esp
    int pulse = angleToPulse(tibiaRotDeg + offsetTibia);
    if (pinT>=0 && pinT<=15) {
        multiplexer.setPWM(pinT, 0, pulse);
    } else {
        int channel = (index % 2 == 1) ? 0 : 1;
        uint32_t duty = (uint32_t)(pulse * 65536.0 / 20000.0);
        ledcWrite(channel, duty);
    }
    delay(SWEEP_DELAY);

    curPos[0] = x;
    curPos[1] = y;
    curPos[2] = z;

    return moved;
}

bool leg::isReachable (float H, float z) {
    z = -z; //points up

    //TODO: figure out those geometric constraints : mechanical, link lenghts
    

    return true;

}

float leg::getX() {return curPos[0];}
float leg::getY() {return curPos[1];}
float leg::getZ() {return curPos[2];}

int leg::angleToPulse (int pwmAngle) {
    pwmAngle = constrain(pwmAngle, 0, 180);

    return map(pwmAngle, 0, 180, 500, 2500);
}

float mapF(float x, float in_min, float in_max, float out_min, float out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}