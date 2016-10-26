#include "mbed.h"

// motor, timer, serial port

Timer t;
Serial pc(SERIAL_TX, SERIAL_RX);


InterruptIn channelA(PA_1); //right encoder
InterruptIn channelB(PC_4); //right encoder
//PA_15, PB_3 < left encoder (they are broken)



class Motor
{
public:
    Motor(PwmOut motorF, PwmOut motorB)
    {
        _motorF = motorF;
        _motorB = motorB;
        _period = .1;
        _motorF.period = _period;
        _motorB.period = _period;
    }
    void setSpeed(float speed)
    {
        //if statement for positive value = forward, negative value =
        _speed = speed;
        if (_speed > 0)
        {
            _motorF = _speed;
            _motorB = 0;
        }
        else if (_speed <0)
        {
            _motorF = 0;
            _motorB = _speed;
        }
        else
        {
            _motorF = 0;
            _motorB = 0;
        }
        
    }
    float getSpeed()
    {
        return _speed;
    }
    void stop()
    {
        _motorF = 1;
        _motorB = 1;
        wait(.5);
        _motorF = 0;
        _motorB = 0;        _
    }
private:
    PwmOut _motorF, _motorB;
    float _speed;
    float _period;
    int _motorF, _motorB
};

Motor RightMotor(PwmOut MRF(PA_7), PwmOut MRB(PB_6));
Motor LeftMotor(PwmOut MLF(PB_10), PwmOut MLB(PC_7));


DigitalOut myled(LED1);


volatile unsigned long pulses = 0;
void incrementEncoder()
{
    pulses++;
}

int PID(


int main() {


    channelA.rise(&incrementEncoder);
    channelA.fall(&incrementEncoder);
    channelB.rise(&incrementEncoder);
    channelB.fall(&incrementEncoder);
    
    //MRF.period(.1);
    //MRF = .1;
    RightMotor.setSpeed(.1); //off for .09 and on for .01
    LeftMotor.setSpeed(.1);
    while(1){
        wait(1);
        pc.printf("Pulses is: %d \r\n", pulses);
    }


}

