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
  pinMode(PA8, OUTPUT);
  pinMode(PA11, OUTPUT);
  digitalWrite(PB4, 1); //red
  digitalWrite(PA8, 1);  //gren off
  digitalWrite(PA11, 1); //blue off
  //grounded switches
  pinMode(PB0, INPUT_PULLUP);
  pinMode(PB1, INPUT_PULLUP);
  pinMode(PB7, INPUT_PULLUP);
  pinMode(PA12, INPUT_PULLUP);
  
  hs.begin(115200);

#ifdef ACC_ON
  if (!accel.begin())
  {
    digitalWrite(PB4, 1);//red led off
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
  pswtch[0] = digitalRead(PB0);
  pswtch[1] = digitalRead(PB1);
  pswtch[2] = digitalRead(PB7);
  pswtch[3] = digitalRead(PA12);
  hs.printf("%d\t%d\t%d\t%d.\t", joystick[0], joystick[1], joystick[2], joystick[3]);
  hs.printf("%d\t%d\t%d\t%d \r\n", pswtch[0], pswtch[1], pswtch[2], pswtch[3]);
#ifdef ACC_ON
  z_acc = accel.getZ();
  if (z_acc < 0)
  {
    // digitalWrite(PB4, 0);  // red on when flipped
    // digitalWrite(PA8, 1);  //
    digitalWrite(PA11, 0xFF); //B
  }
  else
  {
    // digitalWrite(PB4, 0);  // red off
    // digitalWrite(PA8, 0);  // green on upright
    digitalWrite(PA11, 0xff); //
  }
  // digitalWrite(PB4, 0x0);  // red led on
#endif
}
