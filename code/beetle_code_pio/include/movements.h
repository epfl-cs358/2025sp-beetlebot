#include "data.h"
#include "leg.h"


class movements {
    public: 

        //simple forward walk
        void forward();

        //necessary for smoother movements : interpolate the angles to make sure each leg moves together
        void interpolateAngle(leg body [6], int finalAngles [6][3], int stepNumber);
        void initializeAllServos(float angleC, float angleF, float angleT);

        //helper for forward - tripod gait
        void angleTab (int angles[6][3], int walk, int group2 [4][3], int group1 [4][3]);

        movements(leg lf, leg rf, leg lm, leg rm, leg lb, leg rb, Adafruit_PWMServoDriver &multiplexer): 
        multiplexer(multiplexer), lf(lf), rf(rf), lm(lm), rm(rm), lb(lb), rb(rb){} 
        
        int currAngles [6][3] = {
        {90, 90, 90},
        {90, 90, 90},
        {90, 90, 90},
        {90, 90, 90},
        {90, 90, 90},
        {90, 90, 90}};

        
        leg legs[6] = {lf, rf, lm, rm, lb, rb};
        leg* current = &rm;
        
    
    private:
        Adafruit_PWMServoDriver &multiplexer;

        leg lf;
        leg rf;
        leg lm;
        leg rm;
        leg lb;
        leg rb;

        
};