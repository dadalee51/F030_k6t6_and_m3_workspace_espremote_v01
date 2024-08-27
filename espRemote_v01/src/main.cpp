#include <Arduino.h>
#include <Wire.h>
// #define DEBUG_ON //without debug, flash is at 69.8%
// #define ACC_ON
#ifdef ACC_ON
#include "SparkFun_LIS2DH12.h"
SPARKFUN_LIS2DH12 accel;
float z_acc = 0.0;
#endif
#define FOR(I, N) for (int I = 0; I < N; I++)

#define SLAVE_ADDRESS 0x08

int joystick[4] = {0, 0, 0, 0}; // 1~1023 where mid is [524,528,516,520]
int pswtch[4] = {0, 0, 0, 0};   // default to 1,1,1,1
HardwareSerial hs = HardwareSerial(PA_15, PB_6);

// i2c onRequest to master
void requestEvent(){
  FOR(i, 4) Wire.write(joystick[i]);
  FOR(i, 4) Wire.write(pswtch[i]);
}

void setup()
{
  Wire.begin((uint32_t)PA_10, (uint32_t)PA_9);
  Wire.onRequest(requestEvent);
  // analog input pot joysticks
  pinMode(PA_0, INPUT_ANALOG);
  pinMode(PA_1, INPUT_ANALOG);
  pinMode(PA_2, INPUT_ANALOG);
  pinMode(PA_3, INPUT_ANALOG);
  // rgb led
  pinMode(PB4, OUTPUT_OPEN_DRAIN);
  pinMode(PA8, OUTPUT);
  pinMode(PA11, OUTPUT);
  digitalWrite(PB4, 1);  // red
  digitalWrite(PA8, 1);  // gren off
  digitalWrite(PA11, 1); // blue off
  // grounded switches
  pinMode(PB0, INPUT_PULLUP);
  pinMode(PB1, INPUT_PULLUP);
  pinMode(PB7, INPUT_PULLUP);
  pinMode(PA12, INPUT_PULLUP);
  hs.begin(115200);
#ifdef ACC_ON
  if (!accel.begin())
  {
    digitalWrite(PB4, 0); // red
    delay(100);
    digitalWrite(PB4, 1); // red
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
#ifdef DEBUG_ON
  hs.printf("%d\t%d\t%d\t%d\t", joystick[0], joystick[1], joystick[2], joystick[3]);
  hs.printf("%d\t%d\t%d\t%d\r\n", pswtch[0], pswtch[1], pswtch[2], pswtch[3]);
#endif
#ifdef ACC_ON
  z_acc = accel.getZ();
  if (z_acc < 0)
  {
    digitalWrite(PB4, 0); // red on when flipped
  }
  else
  {
    digitalWrite(PB4, 1); // red off
  }
#endif
}
