#include "mbed.h"
#include <cmath>

// motor, timer, serial port
PwmOut MRF(PA_7);
PwmOut MRB(PB_6);
PwmOut MLF(PB_10);
PwmOut MLB(PC_7);
Timer t;
Serial pc(SERIAL_TX, SERIAL_RX);

InterruptIn REncChannelA(PA_1); //right encoder
DigitalIn REncChannelB(PC_4);
InterruptIn LEncChannelA(PA_15); //left encoder
DigitalIn LEncChannelB(PB_3);
//PA_15, PB_3 < left encoder (they are broken)
DigitalOut myled(LED1);

void updateRightEncoder();
void updateLeftEncoder();
volatile unsigned long RCount = 0;
volatile unsigned long LCount = 0;

void forward(int duration, double speed); //
void backward(int duration, double speed);
void turnLeft(int duration, double speed);
void turnRight(int duration, double speed);
void brake();
float encoderPID(); //deviation
int prevError = 0;
int integralError = 0;



int main() {
    t.reset();
    REncChannelA.rise(&updateRightEncoder);
    //LEncChannelA.rise(&updateLeftEncoder);
    
    
    backward(-300,10);
    brake();
    pc.printf("Pulses is: %d \r\n", RCount);
    
    forward(1200,10); //3 second and 20% speed
    brake();
    pc.printf("Pulses is: %d \r\n", RCount);
    backward(-900,10);
    brake();
    pc.printf("Pulses is: %d \r\n", RCount);
}

void forward(int duration, double speed)
{
    RCount= 0;
    speed = 0.01 * speed;
    MRF = speed; MLF = speed; //set speed
    while (RCount != duration);
    MRF = 0; MLF = 0; //set speed back to 0
}
void backward(int duration, double speed)
{
    RCount= 0;
    speed = 0.01 * speed;
    MRB = speed; MLB = speed; //set speed
    while (RCount != duration);
    MRB = 0; MLB = 0; //set speed back to 0
}


void turnRight(int duration, double speed)
{
    RCount= 0;
    speed = 0.01 * speed;
    //   MRB.period(0.1f);
    //   MLF.period(0.1f);
    MRB = speed; MLF = speed; //set speed
    while (RCount != duration);
    MRB = 0; MLF = 0; //set speed back to 0
}

void turnLeft(int duration, double speed)
{
    RCount= 0;
    speed = 0.01 * speed;
    //   MRF.period(0.1f);
    //   MLB.period(0.1f);
    MRF = speed; MLB = speed; //set speed
    while (RCount != duration);
    MRF = 0; MLB = 0; //set speed back to 0
}


void brake()
{
    MRF = 1; MLF = 1; MRB = 1; MLB = 1;
    wait_ms(500);
    MRF = 0; MLF = 0; MRB = 0; MLB = 0;
}

void updateRightEncoder()
{
    if(REncChannelB.read())
        RCount--;
    else RCount++;
}

void updateLeftEncoder()
{
    if(LEncChannelB.read())
        LCount--;
    else LCount++;
}

float encoderPID()
{
    float kp = 1.0;
    float ki = 1.0;
    float kd = 1.0;
    int error = LCount - RCount;
    integralError += error;
    float returnVal = kp*error + ki*integralError + kd*(error - prevError);
    prevError = error;
    return returnVal;
}

