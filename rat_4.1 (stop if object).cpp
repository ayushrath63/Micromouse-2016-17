#include "mbed.h"

//IR Emitter
DigitalOut IR_FL(PB_0);  DigitalOut IR_FR(PC_11); DigitalOut IR_L(PB_7); DigitalOut IR_R(PC_10);
//IR_Reader/Receiver
AnalogIn IRR_FR(PA_4); AnalogIn IRR_FL(PC_1); AnalogIn IRR_R(PA_0); AnalogIn IRR_L(PC_0);
//Motors
PwmOut MRF(PA_7); PwmOut MRB(PB_6); PwmOut MLF(PB_10); PwmOut MLB(PC_7);

DigitalOut myled(LED1);

Ticker IR_R_F;  //for updating speed every second
Ticker second;
Ticker third;
Ticker fourth;

Serial pc(SERIAL_TX,SERIAL_RX);

float IR_Reading(DigitalOut IR_Emitter, AnalogIn IR_Receiver)
//Reads IR in cm value
{
    //reads
    float reading;
    IR_Emitter = 1;
    reading = IR_Receiver.read();
    wait(.01);
    IR_Emitter = 0;
    
    //converts to cm from percentage y = 5.3154x + 2.1271
    reading = 1 - reading;
    reading *= 1000;
    
    //data collected, line of best fit found:  //y = 5.3154x + 2.1271   R² = 0.87585    //-0.4013x2 + 11.751x - 16.437 R² = 0.98426 but they do not work
    return reading;
}

float IR_array[4]; //{L,FL,FR,R}
void IR_Readings(float IRarray[]) //{L,FL,FR,R}
{
    IR_FL = 1; IR_FR = 1; IR_R = 1; IR_L = 1;
    IRarray[0] = (1-IRR_L.read())*1000; IRarray[1] = (1-IRR_FL.read())*1000; IRarray[2] = (1-IRR_FR.read())*1000; IRarray[3] = (1-IRR_R.read())*1000;
    wait(.01);
    IR_FL = 0; IR_FR = 0; IR_R = 0; IR_L = 0;
}

void printIR_array()
{
    pc.printf("distance %f %f %f %f \r\n\n", IR_array[0],IR_array[1],IR_array[2],IR_array[3]);
}

float speed;
int main()
{
    second.attach(&printIR_array,1);
    speed = .2;
    
    while(1)
    {
        MRF = speed; MLF = speed;
        IR_Readings(IR_array);
        if (IR_array[1] < 3 || IR_array[2] < 3)
        {
            speed = 0;
        }
        else
        {
            speed = 0.2;
        }
        
    }
}


//calibration, not needed at the moment

/*float baseR = 0; float baseL = 0; float baseFL = 0; float baseFR = 0;
 void calibrateIRR()
 {
 for(int i = 0; i < 50; i++)
 {
 baseR+=IRR_R.read();
 baseL+=IRR_L.read();
 baseFR+=IRR_FR.read();
 baseFL+=IRR_FL.read();
 }
 baseR /= 50;
 baseL /= 50;
 baseFL /= 50;
 baseFR /= 50;
 }*/





/*IR SENSOR READING FUNCTIONS
 
 // motor, timer, serial port, LED
 PwmOut MRF(PA_7);
 PwmOut MRB(PB_6);
 PwmOut MLF(PB_10);
 PwmOut MLB(PC_7);
 Timer t;
 
 //IR
 AnalogIn IR_FR(PA_4);
 AnalogIn IR_FL(PC_1);
 AnalogIn IR_R(PA_0);
 AnalogIn IR_L(PC_0);
 
 Ticker SecondUpdate;  //for updating speed every second
 
 DigitalOut myled(LED1);
 Serial pc(SERIAL_TX,SERIAL_RX);
 
 void updateSpeed();
 void forward(int duration, int speed);
 
 float origianl_speed = 0.5;
 float speed = 0.5;
 
 //encoder
 float IRPID();
 int prevError = 0;
 int integralError = 0; //sum of all error
 
 int main()
 {
 //SecondUpdate.attach(&updateSpeed,0.2);
 t.reset();
 
 
 while(1)
 {
 pc.printf("Left Forward Distance: %d \r\n", IR_FL.read());
 pc.printf("Right Forward Distance: %d \r\n", IR_FR.read());
 pc.printf("Right Distance: %d \r\n", IR_R.read());
 pc.printf("Left Distance: %d \r\n\n", IR_L.read());
 wait(1.0);
 }
 
 
 
 }
 
 void forwardTill(int threshold)
 {
 while (IR_FL.read() < threshold && IR_FR.read() < threshold)
 {
 MRF = speed;
 MLF = speed;
 }
 MRF = 0; MLF = 0;
 wait(3.0);
 
 }
 
 void updateSpeed()
 {
 MRF = speed + IRPID();
 MLF = speed - IRPID();
 }
 
 float IRPID() //1 through 0.1 current if we want to stay at speed .1
 {
 float kp = (1.0/24.0) * speed * .20; // w/o PID, the error is around 24 (unit error = 24),  so 20% change in speed every unit error
 float ki = (1.0/4.0) * speed * 0.008; //w/i ID unit error is 4, so 0.8% additional change every unit error
 float kd = speed * .002; // 0.2% change
 int error = IR_R - IR_L; // ir sensor value
 integralError += error;
 if (error ==0)
 integralError = 0;
 float returnVal = kp*error + ki*integralError + kd*(error - prevError);
 prevError = error;
 return returnVal;
 
 }
 
 */

