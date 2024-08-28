/**
 * First uploads please use legacy arduino IDE, then can use platforIO project
 */
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <ESP8266WebServer.h>
#include <StreamString.h>
#define SLAVE_ADDR 0x0C
#define FOR(I,N) for(int I=0;I<N;I++)
const char*apid = "TIGO_R030_A01";
const char*pswd = "12345678";
ESP8266WebServer server(80);

#ifdef ACC_ON
  #include "SparkFun_LIS2DH12.h"
  SPARKFUN_LIS2DH12 accel;
  float z_acc=0.0;
#endif

//function header 
StreamString htmlBuffer;
StreamString outputBuffer;
char ipString[16];
int slaveData[8];
void handleRoot() {
  IPAddress localIp = WiFi.localIP();
  sprintf(ipString,"%d.%d.%d.%d" ,localIp[0],localIp[1],localIp[2],localIp[3]);
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;
  //scanI2CDevice(); //only enable when needed.
  outputBuffer.replace("\n", "<br/>");
  htmlBuffer.clear();
  htmlBuffer.printf("<html><head><meta http-equiv='refresh' content='5'/></head>\
  <body>\
    <h1>esp m3 Data Output (Air Serial)</h1>\
    <p>Uptime: %02d:%02d:%02d</p>\
    <p>IP Adress: %s </p>\
    <p>Data: %s </p>\
  </body>\
</html>",
              hr, min % 60, sec % 60, ipString, outputBuffer.c_str());
  server.send(200, "text/html", htmlBuffer.c_str());
}

void setup() {
  htmlBuffer.reserve(200);  //approx 200 chars
  outputBuffer.reserve(200); //same
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(apid, pswd);
  //IPAddress IP = WiFi.softAPIP();
  WiFi.begin("076519399", "076519399");

  ArduinoOTA.begin();
  pinMode(2, OUTPUT);
  server.on("/", handleRoot);
  server.begin();
  //Wire.setClock(100000); //set i2c fast mode.
  Wire.begin(4,14); //join i2c as master //sda4,scl14
  pinMode(2, OUTPUT); //led for m3
  Serial.begin(115200);
  
}
int tempInt=0;
void loop() {
  server.handleClient(); //use this to enable airSerial
  ArduinoOTA.handle(); //required for ota.
  //read from slave stm32, 
  Wire.requestFrom(SLAVE_ADDR, 8);    // request x bytes from sensor
  delay(10);
  //Serial.println(Wire.available());
  //int i=0;
 // while(Wire.available()>0){
  FOR(i,8)  slaveData[i]=Wire.read();
  //  i++;
  //}
  // Serial.println();
   delay(10);
  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write("ABC");
  Wire.endTransmission();
  delay(10);
  // FOR(i,8){
  //   if (Wire.available() >= sizeof(int)) {
  //     // digitalWrite(2,1);
      
  //     tempInt=0;
  //     FOR(j,4){
  //       tempInt = tempInt | (Wire.read() <<  (8 * j));  // Read 4 bytes per integer
  //       slaveData[j]=tempInt;
  //     }
      
  //     // digitalWrite(2,0);
  //   }
  // }
  outputBuffer.clear();
  FOR(i,8)outputBuffer.printf("%d  ", slaveData[i]);
}
