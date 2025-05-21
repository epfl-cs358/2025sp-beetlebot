#include "data.h"
#include "leg.h"


class movements {
    public: 

        //simple walk
        void forward();
        void rotation(int direction);
        void sideways(int direction);

        void sitUp();

        //necessary for smoother movements : interpolate the angles to make sure each leg moves together
        void interpolateAngle(leg* body[6], int finalAngles[6][3], int stepNumber);
        void initializeAllServos(float angleC, float angleF, float angleT);

        //forward angle swap - tripod gait
        void angleTab2 (int angles[6][3], int walk, int group2 [4][3], int group1 [4][3]);
        void angleTab4(int angles[6][3], int walk, int group1 [4][3], int group2[4][3], 
            int middleGroup1 [4][3], int middleGroup2 [4][3]);

        movements(leg lf, leg rf, leg lm, leg rm, leg lb, leg rb, Adafruit_PWMServoDriver &multiplexer, int stepCounter)
        : multiplexer(multiplexer), lf(lf), rf(rf), lm(lm), rm(rm), lb(lb), rb(rb), stepCounter(stepCounter)
        {
            legs[0] = &this->lf;
            legs[1] = &this->rf;
            legs[2] = &this->lm;
            legs[3] = &this->rm;
            legs[4] = &this->lb;
            legs[5] = &this->rb;
            current = &this->rm;
        }
        
        int currAngles [6][3] = {
        {90, 90, 90},
        {90, 90, 90},
        {90, 90, 90},
        {90, 90, 90},
        {90, 90, 90},
        {90, 90, 90}};

        int stepCounter;

        
        leg* legs[6];
        leg* current;
        
    
    private:
        Adafruit_PWMServoDriver &multiplexer;

        leg lf;
        leg rf;
        leg lm;
        leg rm;
        leg lb;
        leg rb;

        
};