#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#define counterPin 14
#define counterResetPin 12

struct wifi {
  String ssid;
  String pass;
};

struct telegram {
  String token;
  String chat_id;
  String text;
};

struct network {
  IPAddress ip; 
  IPAddress gateway;
  IPAddress subnet;
  String mdns;
};

wifi sta;
wifi ap;
telegram telegram;
network network;
int count = 0, counterPin_old = 1;
ESP8266WebServer server(80);

void setup(void) {
  pinMode(counterPin, INPUT_PULLUP);
  pinMode(counterResetPin, INPUT_PULLUP);
  
  Serial.begin(115200);
  delay(100);

  SPIFFS.begin();
  delay(100);
  
  readCounter();
  readConfig();
  delay(500);
  
  WiFi.mode(WIFI_AP_STA);
  WiFi.config(network.ip, network.gateway, network.subnet);
  WiFi.softAP(ap.ssid, ap.pass);
  WiFi.begin(sta.ssid, sta.pass);
  WiFi.waitForConnectResult();
  MDNS.begin(network.mdns);
  
  server.on("/", HTTP_GET, handleRoot);
  server.on("/api/restart", HTTP_GET, handleRestart);
  server.on("/api/counter", HTTP_GET, handleGetCounter);
  server.on("/api/counter", HTTP_POST, handleResetCounter);
  server.on("/api/config", HTTP_GET, handleGetConfig);
  server.on("/api/config/wifi", HTTP_GET, handleGetWifiConfig);
  server.on("/api/config/wifi", HTTP_POST, handleSetWifiConfig);
  server.on("/api/config/wifi/sta", HTTP_GET, handleGetWifiStaConfig);
  server.on("/api/config/wifi/sta", HTTP_POST, handleSetWifiStaConfig);
  server.on("/api/config/wifi/ap", HTTP_GET, handleGetWifiApConfig);
  server.on("/api/config/wifi/ap", HTTP_POST, handleSetWifiApConfig);
  server.on("/api/config/network", HTTP_GET, handleGetNetworkConfig);
  server.on("/api/config/network", HTTP_POST, handleSetNetworkConfig);
  server.on("/api/config/telegram", HTTP_GET, handleGetTelegramConfig);
  server.on("/api/config/telegram", HTTP_POST, handleSetTelegramConfig);
  server.on("/favicon", HTTP_GET, handleFavicon);
  server.on("/bootstrap", HTTP_GET, handleBootstrap);
  server.onNotFound(handleNotFound);
  server.begin();
}

void loop(void) {
  server.handleClient();
  MDNS.update();
  
  int state = digitalRead(counterPin);
  if ((state == LOW) && (counterPin_old == 1)) {
    count++; writeCounter();
    sendNotification();
  }
  counterPin_old = state;
  delay(10);
  
  if ((digitalRead(counterResetPin) == LOW) && (count != 0)) {
    resetCounter();
  }
}
