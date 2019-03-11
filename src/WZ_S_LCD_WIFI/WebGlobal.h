#ifndef WEBGLOBAL_H
#define WEBGLOBAL_H

boolean firstStart = true;                    // On firststart = true, NTP will try to get a valid time

strDateTime DateTime;                     // Global DateTime structure, will be refreshed every Second
WiFiUDP UDPNTPClient;                     // NTP Client
unsigned long UnixTimestamp = 0;                // GLOBALTIME  ( Will be set by NTP)
boolean Refresh = false; // For Main Loop, to refresh things like GPIO / WS2812
int cNTP_Update = 0;                      // Counter for Updating the time via NTP
Ticker tkSecond;                        // Second - Timer for Updating Datetime Structure

byte Minute_Old = 100;        // Helpvariable for checking, when a new Minute comes up (for Auto Turn On / Off)

ESP8266WebServer server(80);                  // The Webserver
/*
**
** CONFIGURATION HANDLING
**
*/

bool saveConfig() {
    StaticJsonBuffer<1024> jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();

    json["ssid"] = config.ssid;
    json["password"] = config.password;
    json["dhcp"] = config.dhcp;

    JsonArray& array_ip = json.createNestedArray("IP");
    array_ip.add(config.IP[0]);
    array_ip.add(config.IP[1]);
    array_ip.add(config.IP[2]);
    array_ip.add(config.IP[3]);
    JsonArray& array_mqtt = json.createNestedArray("MqttIP");
    array_mqtt.add(config.MqttIP[0]);
    array_mqtt.add(config.MqttIP[1]);
    array_mqtt.add(config.MqttIP[2]);
    array_mqtt.add(config.MqttIP[3]);
    json["MqttPort"] = config.MqttPort;

    JsonArray& array_mask = json.createNestedArray("Netmask");
    array_mask.add(config.Netmask[0]);
    array_mask.add(config.Netmask[1]);
    array_mask.add(config.Netmask[2]);
    array_mask.add(config.Netmask[3]);
    JsonArray& array_gateway = json.createNestedArray("Gateway");
    array_gateway.add(config.Gateway[0]);
    array_gateway.add(config.Gateway[1]);
    array_gateway.add(config.Gateway[2]);
    array_gateway.add(config.Gateway[3]);
    json["ntpServerName"] = config.ntpServerName;
    json["Update_Time_Via_NTP_Every"] = config.Update_Time_Via_NTP_Every;
    json["timezone"] = config.timezone;
    json["daylight"] = config.daylight;
    json["DeviceName"] = config.DeviceName;
    json["AutoTurnOff"] = config.AutoTurnOff;
    json["AutoTurnOn"] = config.AutoTurnOn;
    json["TurnOffHour"] = config.TurnOffHour;
    json["TurnOffMinute"] = config.TurnOffMinute;
    json["TurnOnHour"] = config.TurnOnHour;
    json["TurnOnMinute"] = config.TurnOnMinute;
    //json["ota"] = config.ota;
    json["mqtt_topic"] = config.mqtt_topic;
    json["mqtt_subtopic"] = config.mqtt_subtopic;
    json["mqtt_sub"] = config.mqtt_sub;
    json["mqtt_server_domain"] = config.mqtt_server_domain;
    json["mqtt_user"] = config.mqtt_user;
    json["mqtt_password"] = config.mqtt_password;
    json.prettyPrintTo(Serial);

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
        Serial.println("Failed open conf");
        return false;
    }

    json.printTo(configFile);
    return true;
}


