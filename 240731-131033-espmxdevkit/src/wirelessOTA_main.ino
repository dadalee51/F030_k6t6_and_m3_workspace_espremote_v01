/**
 * First uploads please use legacy arduino IDE, then can use platforIO project
 */
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <ESP8266WebServer.h>
#include <StreamString.h>
#include <espNow.h>
#define SLAVE_ADDR 0x0C
#define FOR(I, N) for (int I = 0; I < N; I++)
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

const char *apid = "TIGO_R030_A01";
const char *pswd = "12345678";
ESP8266WebServer server(80);
long last_millis = 0;
#ifdef ACC_ON
#include "SparkFun_LIS2DH12.h"
SPARKFUN_LIS2DH12 accel;
float z_acc = 0.0;
#endif

// function header
StreamString htmlBuffer;
StreamString outputBuffer;
char ipString[16];
int slaveData[8];

typedef struct struct_message {
  char a[32];
  int b;
  float c;
  String d;
  bool e;
} struct_message;

// Create a struct_message called myData
struct_message myData;


unsigned long lastTime = 0;  
unsigned long timerDelay = 2000;  // send readings timer


// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}

void handleRoot()
{
  IPAddress localIp = WiFi.localIP();
  sprintf(ipString, "%d.%d.%d.%d", localIp[0], localIp[1], localIp[2], localIp[3]);
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;
  // scanI2CDevice(); //only enable when needed.
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

void setup()
{
  htmlBuffer.reserve(200);   // approx 200 chars
  outputBuffer.reserve(200); // same
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(apid, pswd);
  // IPAddress IP = WiFi.softAPIP();
  WiFi.begin("076519399", "076519399");

  ArduinoOTA.begin();
  pinMode(2, OUTPUT);
  server.on("/", handleRoot);
  server.begin();
  // Wire.setClock(100000); //set i2c fast mode.
  Wire.begin(4, 14);  // join i2c as master //sda4,scl14
  pinMode(2, OUTPUT); // led for m3
  Serial.begin(115200);

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);
   // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}
int tempInt = 0;
void loop()
{
  server.handleClient(); // use this to enable airSerial
  ArduinoOTA.handle();   // required for ota.
  // read from slave stm32,
  Wire.requestFrom(SLAVE_ADDR, 32); // request x bytes from sensor
  FOR(i, 8)
  {
    if (Wire.available() >= sizeof(int))
    {
      tempInt = 0;
      FOR(j, 4)
      {
        tempInt = tempInt | (Wire.read() << (8 * j)); // Read 4 bytes per integer
      }
      slaveData[i] = tempInt;
    }
  }
  outputBuffer.clear();
  FOR(i, 8)
  outputBuffer.printf("%d  ", slaveData[i]);
 
  if ((millis() - lastTime) > timerDelay) {
    // Set values to send
    strcpy(myData.a, "THIS IS A CHAR");
    myData.b = random(1,20);
    myData.c = 1.2;
    myData.d = "Hello";
    myData.e = false;

    // Send message via ESP-NOW
    esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));

    lastTime = millis();
  }


}

void sendToSlave()
{
  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write("ABC");
  Wire.endTransmission();
}