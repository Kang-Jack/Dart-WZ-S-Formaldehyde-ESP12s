#include <SoftwareSerial.h>

#include <Wire.h>
#include "SSD1306.h"

const int pinSDA = D2;
const int pinSCL = D1;
SSD1306  display(0x3c, pinSDA, pinSCL);


#define PLUGIN_200
#define PLUGIN_ID_200 200
#define PLUGIN_NAME_200 "Formaldehyde - Dart WZ-S"
#define PLUGIN_VALUENAME1_200 "ppb"
#define PLUGIN_VALUENAME2_200 "ug/m³"
#define PLUGIN_VALUENAME3_200 "mg/m³"
#define WZ_S_START_TAG 0Xff
#define WZ_S_SIZE 9

//SoftwareSerial *swSerial = NULL;
boolean Plugin_200_init = false;
float Plugin_200_last_value = -1;
int Plugin_200_ticks = 0;
int rxPin = D4;
int txPin = D5;

SoftwareSerial swSerial(rxPin,txPin);//  // RX, TX 90 Bytes buffer, enough for up to 10 packets..

const int delayTime = 2000; 
const int waittingSec = 3;
bool isProMode = false;

void init_oled() {
  display.init();
  display.flipScreenVertically();
  display.clear();
  display.setColor(WHITE);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_24);
  display.drawString(0, 0, "Init OLED");
}

void set_s_text(){
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);;
}
void set_m_text(){
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_16);;
}
void set_b_text(){
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_24);
}

void new_screen_oled(int x,int y,char* mesg){
  display.clear();
  display.drawString(x, y, mesg);
  display.display();
  delay(1000);
}

void display_CHCO(float value,bool is_ppb){
    display.clear();
    set_m_text();
    if(is_ppb)
    {
        display.drawString(0, 0,"CHCO(ppb)");
    }
    else
    {
       display.drawString(0, 0,"CHCO(mg/m3)");
      }
    set_b_text();
    display.drawString(0,22,String(value));
    display.display();
    delay(delayTime);
}

void setup() {
    Wire.pins(pinSDA, pinSCL);
    Serial.begin(9600);
    Serial.flush();
    
    swSerial.begin(9600);
    swSerial.flush();
    Plugin_200_init = true;
    Plugin_200_last_value = -1;
    Plugin_200_ticks = 0;

    init_oled();
    new_screen_oled(0,0,"Starting ESP12S");
    
}
void loop() {
    boolean success = false;
    Plugin_200_ticks++;
    if (Plugin_200_PacketAvailable())
    {
        success = Plugin_200_process_data();
    }
    if (success)
    {
        Serial.println("done");
    }
}
void Plugin_200_SerialFlush() {
    if (swSerial != NULL) {
        swSerial.flush();
    }
    else {
        Serial.flush();
    }
}

boolean Plugin_200_PacketAvailable(void)
{
    if (swSerial != NULL) // Software serial
    {
        // When there is enough data in the buffer, search through the buffer to
        // find header (buffer may be out of sync)
        if (!swSerial.available()) return false;
        while ((swSerial.peek() != WZ_S_START_TAG) && swSerial.available()) {
            swSerial.read(); // Read until the buffer starts with the first byte of a message, or buffer empty.
        }
        if (swSerial.available() < WZ_S_SIZE) return false; // Not enough yet for a complete packet
    }
    else // Hardware serial
    {
        //// When there is enough data in the buffer, search through the buffer to
        //// find header (buffer may be out of sync)
        //if (!Serial.available()) return false;
        //while ((Serial.peek() != WZ_S_START_TAG) && Serial.available()) {
        //    Serial.read(); // Read until the buffer starts with the first byte of a message, or buffer empty.
        //}
        //if (Serial.available() < WZ_S_SIZE) return false; // Not enough yet for a complete packet
    }
    return true;
}

boolean Plugin_200_process_data() {
    String log;
    uint8_t data[WZ_S_SIZE];
    if (swSerial != NULL)
    {
        for (int i = 0; i < WZ_S_SIZE; i++){
            data[i] = swSerial.read();
            Serial.println(data[i]);
        }
    }
    else
    {
        /* for (int i = 0; i < WZ_S_SIZE; i++)
             data[i] = Serial.read();*/
    }
    uint8_t checksum = 0;
    for (int i = 1; i < WZ_S_SIZE - 1; i++)
        checksum += data[i];

    checksum = (~checksum) + 1;

    if (checksum != data[WZ_S_SIZE - 1])
    {
        log = F("WZ-S : checksum error.");
        Serial.println(log);
        //addLog(LOG_LEVEL_ERROR, log);
        return false;
    }

    //uint8_t  high = data[2];
    //uint8_t  low = data[3];
    //float r1 = high * 256.0 + low;
    // ignored if the value has not changed and reported time is less than 1 minute

    float r1  = (float)(data[4] << 8 | data[5]);
    

    //float r2 = (float)(data[4] << 8 | data[5]);   //求出ppb值
    float r2 = r1/1000.0 * 1.3393;
    Plugin_200_SerialFlush(); // Make sure no data is lost due to full buffer.
    if (!(r1 != Plugin_200_last_value || Plugin_200_ticks >= 600))
    {
        return false;
    }
    Plugin_200_last_value = r1;
    Plugin_200_ticks = 0;
    //float r2 = r1 * 30.03 / 22.4;
    //float r3 = r2 / 1000.0;
    display_CHCO(r1,true);
    display_CHCO(r2,false);
    log = F("WZ-S : ");
    //log += r1;
    //log += F("ppb, ");
    log += r1;
    log += F("ppb, ");
    log += r2;
    log += F("mg/m³(calc) ");
    //log += r3;
    //log += F("mg/m³, ");
    Serial.println(log);
    return true;
}
