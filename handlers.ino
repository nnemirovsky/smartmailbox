void handleFavicon() {
  File f = SPIFFS.open(faviconFile, "r");
  server.streamFile(f, "image/png");
  f.close();
}

void handleBootstrap() {
  File f = SPIFFS.open(bootstrapFile, "r");
  server.streamFile(f, "text/css");
  f.close();
}
void handleRoot() {
  File f = SPIFFS.open(indexPageFile, "r");
  server.streamFile(f, "text/html");
  f.close();
}

void handleRestart() {
  server.send(200, "application/json", "{\"msg\":\"the device will be restarted\"}");
  delay(100);
  pinMode(0, OUTPUT);
  digitalWrite(0, 1);
  pinMode(2, OUTPUT);
  digitalWrite(2, 1);
  ESP.reset();
}

void handleGetCounter() {
  server.send(200, "text/plain", String(count));
}

void handleResetCounter() {
  if (count != 0) {
    resetCounter();
    server.send(200, "application/json", "{\"msg\":\"done\"}");
  } else {
    server.send(200, "application/json", "{\"msg\":\"counter cannot be reset because its value is 0\"}");
  }
}

void handleNotFound() {
  server.send(404, "text/plain", "404: Not found");
}

void handleGetConfig() {
  String conf_str;
  serializeJson(getConfig(), conf_str);
  server.send(200, "application/json", conf_str);
}

void handleGetWifiConfig() {
  String conf_str;
  serializeJson(getConfig()["wifi"], conf_str);
  server.send(200, "application/json", conf_str);
}

void handleSetWifiConfig() {
  DynamicJsonDocument conf = getConfig();
  StaticJsonDocument<256> filter, subconf;
  String conf_str;
  filter["sta"]["ssid"] = true;
  filter["sta"]["pass"] = true;
  filter["ap"]["ssid"] = true;
  filter["ap"]["pass"] = true;
  DeserializationError err = deserializeJson(subconf, server.arg("plain"), DeserializationOption::Filter(filter));
  if (err) {
    server.send(400, "application/json", "{\"msg\":\"" + String(err.c_str()) + "\"}");
  } else {
    conf["wifi"] = subconf;
    serializeJson(conf, conf_str);
    saveConfig(conf_str);
    conf_str = "";
    serializeJson(subconf, conf_str);
    server.send(200, "application/json", "{\"msg\":\"Done\",\"data\":" + conf_str + "}");
  }
}

void handleGetWifiStaConfig() {
  String conf_str;
  serializeJson(getConfig()["wifi"]["sta"], conf_str);
  server.send(200, "application/json", conf_str);
}

void handleSetWifiStaConfig() {
  DynamicJsonDocument conf = getConfig();
  StaticJsonDocument<256> filter, subconf;
  String conf_str;
  filter["ssid"] = true;
  filter["pass"] = true;
  DeserializationError err = deserializeJson(subconf, server.arg("plain"), DeserializationOption::Filter(filter));
  if (err) {
    server.send(400, "application/json", "{\"msg\":\"" + String(err.c_str()) + "\"}");
  } else {
    conf["wifi"]["sta"] = subconf;
    serializeJson(conf, conf_str);
    saveConfig(conf_str);
    conf_str = "";
    serializeJson(subconf, conf_str);
    server.send(200, "application/json", "{\"msg\":\"Done\",\"data\":" + conf_str + "}");
  }
}

void handleGetWifiApConfig() {
  String conf_str;
  serializeJson(getConfig()["wifi"]["ap"], conf_str);
  server.send(200, "application/json", conf_str);
}

void handleSetWifiApConfig() {
  DynamicJsonDocument conf = getConfig();
  StaticJsonDocument<256> filter, subconf;
  String conf_str;
  filter["ssid"] = true;
  filter["pass"] = true;
  DeserializationError err = deserializeJson(subconf, server.arg("plain"), DeserializationOption::Filter(filter));
  if (err) {
    server.send(400, "application/json", "{\"msg\":\"" + String(err.c_str()) + "\"}");
  } else {
    conf["wifi"]["ap"] = subconf;
    serializeJson(conf, conf_str);
    saveConfig(conf_str);
    conf_str = "";
    serializeJson(subconf, conf_str);
    server.send(200, "application/json", "{\"msg\":\"Done\",\"data\":" + conf_str + "}");
  }
}

void handleGetNetworkConfig() {
  String conf_str;
  serializeJson(getConfig()["network"], conf_str);
  server.send(200, "application/json", conf_str);
}

void handleSetNetworkConfig() {
  DynamicJsonDocument conf = getConfig();
  StaticJsonDocument<256> filter, subconf;
  String conf_str;
  filter["ip"] = true;
  filter["subnet"] = true;
  filter["gateway"] = true;
  filter["mdns"] = true;
  DeserializationError err = deserializeJson(subconf, server.arg("plain"), DeserializationOption::Filter(filter));
  if (err) {
    server.send(400, "application/json", "{\"msg\":\"" + String(err.c_str()) + "\"}");
  } else {
    conf["network"] = subconf;
    serializeJson(conf, conf_str);
    saveConfig(conf_str);
    conf_str = "";
    serializeJson(subconf, conf_str);
    server.send(200, "application/json", "{\"msg\":\"Done\",\"data\":" + conf_str + "}");
  }
}

void handleGetTelegramConfig() {
  String conf_str;
  serializeJson(getConfig()["telegram"], conf_str);
  server.send(200, "application/json", conf_str);
}

void handleSetTelegramConfig() {
  DynamicJsonDocument conf = getConfig();
  StaticJsonDocument<256> filter, subconf;
  String conf_str;
  filter["token"] = true;
  filter["chat_id"] = true;
  filter["text"] = true;
  DeserializationError err = deserializeJson(subconf, server.arg("plain"), DeserializationOption::Filter(filter));
  if (err) {
    server.send(400, "application/json", "{\"msg\":\"" + String(err.c_str()) + "\"}");
  } else {
    telegram.token = subconf["token"].as<String>();
    telegram.chat_id = subconf["chat_id"].as<String>();
    telegram.text = subconf["text"].as<String>();
    conf["telegram"] = subconf;
    serializeJson(conf, conf_str);
    saveConfig(conf_str);
    conf_str = "";
    serializeJson(subconf, conf_str);
    server.send(200, "application/json", "{\"msg\":\"Done\",\"data\":" + conf_str + "}");
  }
}
