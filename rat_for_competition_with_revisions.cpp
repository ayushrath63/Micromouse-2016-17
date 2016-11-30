#include "mbed.h"
#include <cmath>
//IR Emitter
DigitalOut IR_FL(PB_0);  DigitalOut IR_FR(PC_11); DigitalOut IR_L(PB_7); DigitalOut IR_R(PC_10);
//IR_Reader/Receiver
AnalogIn IRR_FR(PA_4); AnalogIn IRR_FL(PC_1); AnalogIn IRR_R(PA_0); AnalogIn IRR_L(PC_0);
float PID(float itemA, float itemB, float kp, float ki, float kd); //returns discrepancy changer
void resetPID();
//Motors
PwmOut MRF(PA_7); PwmOut MRB(PB_6); PwmOut MLF(PB_10); PwmOut MLB(PC_7);

//encoder
InterruptIn REncChannelA(PA_1); //right encoder
DigitalIn REncChannelB(PC_4);
InterruptIn LEncChannelA(PA_15); //left encoder
DigitalIn LEncChannelB(PB_3);

//Encoder function decl
void updateRightEncoder();
void updateLeftEncoder();
volatile unsigned long RCount = 0;
volatile unsigned long LCount = 0;

DigitalOut myled(LED1);
Ticker print_IR_Ticker;
Ticker IR_Ticker;
Serial pc(SERIAL_TX,SERIAL_RX);
Timer t; //global

//IR function

float IR_array[4]; //{L,FL,FR,R}
float IR_array_past[4]; //{L,FL,FR,R}
void IR_Readings(float IRarray[]); //{L,FL,FR,R} [(1-x)*1000] takes .1 second to blink&read
void printIR_array();

//movement function
void turnLeft(int duration, double speed);
void turnRight(int duration, double speed);
void forward(int duration, double speed);
void backward(int duration, double speed);
void brake();
void calibrateFront();

//Global Variable
float speed = 0.35;
float tspeed = 0.2;
const int IRF_THRESHOLD = 85;
const int IRF_THRESHOLD2= 45;
bool calibrated = false;
float calibrate_time = 1; //2 seconds

int main() {
    //print IR, count encoder Interrupt
    print_IR_Ticker.attach(&printIR_array,1);
    REncChannelA.rise(&updateRightEncoder);
    LEncChannelA.rise(&updateLeftEncoder);
    
    //
    wait(2); //wait 2 second before startiing
    t.start();
    calibrated = false;
    while(1)
    {
        IR_Readings(IR_array); //read array        
        if (IR_array[1] < IRF_THRESHOLD && IR_array[2] < IRF_THRESHOLD) //if it sees wall
        {
            myled = 1; wait(0.1); myled = 0;
            brake();
            calibrateFront();
            //backward(1, 0.05);
            brake();
            
            wait(1);
            myled = 1; wait(0.1); myled = 0;
            if (IR_array[0] < IR_array[3])
                turnRight(140,tspeed); //90degrees
            else
                turnLeft(140,tspeed);
            calibrated = true; //work from here
            MRF=0;MLF=0;MRB=0;MLB=0;
            wait(1);
            forward(60,tspeed);
            
        }
        
        //forward relying on both wall
        else if (!calibrated) //2second calibration time
        {
            
            MRF = speed + PID(IR_array[0],IR_array[3],0.2,0.008,0.002);
            MLF = speed - PID(IR_array[0],IR_array[3],0.2,0.008,0.002);
            
            if(t.read() > calibrate_time)
            {
                calibrated = true;
                IR_array_past[0] = IR_array[0];
                IR_array_past[3] = IR_array[3]; //save the calibrated IR values
                resetPID(); //PID function will be used by different entitiy now gotta reset it once doen using
            }
        }
        //forward relying on left wall
        else
        {
            float LeftIRDifference = IR_array[0]-IR_array_past[0];
            float RightIRDifference = IR_array[3]-IR_array_past[3];
            if (abs(RightIRDifference) > 20)//if the distance is bigger than 2nd threshold contine
            {
                MLF = speed + PID(IR_array[3],IR_array_past[3],0.2,0.008,0.002);
                MRF = speed - PID(IR_array[3],IR_array_past[3],0.2,0.008,0.002); //HERE
            }
            else if (abs(LeftIRDifference) > 20)//if the distance is bigger than 2nd threshold contine
            {
                MRF = speed + PID(IR_array[0],IR_array_past[0],0.2,0.008,0.002);
                MLF = speed - PID(IR_array[0],IR_array_past[0],0.2,0.008,0.002); //HERE
            }
            else
            {
                MRF = speed;
                MLF = speed;
            }
        }
    }
}

