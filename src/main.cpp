/*
 *   Written by: Paul Tappeiner   /   05tappau@rgtfo-me.it
 *   early 2024
 *   free for public use              dont quote me i aint no lawyer
 */

#include <Arduino.h>
#include <ezButton.h>

ezButton StartRead(7);

#define HighEdge 825
#define LowEdge 775

bool nowstate = 0, prevstate = 0, currentlyReading = 0, GoCalculate = 0;

//  can consist of 67 Bits if we take away the Guard Zone we end up with (67-(3+3+5)=)56
//  lets stick with 64 cuz i am to lazy to not record the inner 5 Guard Bits
//  The start of a code will be indicatetd by a buttonpress because it makes stuff easier and its more realistic, ever used a reader?

unsigned int Starttimestamp[4];
bool Signalstate[64];
bool DecodeNums[20][7]{
    {0, 0, 0, 1, 1, 0, 1},
    {0, 0, 1, 1, 0, 0, 1},
    {0, 0, 1, 0, 0, 1, 1},
    {0, 1, 1, 1, 1, 0, 1},
    {0, 1, 0, 0, 0, 1, 1},
    {0, 1, 1, 0, 0, 0, 1},
    {0, 1, 0, 1, 1, 1, 1},
    {0, 1, 1, 1, 0, 1, 1},
    {0, 1, 1, 0, 1, 1, 1},
    {0, 0, 0, 1, 0, 1, 1},
    //      4 End Digits
    {1, 1, 1, 0, 0, 1, 0},
    {1, 1, 0, 0, 1, 1, 0},
    {1, 1, 0, 1, 1, 0, 0},
    {1, 0, 0, 0, 0, 1, 0},
    {1, 0, 1, 1, 1, 0, 0},
    {1, 0, 0, 1, 1, 1, 0},
    {1, 0, 1, 0, 0, 0, 0},
    {1, 0, 0, 0, 1, 0, 0},
    {1, 0, 0, 1, 0, 0, 0},
    {1, 1, 1, 0, 1, 0, 0}};

int BinReadOutInt[20];
String BinReadOutString[20];
String DedodeNumsString[20]
{
    //      for the 4 Start Digits
    "0001101",
    "0011001",
    "0010011",
    "0111101",
    "0100011",
    "0110001",
    "0101111",
    "0111011",
    "0110111",
    "0001011",
    //      for the 4 End Digits
    "1110010",
    "1100110",
    "1101100",
    "1000010",
    "1011100",
    "1001110",
    "1010000",
    "1000100",
    "1001000",
    "1110100"
};

int DecodedDigits[8]{0, 0, 0, 0, 0, 0, 0, 0};

unsigned int prevRead = 0;
int Startavg = 0;
int signalNum = 0;

void analog_to_Binary()
{
    //  approache where you look after x millis and note the state
    if (signalNum <= 4)
    {
        if (analogRead(A0) >= HighEdge)
            nowstate = 1;
        if (analogRead(A0) <= LowEdge)
            nowstate = 0;
        if (nowstate != prevstate)
        {
            Starttimestamp[signalNum] = millis();
            prevstate = nowstate;
            signalNum++;
        };
        if (signalNum == 4)
            Startavg = (Starttimestamp[1] - Starttimestamp[0]) + (Starttimestamp[2] - (Starttimestamp[1] + Starttimestamp[0])) + (Starttimestamp[3] - (Starttimestamp[2] + Starttimestamp[1] + Starttimestamp[0]));
    }

    if (signalNum >= 4 && signalNum < 64)
    {
        if (millis() - prevRead >= Startavg)
        {
            if (analogRead(A0) >= HighEdge)
                Signalstate[signalNum] = 1;

            if (analogRead(A0) <= LowEdge)
                Signalstate[signalNum] = 0;
        }
    }

    if (signalNum == 64)
    {
        currentlyReading = 0;
        GoCalculate = 1;
    }
}

/*      The weird aproache wher you note the time of every change kinda scuffed
    if (analogRead(A0) >= HighEdge)
        nowstate = 1;
    if (analogRead(A0) <= LowEdge)
        nowstate = 0;

    if (nowstate != prevstate)
    {
        Signalstate[signalNum] = nowstate;
        timestamp[signalNum] = millis();
        prevstate = nowstate;
        signalNum++;

        if (signalNum == 67)
        {
            currentlyReading = 0;
            GoCalculate = 1;
        }
    };
*/

void EANDecodeViaINT()
{                               //  start with bringing it to string because i didnt find a easy way to make them ints
    //  4 start Digits
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 7; j++)
        {
            BinReadOutString[i] = Signalstate[j+i*j];
        }
    }
    //  4 End Digits
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 7; j++)
        {
            BinReadOutString[i] = Signalstate[j+i*j+5];
        }
    }
}

void EANDecodeViaString()
{   
    //  4 start Digits
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 7; j++)
        {
            BinReadOutString[i] = Signalstate[j+i*j];
        }
    }
    //  4 End Digits
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 7; j++)
        {
            BinReadOutString[i] = Signalstate[j+i*j+5];
        }
    }
    //  compare to Lookuptable
    for (size_t i = 0; i < count; i++)
    {
        if (BinReadOutString[i]==)
        {
            /* code */
        }
        
    }
}

void EANDecode()
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            for (int k = 0; k < 7; k++)
            {
                int tempChecksum = 0;
                if (Signalstate[i * 7 + k] == DecodeNums[i][k]) //  multpl by 7 becaus there are 7 bits per digit
                {
                    tempChecksum++;
                }
                if (tempChecksum == 7)
                    DecodedDigits[i] = j;
            }
        }
    }

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            for (int k = 0; k < 7; k++)
            {
                int tempChecksum = 0;
                if (Signalstate[i * 7 + k + 5] == DecodeNums[i][k]) //  multpl by 7 becaus there are 7 bits per digit + 5 cuz those extra 5 guard bits
                {
                    tempChecksum++;
                }
                if (tempChecksum == 7)
                    DecodedDigits[i + 4] = j;
            }
        }
    }
}

void setup()
{
    Serial.begin(9600);
    pinMode(A0, INPUT);
    StartRead.setDebounceTime(50); // set debounce time to 50 milliseconds
}

void loop()
{
    if (StartRead.isPressed() == true || currentlyReading == true)
    {
        analog_to_Binary();
    }
    if (GoCalculate == 1)
    {
        EANDecode();
    }
}