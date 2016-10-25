#include "mbed.h"

// motor, timer, serial port
PwmOut MRF(PA_7);
PwmOut MRB(PB_6);
PwmOut MLF(PB_10);
PwmOut MLB(PC_7);
Timer t;
Serial pc(SERIAL_TX, SERIAL_RX);


InterruptIn channelA(PA_1); //right encoder
InterruptIn channelB(PC_4); //right encoder
//PA_15, PB_3 < left encoder (they are broken)




DigitalOut myled(LED1);


volatile unsigned long pulses = 0;
void incrementEncoder()
{
    pulses++;
}


int main() {


    channelA.rise(&incrementEncoder);
    channelA.fall(&incrementEncoder);
    channelB.rise(&incrementEncoder);
    channelB.fall(&incrementEncoder);
    
    //MRF.period(.1);
    //MRF = .1;
    MLF.period(.1);
    MLF = .1;
    while(1){
        wait(1);
        pc.printf("Pulses is: %d \r\n", pulses);
    }


}