float integralError = 0;
float prevError = 0;
void resetPID()
{
    integralError = 0;
    prevError = 0;
}
float PID(float itemA, float itemB, float kp, float ki, float kd) //1 through 0.1 current if we want to stay at speed .1
{
    kp *= speed; // 20% change in speed every unit error
    ki *= speed; // change every unit error
    kd *= speed; // 0.2% change
    float error = itemA - itemB; // ir sensor value (it's so small I emphlified it
    
    //pc.printf("error: %f \r\n\n", error);
    integralError += error;
    if (error ==0)
        integralError = 0;
    float returnVal = kp*error + ki*integralError + kd*(error - prevError);
    prevError = error;
    return returnVal;
}


void IR_Readings(float IRarray[]) //{L,FL,FR,R}
{
    IR_FL = 1; IR_FR = 1; IR_R = 1; IR_L = 1;
    wait(.02);
    IRarray[0] = (1-IRR_L.read())*100; IRarray[1] = (1-IRR_FL.read())*100; IRarray[2] = (1-IRR_FR.read())*100; IRarray[3] = (1-IRR_R.read())*100;
    //    IRarray[0] = IRR_L.read(); IRarray[1] = IRR_FL.read(); IRarray[2] = IRR_FR.read(); IRarray[3] = IRR_R.read();
    IR_FL = 0; IR_FR = 0; IR_R = 0; IR_L = 0;
}


void printIR_array()
{
    pc.printf("distance %f %f %f %f \r\n", IR_array[0],IR_array[1],IR_array[2],IR_array[3]);
    pc.printf("Rspeed, Lspeed: %f %f \r\n\n", MRF.read(), MLF.read());
}

//motor function
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

void turnRight(int duration, double speed)
{
    RCount= 0; LCount = 0;
    MLF = speed; MRB = speed; //set speed
    while (LCount != duration);
    MRB = 0; MLF = 0; //set speed back to 0
}

void turnLeft(int duration, double speed)
{
    RCount= 0; LCount = 0;
    MRF = speed; MLB = speed; //set speed
    while (RCount != duration);
    MRF = 0; MLB = 0; //set speed back to 0
}


void forward(int duration, double speed)
{
    RCount= 0; LCount = 0;
    MRF = speed; MLF = speed; //set speed
    while (RCount != duration);
    MRF = 0; MLF = 0; //set speed back to 0
}

void backward(int duration, double speed)
{
    RCount= 0; LCount = 0;
    MRB = speed; MLB = speed; //set speed
    while (RCount != -1*duration);
    MRB = 0; MLB = 0; //set speed back to 0
}

void brake()
{
    MRB = 1; MRF = 0; MLB = 0; MLF = 0;
    wait(0.1);
    MRB = 0; MRF = 0; MLB = 0; MLF = 0;
}

void calibrateFront() //this needs some work
{
    float speed=0.05;
    int threshold1 = 4;
    while(abs(IR_array[1] - IR_array[2]) > threshold1)
    {
        IR_Readings(IR_array);
        if(IR_array[1]>IR_array[2])
        {
            MRF = 0;
            MLF = speed;
            MRB = speed;
            MLB = 0;
        }
        else
        {
            MRF = speed;
            MLF = 0;
            MRB = 0;
            MLB = speed;
        }
    }
    MRF = 0; MLF = 0; MRB = 0; MLB = 0; //set speed back to 0
}

