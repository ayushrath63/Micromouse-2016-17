#include "mbed.h"

Timer t;
Serial pc(SERIAL_TX, SERIAL_RX);

InterruptIn channelB(PC_4); //right encoder



class Motor
{
public:
    Motor(PwmOut* motorF, PwmOut* motorB)
    {
        _motorF = motorF;
        _motorB = motorB;
        _speed = 0;
    }
    void setSpeed(float speed)
    {
        //if statement for positive value = forward, negative value =
        _speed = speed;
        if (_speed > 0)
        {
            _motorF->write(speed);
            _motorB->write(0);
        }
        else if (_speed <0)
        {
            _motorF->write(0);
            _motorB->write(speed);
        }
        else
        {
            _motorF->write(0);
            _motorB->write(0);
        }
        
    }
    float getSpeed()
    {
        return _speed;
    }
    void stop()
    {
        _speed=0;
        _motorF->write(1);
        _motorB->write(1);
        wait(.5);
        _motorF->write(0);
        _motorB->write(0);
    }
private:
    PwmOut *_motorF, *_motorB;
    float _speed;
};

PwmOut MRF(PA_7);
PwmOut MRB(PB_6);
PwmOut MLF(PB_10);
PwmOut MLB(PC_7);

Motor RightMotor(&MRF, &MRB);
Motor LeftMotor(&MLF, &MLB);


DigitalOut myled(LED1);

volatile unsigned long pulses = 0;
void incrementEncoder()
{
    pulses++;
}


int main()
{
    RightMotor.setSpeed(.1); //off for .09 and on for .01
    //LeftMotor.setSpeed(.1);
    (&MRF)->write(.5);
    
    while(1){
        wait(1);
        pc.printf("Pulses is: %d \r\n", pulses);
        pc.printf("%f", RightMotor.getSpeed());
    }
}