bool loadConfig() {
    File configFile = SPIFFS.open("/config.json", "r");
    if (!configFile) {
        Serial.println("Failed open conf");
        return false;
    }

    size_t size = configFile.size();
    if (size > 1024) {
        Serial.println("Conf too large");
        return false;
    }

    // Allocate a buffer to store contents of the file.
    std::unique_ptr<char[]> buf(new char[size]);

    // We don't use String here because ArduinoJson library requires the input
    // buffer to be mutable. If you don't use ArduinoJson, you may as well
    // use configFile.readString instead.
    configFile.readBytes(buf.get(), size);

    StaticJsonBuffer<1024> jsonBuffer;
    JsonObject& json = jsonBuffer.parseObject(buf.get());

    if (!json.success()) {
        Serial.println("Failed: parse conf");
        return false;
    }
    const char* id = json["ssid"];
    config.ssid = String(id);
    const char* password = json["password"];
    config.password = String(password);
    config.dhcp = json["dhcp"];
    JsonArray& jIP = json["IP"];
    config.IP[0] = jIP[0];
    config.IP[1] = jIP[1];
    config.IP[2] = jIP[2];
    config.IP[3] = jIP[3];
    JsonArray& mqttIP = json["MqttIP"];
    config.MqttIP[0] = mqttIP[0];
    config.MqttIP[1] = mqttIP[1];
    config.MqttIP[2] = mqttIP[2];
    config.MqttIP[3] = mqttIP[3];
    JsonArray& gateIP = json["Gateway"];
    config.Gateway[0] = gateIP[0];
    config.Gateway[1] = gateIP[1];
    config.Gateway[2] = gateIP[2];
    config.Gateway[3] = gateIP[3];
    JsonArray& maskIP = json["Netmask"];
    config.Netmask[0] = maskIP[0];
    config.Netmask[1] = maskIP[1];
    config.Netmask[2] = maskIP[2];
    config.Netmask[3] = maskIP[3];
    config.MqttPort = json["MqttPort"];
    const char* ntp = json["ntpServerName"];
    config.ntpServerName = String(ntp);
    config.Update_Time_Via_NTP_Every = json["Update_Time_Via_NTP_Every"];
    config.timezone = json["timezone"];
    config.daylight = json["daylight"];
    const char* device = json["DeviceName"];
    config.DeviceName = String(device);
    config.AutoTurnOff = json["AutoTurnOff"];
    config.AutoTurnOn = json["AutoTurnOn"];
    config.TurnOffHour = json["TurnOffHour"];
    config.TurnOffMinute = json["TurnOnMinute"];
    config.TurnOnHour = json["TurnOnHour"];
    config.TurnOnMinute = json["TurnOffMinute"];
    config.ota = json["ota"];
    const char* topic = json["mqtt_topic"];
    config.mqtt_topic = String(topic);
    const char* subtopic = json["mqtt_subtopic"];
    config.mqtt_subtopic = String(subtopic);
    const char* mqtt_domain = json["mqtt_server_domain"];
    config.mqtt_server_domain = String(mqtt_domain);
    const char* sub = json["mqtt_sub"];
    config.mqtt_sub = String(sub);
    const char* mqttUser = json["mqtt_user"];
    config.mqtt_user = String(mqttUser);
    const char* mqttPassword = json["mqtt_password"];
    config.mqtt_password = String(mqttPassword);
    config.mqtt_pub_topic = config.mqtt_topic + config.mqtt_subtopic;
    return true;
}


void ConfigureWifi(){
    WiFi.disconnect();
    if (AdminEnabled){
        Serial.println("admin enabled");
        WiFi.mode(WIFI_AP_STA);
        WiFi.softAP(ACCESS_POINT_NAME, ACCESS_POINT_PASSWORD);
    }
    else{
        Serial.println("admin disabled");
        WiFi.mode(WIFI_STA);
    }
    Serial.println("Configuring Wifi");
    WiFi.begin(config.ssid.c_str(), config.password.c_str());
    // ... Give ESP 10 seconds to connect to station.
    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
        Serial.write('.');
        //Serial.print(WiFi.status());
        delay(500);
    }
    while (WiFi.status() != WL_CONNECTED) {
        WifiEnabled=false;
        //Serial.println("Wifi Failed! Rebooting...");
        //delay(500);
        //ESP.restart();
    }
    if (!config.dhcp)
    {
        WiFi.config(IPAddress(config.IP[0], config.IP[1], config.IP[2], config.IP[3]), IPAddress(config.Gateway[0], config.Gateway[1], config.Gateway[2], config.Gateway[3]), IPAddress(config.Netmask[0], config.Netmask[1], config.Netmask[2], config.Netmask[3]));
    }
}


