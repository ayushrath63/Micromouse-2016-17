#include "mbed.h"

//IR blink
DigitalOut IR_FL(PB_0);
DigitalOut IR_FR(PC_11);
DigitalOut IR_L(PB_7);
DigitalOut IR_R(PC_10);

//IR_Reader
AnalogIn IRR_FR(PA_4);
AnalogIn IRR_FL(PC_1);
AnalogIn IRR_R(PA_0);
AnalogIn IRR_L(PC_0);

Ticker IR_FL_F;  //for updating speed every second
Ticker second;
Ticker third;
Ticker fourth;

Serial pc(SERIAL_TX,SERIAL_RX);

void IR_FL_read()
{
    IR_R = 1;
    wait(.01);
    pc.printf("Left Forward Distance: %d \r\n", IRR_R.read());
    IR_R = 0;
}

int main()
{
    
    IR_FL_F.attach(&IR_FL_read, 1);
    /*second.attach(&second_func,1);
     third.attach(&third_fun,1);
     fourth.attach(&fourth_func, 1);
     
     while(1)
     {
     pc.printf("Left Forward Distance: %d \r\n", IR_FL.read());
     //pc.printf("Right Forward Distance: %d \r\n", IR_FR.read());
     //pc.printf("Right Distance: %d \r\n", IR_R.read());
     //pc.printf("Left Distance: %d \r\n\n", IR_L.read());
     wait(1.0);
     }*/
}






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