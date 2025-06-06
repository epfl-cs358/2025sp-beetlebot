#ifndef leg_H
#define leg_H

#include <Adafruit_PWMServoDriver.h>
#include "data.h"


enum servoIndex {
    Coxa = 0, Femur = 1, Tibia = 2
};

//math version of map with floats, check constraints
float mapF(float x, float in_min, float in_max, float out_min, float out_max);

//for a whole leg (3 sets of servos)
class leg {
    public: 

        void setAngle (int servoPin, float angle, int onEsp = 2); //0 - left tibia, 1 - right tibia, 2 : not on esp


        int coxaLimits[2] = {65, 110};
        //coxa, femur, tibia in mm
        const float length[3] = {36.3, 29.9, 56.7};        

        leg(Adafruit_PWMServoDriver &multiplexer, int offsetCoxa, int offsetFemur, int offsetTibia, String name, int index, 
            int pinC, int pinF, int pinT) 
        : multiplexer(multiplexer), offsetCoxa(offsetCoxa), offsetFemur(offsetFemur), offsetTibia(offsetTibia), 
        name(name), index(index), pinC(pinC), pinF(pinF), pinT(pinT)
        {} 
        
        const int pinC;
        const int pinF;
        const int pinT;

        String name; //lf, rf...
        int index; //0,1...5
    
    private:
        Adafruit_PWMServoDriver &multiplexer;

        const int offsetCoxa;
        const int offsetFemur;
        const int offsetTibia;
        int angleToPulse(int pwmAngle);

        
};
#endif