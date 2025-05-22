#include "main.h"
#include <Adafruit_PWMServoDriver.h>
#include <ESP32Servo.h>
#include <WebSerial.h>              // ESPAsyncWebServer helper
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

/* ───────────────── Hardware ───────────────── */
Adafruit_PWMServoDriver pca(0x40);
Servo espServos[2];                 // channels 16+17
const int espPins[2] = { 23, 18 };  // keep same order as in servoPins table

/* micro-seconds range */
constexpr int usMin = 500, usMax = 2500;
constexpr int tickMin = int(usMin / 4.88);   // PCA 12-bit tick @60 Hz
constexpr int tickMax = int(usMax / 4.88);

/* hexapod instance */
hexapod bot(false);                 // debug-off

/* util: pwm write */
static inline int usToTicks(int us){ return map(us, usMin, usMax, tickMin, tickMax); }
static inline int angleToUS(float deg){ return usMin + (usMax-usMin)*deg/300.0; }

/* servo index 0-17 → pin / channel lookup ---------------------------- */
static int pinLUT[18];      // pin number
static bool isPCA[18];      // true = PCA, false = ESP

static void buildLookup()
{
    int idx = 0;
    for(int leg=0; leg<6; ++leg)
        for(int jnt=0; jnt<3; ++jnt, ++idx)
        {
            pinLUT[idx] = servoPins[leg][jnt];
            isPCA[idx]  = (pinLUT[idx] <= 15);
        }
}


/* ───────────────── Wi-Fi / Serial setup ───────────────── */
AsyncWebServer server(80);

void setup()
{
    Serial.begin(115200);
    buildLookup();


    /* PCA9685 */
    pca.begin();
    pca.setPWMFreq(60);

    /* two direct ESP32 channels */
    espServos[0].setPeriodHertz(50);
    espServos[0].attach(espPins[0], usMin, usMax);
    espServos[1].setPeriodHertz(50);
    espServos[1].attach(espPins[1], usMin, usMax);

    bot.stand();                     // nice pose at boot
}

/* ───────────────── Main loop ───────────────── */
const float dt = 0.02f;              // 50 Hz tick
unsigned long lastTick = 0;

void loop()
{
    unsigned long now = millis();
    if(now - lastTick >= dt*1000){
        lastTick += dt*1000;
        if(fabs(bot.speed) > 0.001) bot.step(dt);
    }
}

/* ───────────────── Input handlers ───────────────── */

void handleSerialInput()
{
    if(Serial.available()){
        String s = Serial.readStringUntil('\n');
        s.trim();  handleTextCommand(s);
    }
}

/* ───────────────── Command parser ───────────────── */
void handleTextCommand(String in)
{
    in.toLowerCase();
    if(in == "stand"){ bot.speed=0; bot.stand();  
    Serial.println("Standing"); }
    else if(in == "sit"){ bot.speed=0; bot.sit();
    Serial.println("Sitting"); }
    else if(in.startsWith("walk")){
        float spd = in.substring(4).toFloat(); if(spd==0) spd=0.6;
        bot.speed = constrain(spd,-1.f,1.f);
        Serial.println("Walking "+String(bot.speed));
    }
    else if(in=="stop"){ bot.speed=0; 
    Serial.println("Stopped"); }
    else Serial.println("Commands: stand  sit  walk <spd>  stop");
}

void applyServos()
{
    extern float servoangle[18];      // OK – linker pulls the one in hexapod
    for (int i = 0; i < 18; ++i)
    {
        float deg = constrain(servoangle[i], 0.f, 300.f);
        if (isPCA[i]) {
            int ticks = usToTicks(angleToUS(deg));
            pca.setPWM(pinLUT[i], 0, ticks);
        } else {
            int espIdx = (pinLUT[i] == espPins[0]) ? 0 : 1;
            espServos[espIdx].write(deg);
        }
    }
}
