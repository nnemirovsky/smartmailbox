void writeCounter() {
  File f = SPIFFS.open("/counter", "w");
  f.write(count);
  f.close();
}

void readCounter() {
  File f = SPIFFS.open("/counter", "r");
  count = f.read();
  f.close();
}

void resetCounter() {
  count = 0;
  writeCounter();
}

DynamicJsonDocument getConfig() {
  DynamicJsonDocument conf(1024);
  File f = SPIFFS.open("/config", "r");
  deserializeJson(conf, f);
  f.close();
  return conf;
}

void saveConfig(String conf) {
  File f = SPIFFS.open("/config", "w");
  f.print(conf);
  f.close();
}

void readConfig() {
  DynamicJsonDocument doc = getConfig();
  sta_ssid = doc["wifi"]["sta"]["ssid"].as<String>();
  sta_pass = doc["wifi"]["sta"]["pass"].as<String>();
  ap_ssid = doc["wifi"]["ap"]["ssid"].as<String>();
  ap_pass = doc["wifi"]["ap"]["pass"].as<String>();
  ip.fromString(doc["network"]["ip"].as<String>());
  gateway.fromString(doc["network"]["gateway"].as<String>());
  subnet.fromString(doc["network"]["subnet"].as<String>());
  mdns = doc["network"]["mdns"].as<String>();
  token = doc["telegram"]["token"].as<String>();
  chat_id = doc["telegram"]["chat_id"].as<String>();
  text = doc["telegram"]["text"].as<String>();
}

void sendNotification() {
  std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
  client->setInsecure();
  HTTPClient https;
  String url = "https://api.telegram.org/bot" + token + "/sendMessage?chat_id=" + chat_id + "&text=" + text;
  url.replace("$counter$", String(count));
  https.begin(*client, url);
  https.GET();
  https.end();
}
