void writeCounter() {
  File f = SPIFFS.open(counterFile, "w");
  f.write(count);
  f.close();
}

void readCounter() {
  File f = SPIFFS.open(counterFile, "r");
  count = f.read();
  f.close();
}

void resetCounter() {
  count = 0;
  writeCounter();
}

DynamicJsonDocument getConfig() {
  DynamicJsonDocument conf(1024);
  File f = SPIFFS.open(configFile, "r");
  deserializeJson(conf, f);
  f.close();
  return conf;
}

void saveConfig(String conf) {
  File f = SPIFFS.open(configFile, "w");
  f.print(conf);
  f.close();
}

void readConfig() {
  DynamicJsonDocument doc = getConfig();
  sta.ssid = doc["wifi"]["sta"]["ssid"].as<String>();
  sta.pass = doc["wifi"]["sta"]["pass"].as<String>();
  ap.ssid = doc["wifi"]["ap"]["ssid"].as<String>();
  ap.pass = doc["wifi"]["ap"]["pass"].as<String>();
  network.ip.fromString(doc["network"]["ip"].as<String>());
  network.gateway.fromString(doc["network"]["gateway"].as<String>());
  network.subnet.fromString(doc["network"]["subnet"].as<String>());
  network.mdns = doc["network"]["mdns"].as<String>();
  telegram.token = doc["telegram"]["token"].as<String>();
  telegram.chat_id = doc["telegram"]["chat_id"].as<String>();
  telegram.text = doc["telegram"]["text"].as<String>();
}

void sendNotification() {
  std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
  client->setInsecure();
  HTTPClient https;
  String url = "https://api.telegram.org/bot" + telegram.token + "/sendMessage?chat_id=" + telegram.chat_id + "&text=" + telegram.text;
  url.replace("$counter$", String(count));
  https.begin(*client, url);
  https.GET();
  https.end();
}
