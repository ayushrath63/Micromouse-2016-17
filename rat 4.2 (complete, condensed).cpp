#include "mbed.h"

//IR Emitter
DigitalOut IR_FL(PB_0);  DigitalOut IR_FR(PC_11); DigitalOut IR_L(PB_7); DigitalOut IR_R(PC_10);
//IR_Reader/Receiver
AnalogIn IRR_FR(PA_4); AnalogIn IRR_FL(PC_1); AnalogIn IRR_R(PA_0); AnalogIn IRR_L(PC_0);
//Motors
PwmOut MRF(PA_7); PwmOut MRB(PB_6); PwmOut MLF(PB_10); PwmOut MLB(PC_7);

DigitalOut myled(LED1);
Ticker IR_Ticker;
Serial pc(SERIAL_TX,SERIAL_RX);

float IR_array[4]; //{L,FL,FR,R}
void IR_Readings(float IRarray[]); //{L,FL,FR,R} [(1-x)*1000] takes .1 second to blink&read
void printIR_array();
float IRPID(float kp, float ki, float kd); //returns discrepancy changer



const int IRF_THRESHOLD = 3;
float speed = .2;
int main()
{
    IR_Ticker.attach(&printIR_array,1);
    
    while(1)
    {
        IR_Readings(IR_array);
        if (IR_array[1] < IRF_THRESHOLD || IR_array[2] < IRF_THRESHOLD)
        {
            MRF = 0; MLF = 0;
        }
        else
        {
            MRF = speed + IRPID(0.2,0.008,0.002);
            MLF = speed - IRPID(0.2,0.008,0.002);
        }
        
    }
}

void IR_Readings(float IRarray[]) //{L,FL,FR,R}
{
    IR_FL = 1; IR_FR = 1; IR_R = 1; IR_L = 1;
    wait(.02);
    IRarray[0] = (1-IRR_L.read())*1000; IRarray[1] = (1-IRR_FL.read())*1000; IRarray[2] = (1-IRR_FR.read())*1000; IRarray[3] = (1-IRR_R.read())*1000;
    IR_FL = 0; IR_FR = 0; IR_R = 0; IR_L = 0;
}


void printIR_array()
{
    pc.printf("distance %f %f %f %f \r\n", IR_array[0],IR_array[1],IR_array[2],IR_array[3]);
    pc.printf("Rspeed, Lspeed: %f %f \r\n\n", MRF.read(), MLF.read());
}


float integralError = 0;
float prevError = 0;
float IRPID(float kp, float ki, float kd) //1 through 0.1 current if we want to stay at speed .1
{
    kp *= speed; // 20% change in speed every unit error
    ki *= speed; // change every unit error
    kd *= speed; // 0.2% change
    float error = IR_array[0] - IR_array[3]; // ir sensor value (it's so small I emphlified it
    
    //pc.printf("error: %f \r\n\n", error);
    integralError += error;
    if (error ==0)
        integralError = 0;
    float returnVal = kp*error + ki*integralError + kd*(error - prevError);
    prevError = error;
    return returnVal;
}

