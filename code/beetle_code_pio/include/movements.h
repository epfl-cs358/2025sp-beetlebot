#include "data.h"
#include "leg.h"

const int midStand [3] = {90, 115, 100};
class movements {
    public: 
        //simple walk
        void forward();
        void backward();

        //turns
        void rotation(int way, int cycle = 5);
        void forwardCurve(int way, float turn, int cycle = 4);
        void backwardCurve (int way, float turn, int cycle = 4);

        //left or right translational movements
        void sideways(int direction);

        //setups
        void standUp();
        void sitDown();

        // fun little wave
        void wave();

        //necessary for smoother movements : interpolate the angles to make sure each leg moves together
        void interpolateAngle(leg* body[6], int finalAngles[6][3], int stepNumber);
        void initializeAllServos(float angleC, float angleF, float angleT);

        //forward angle swap - tripod gait
        void angleTab2 (int angles[6][3], int walk, int group1 [4][3], int group2 [4][3]);
        void angleTab4(int angles[6][3], int walk, int group1 [4][3], int group2[4][3], 
            int middleGroup1 [4][3], int middleGroup2 [4][3]);
        void angleTabCorner (int angles[6][3], int walk, int group1 [4][3], int group2 [4][3]);
        void angleTabSide(int angles[6][3], int walk, int group1 [8][3], int group2[8][3], 
            int middleGroup1 [8][3], int middleGroup2 [8][3]);
        void angleTabStand (int angles[6][3], int leg, int angleLift [3]);
        void angleTabWave (int angles[6][3], int walk, int waveG[6][3]);

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

// mix two 6×3 angle tables:  dst = (1-λ)*a + λ*b
inline void mixTables(int dst[6][3], const int a[6][3], const int b[6][3], float lambda);
