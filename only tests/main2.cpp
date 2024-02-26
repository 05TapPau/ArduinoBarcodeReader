#include <Arduino.h>

#define Idle 0
#define Read 1
#define decode 2
#define HighEdge 825
#define LowEdge 775

bool nowstate = 0, prevstate = 0;
int Status = Idle, StartRead = 0;

int avgTBit = 0;
bool ReadOut[3][2];
bool Start[3]{
    1,
    0,
    1};

void WaitForEvent()
{
    if (analogRead(A0) >= HighEdge)
        nowstate = 1;
    if (analogRead(A0) <= LowEdge)
        nowstate = 0;
    if (nowstate != prevstate)
    {
        ReadOut[StartRead][0] = nowstate;
        ReadOut[StartRead][1] = millis();

        StartRead++;
        prevstate = nowstate;
    }

    /*
    //  eigentlich schun decoden?
        for (int i = 0; i < 3; i++)
        {
            if (ReadOut[i] == Start[i])
            {
                StartRead++;
            }
        }
    */
    if (StartRead == 3)
        Status = Read;
}

void GetReadOut()
{
    if (avgTBit == 0){
        avgTBit = (ReadOut[0][1]+ReadOut[1][1]+ReadOut[2][1])/3;
    }
}

void setup()
{
    Serial.begin(9600);
    pinMode(A0, INPUT);
}

void loop()
{
    switch (Status)
    {
    case Idle:
        WaitForEvent();
        break;

    case Read:
        GetReadOut();
        break;

    case decode:
        break;

    default:
        break;
    }
}