#include <Arduino.h>
#define ACC_ON
#ifdef ACC_ON
#include "SparkFun_LIS2DH12.h"
SPARKFUN_LIS2DH12 accel;
float z_acc = 0.0;
#endif
#define FOR(I, N) for (int I = 0; I < N; I++)

int joystick[4] = {0, 0, 0, 0};
int pswtch[4] = {0,0,0,0};
HardwareSerial hs = HardwareSerial(PA_15, PB_6);
void setup()
{
  //analog input pot joysticks
  pinMode(PA_0, INPUT_ANALOG);
  pinMode(PA_1, INPUT_ANALOG);
  pinMode(PA_2, INPUT_ANALOG);
  pinMode(PA_3, INPUT_ANALOG);
  //rgb led
  pinMode(PB4, OUTPUT_OPEN_DRAIN);
  pinMode(PA_8, OUTPUT_OPEN_DRAIN);
  pinMode(PA_11, OUTPUT_OPEN_DRAIN);
  digitalWrite(PA_8, 1);  //gren off
  digitalWrite(PA_11, 1); //blue off
  digitalWrite(PB4, 0); //red
  //grounded switches
  pinMode(PB_0, INPUT);
  pinMode(PB_1, INPUT);
  pinMode(PB_7, INPUT_PULLUP);
  pinMode(PA_12, INPUT);
  
  hs.begin(115200);

#ifdef ACC_ON
  if (!accel.begin())
  {
    digitalWrite(PB4, 0);//red led on
    delay(100);
  }
#endif
}

void loop()
{
  joystick[0] = analogRead(PA_0);
  joystick[1] = analogRead(PA_1);
  joystick[2] = analogRead(PA_2);
  joystick[3] = analogRead(PA_3);
  pswtch[0] = digitalRead(PB_0);
  pswtch[1] = digitalRead(PB_1);
  pswtch[2] = digitalRead(PB_7);
  pswtch[3] = digitalRead(PA_12);
  hs.printf("%d\t%d\t%d\t%d.\t", joystick[0], joystick[1], joystick[2], joystick[3]);
  hs.printf("%d\t%d\t%d\t%d \r\n", pswtch[0], pswtch[1], pswtch[2], pswtch[3]);
#ifdef ACC_ON
  z_acc = accel.getZ();
  if (z_acc > 0)
  {
    // digitalWrite(PB4, 0);  // red on when flipped
    // digitalWrite(PA_8, 1);  //
    digitalWrite(PA_11, 1); //
  }
  else
  {
    // digitalWrite(PB4, 0);  // red off
    // digitalWrite(PA_8, 0);  // green on upright
    digitalWrite(PA_11, 1); //
  }
    digitalWrite(PB4, 0);  // red led on
#endif
}
