#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#define counterPin 14
#define counterResetPin 12

String sta_ssid, sta_pass, ap_ssid, ap_pass, mdns, token, chat_id, text;
int count = 0, counterPin_old = 1;
IPAddress ip, gateway, subnet;
ESP8266WebServer server(80);

void setup(void) {
  pinMode(counterPin, INPUT_PULLUP);
  pinMode(counterResetPin, INPUT_PULLUP);

  Serial.begin(115200);
  delay(100);
//  while (!Serial) continue;
  
  SPIFFS.begin();
  delay(100);
  
  readCounter();
  readConfig();
  delay(500);

  WiFi.mode(WIFI_AP_STA);
  WiFi.config(ip, gateway, subnet);
  WiFi.softAP(ap_ssid, ap_pass);
  WiFi.begin(sta_ssid, sta_pass);
  WiFi.waitForConnectResult();
  MDNS.begin(mdns);

  //  server.sendHeader("Access-Control-Allow-Origin", "*");
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

  // Inc counter
  int state = digitalRead(counterPin);
  if ((state == LOW) && (counterPin_old == 1)) {
    count++;
    writeCounter();
    sendNotification();
  }
  counterPin_old = state;
  delay(10);


  // Reset counter
  if ((digitalRead(counterResetPin) == LOW) && (count != 0)) {
    resetCounter();
  }
}