void WriteConfig(){
    Serial.println("Writing Conf");
    if (saveConfig()) { Serial.println("Wrote Config done"); }
    else { Serial.println("Wrote Config faild"); }
}

void ReadConfig(){
    Serial.println("Reading Conf");
    if (loadConfig())return;
    else {
        Serial.println("default Conf");
        // DEFAULT CONFIG
        config.ssid = "";
        config.password = "";
        config.dhcp = false;
        config.IP[0] = 0; config.IP[1] = 0; config.IP[2] = 0; config.IP[3] = 0;
        config.MqttIP[0] = 0; config.MqttIP[1] = 0; config.MqttIP[2] = 0; config.MqttIP[3] = 0;
        config.MqttPort = 1883;
        config.Netmask[0] = 255; config.Netmask[1] = 255; config.Netmask[2] = 255; config.Netmask[3] = 0;
        config.Gateway[0] = 0; config.Gateway[1] = 0; config.Gateway[2] = 0; config.Gateway[3] = 0;
        config.ntpServerName = "cn.pool.ntp.org";
        config.Update_Time_Via_NTP_Every = 0;
        config.timezone = -10;
        config.daylight = true;
        config.DeviceName = "ESPMega";
        config.AutoTurnOff = false;
        config.AutoTurnOn = false;
        config.TurnOffHour = 0;
        config.TurnOffMinute = 0;
        config.TurnOnHour = 0;
        config.TurnOnMinute = 0;
        config.ota = false;
        config.mqtt_server_domain = "local";
        config.mqtt_user = "";
        config.mqtt_password = "";
        config.mqtt_topic = "esp/answer";
        config.mqtt_subtopic = "/data";
        config.mqtt_sub = "home/command/esp12s/mega";
        WriteConfig();
        Serial.println("General config");
        return;
    }
}

/*
**
**  NTP
**
*/

const int NTP_PACKET_SIZE = 48;
byte packetBuffer[NTP_PACKET_SIZE];
void NTPRefresh()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        IPAddress timeServerIP;
        WiFi.hostByName(config.ntpServerName.c_str(), timeServerIP);
        Serial.println("sending NTP packet...");
        memset(packetBuffer, 0, NTP_PACKET_SIZE);
        packetBuffer[0] = 0b11100011;   // LI, Version, Mode
        packetBuffer[1] = 0;     // Stratum, or type of clock
        packetBuffer[2] = 6;     // Polling Interval
        packetBuffer[3] = 0xEC;  // Peer Clock Precision
        packetBuffer[12] = 49;
        packetBuffer[13] = 0x4E;
        packetBuffer[14] = 49;
        packetBuffer[15] = 52;
        UDPNTPClient.beginPacket(timeServerIP, 123);
        UDPNTPClient.write(packetBuffer, NTP_PACKET_SIZE);
        UDPNTPClient.endPacket();


        delay(1000);

        int cb = UDPNTPClient.parsePacket();
        if (!cb) {
            Serial.println("NTP no packet");
        }
        else
        {
            Serial.println("NTP received");
            UDPNTPClient.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
            unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
            unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
            unsigned long secsSince1900 = highWord << 16 | lowWord;
            const unsigned long seventyYears = 2208988800UL;
            unsigned long epoch = secsSince1900 - seventyYears;
            UnixTimestamp = epoch;
        }
    }
}

void Second_Tick() {
    strDateTime tempDateTime;
    AdminTimeOutCounter++;
    cNTP_Update++;
    UnixTimestamp++;
    ConvertUnixTimeStamp(UnixTimestamp + (config.timezone * 360), &tempDateTime);
    if (config.daylight)
        if (summertime(tempDateTime.year, tempDateTime.month, tempDateTime.day, tempDateTime.hour, 0))
        {
            ConvertUnixTimeStamp(UnixTimestamp + (config.timezone * 360) + 3600, &DateTime);
        }
        else
        {
            DateTime = tempDateTime;
        }
    else
    {
        DateTime = tempDateTime;
    }
    Refresh = true;
}


#endif
