#define ACCESS_POINT_NAME  "WemosD1R2"
#define ACCESS_POINT_PASSWORD  "12345678"
int AdminTimeOutCounter = 0;                  // Counter for Disabling the AdminMode
boolean OTAEnabled = false;
boolean AdminEnabled = true;    // Enable Admin Mode for a given Time
boolean WifiEnabled = true;
struct strConfig {
    String ssid;
    String password;
    byte  IP[4];
    byte  MqttIP[4];
    long  MqttPort;
    byte  Netmask[4];
    byte  Gateway[4];
    boolean dhcp;
    String ntpServerName;
    long Update_Time_Via_NTP_Every;
    long timezone;
    boolean daylight;
    String DeviceName;
    boolean AutoTurnOff;
    boolean AutoTurnOn;
    byte TurnOffHour;
    byte TurnOffMinute;
    byte TurnOnHour;
    byte TurnOnMinute;

    boolean ota;
    String mqtt_topic;
    String mqtt_subtopic;
    String mqtt_sub;
    String mqtt_user;
    String mqtt_password;
    String mqtt_server_domain;
    String mqtt_pub_topic;
}   config;

static const uint8_t monthDays[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
#define LEAP_YEAR(Y) ( ((1970+Y)>0) && !((1970+Y)%4) && ( ((1970+Y)%100) || !((1970+Y)%400) ) )

struct  strDateTime{
    byte hour;
    byte minute;
    byte second;
    int year;
    byte month;
    byte day;
    byte wday;

};

