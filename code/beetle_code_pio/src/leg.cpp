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


int leg::angleToPulse (int pwmAngle) {

    return map(pwmAngle, 0, 180, 125, 625);
}

//maps x from domain in: [min, max] to the corresponding domain out: [min, max]
float mapF(float x, float in_min, float in_max, float out_min, float out_max) {
    return ((x - in_min) * (out_max - out_min) / (in_max - in_min)) + out_min;
}