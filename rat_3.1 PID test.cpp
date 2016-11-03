#include "mbed.h"
#include <cmath>

// motor, timer, serial port, LED
PwmOut MRF(PA_7);
PwmOut MRB(PB_6);
PwmOut MLF(PB_10);
PwmOut MLB(PC_7);
Timer t;
Serial pc(SERIAL_TX, SERIAL_RX);
DigitalOut myled(LED1);

//encoder
InterruptIn REncChannelA(PA_1); //right encoder
DigitalIn REncChannelB(PC_4);
InterruptIn LEncChannelA(PA_15); //left encoder
DigitalIn LEncChannelB(PB_3);
Ticker SecondUpdate;  //for updating speed every second

//Encoder function
void updateRightEncoder();
void updateLeftEncoder();
volatile unsigned long RCount = 0;
volatile unsigned long LCount = 0;

//deviation & fix
float encoderPID();
int prevError = 0;
int integralError = 0; //sum of all error
void updateSpeed();

float speed = 0.5;
int main() {
    REncChannelA.rise(&updateRightEncoder);
    LEncChannelA.rise(&updateLeftEncoder);
    SecondUpdate.attach(&updateSpeed,0.2);
    t.reset();
    
}
float elapsedTime=0; //erase in the future
void updateSpeed()
{
    MRF = speed + encoderPID();
    MLF = speed - encoderPID();
    //pc.printf("encoder value is %f \r\n", encoderPID());
    pc.printf("diff is: %d \r\n", LCount - RCount);
    //pc.printf("speedR is: %f \r\n", MRF.read());
    //pc.printf("speedL is: %f \r\n", MLF.read());
    pc.printf("elapsedTime: %f \r\n\n", elapsedTime);
    
    elapsedTime+=.2;
};


void updateRightEncoder()
{
    if(REncChannelB.read())
        RCount--;
    else RCount++;
}

void updateLeftEncoder()
{
    if(LEncChannelB.read()) //changed signs bc it would read opposite
        LCount++;
    else LCount--;
}

float encoderPID() //1 through 0.1 current if we want to stay at speed .1
{
    float kp = (1.0/24.0) * speed * .20; // w/o PID, the error is around 24 (unit error = 24),  so 20% change in speed every unit error
    float ki = (1.0/4.0) * speed * 0.008; //w/i ID unit error is 4, so 0.8% additional change every unit error
    float kd = speed * .002; // 0.2% change
    int error = LCount - RCount;
    integralError += error;
    float returnVal = kp*error + ki*integralError + kd*(error - prevError);
    
    //pc.printf("kp*error is: %f \r\n", kp*error);   //debugging process
    //pc.printf("kp*integral error is: %f \r\n", ki*integralError);
    //pc.printf("kp*deriv error is: %f \r\n", kd*(error - prevError));
    //pc.printf("returnVal: %f \r\n", returnVal);
    //pc.printf("\n");
    
    prevError = error;
    return returnVal;
    
}

