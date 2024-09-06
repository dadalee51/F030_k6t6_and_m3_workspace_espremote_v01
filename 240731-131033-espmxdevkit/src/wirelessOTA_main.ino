/**
 * First uploads please use legacy arduino IDE, then can use platforIO project
 */
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <ESP8266WebServer.h>
#include <StreamString.h>
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

typedef struct struct_message
{
  int id; // must be unique for each sender board
  int x;
  int y;
} struct_message;

// Create a struct_message called myData
struct_message myData;

// Create peer interface
esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
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
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }
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

  if (last_millis - millis() > 1000)
  { 
    last_millis = millis();
    // Set values to send
    myData.id = 1;
    myData.x = random(0, 50);
    myData.y = random(0, 50);

    // Send message via ESP-NOW
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));

    if (result == ESP_OK)
    {
      Serial.println("Sent with success");
    }
    else
    {
      Serial.println("Error sending the data");
    }
  }
}

void sendToSlave()
{
  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write("ABC");
  Wire.endTransmission();
}