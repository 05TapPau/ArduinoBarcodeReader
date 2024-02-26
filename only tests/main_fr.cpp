
#include <Arduino.h>
#define HighEdge 825
#define LowEdge 775

/*
  44 Edges on my first EAN
*/

bool nowstate = 0, prevstate = 0;
bool reading = 1, wasreading = 0;

unsigned int signaltime[44][2];

int signalNum = 0;

void analog_to_Binary()
{
  if (analogRead(A0) >= HighEdge)
    nowstate = 1;
  if (analogRead(A0) <= LowEdge)
    nowstate = 0;

  if (nowstate != prevstate)
  {
    prevstate = nowstate;

    // every time this changes it shoud measure the time atleast for the first two strips

    signaltime[signalNum][0] = nowstate;
    signaltime[signalNum][1] = millis() - signaltime[signalNum - 1][1];
    
    Serial.print(signaltime[signalNum][0]);
    Serial.print(" - ");
    Serial.println(signaltime[signalNum][1]);

    signalNum++;
  }
};

void calcOutPut()
{
  Serial.print("Done");
}

void setup()
{
  Serial.begin(9600);
  pinMode(A0, INPUT);
}

void loop()
{
  if (reading == 1)
  {
    analog_to_Binary();
  }

  if (reading == 0)
  {
    calcOutPut();
    reading == 1;
  }
}