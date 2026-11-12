
/*https://github.com/Raeymeister/BelgianSmartMeter2Domoticz/blob/master/BelgianSmartMeterToDomoticz.ino*/
#include <WiFiManager.h>  // https://github.com/tzapu/WiFiManager
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include <ESPmDNS.h>
#include <ElegantOTA.h>

WebServer server(80);

#define RXD2 16  //RXD2 16  //ESP32 firebeetle
#define TXD2 17  //TXD2 17  //ESP32 firebeetle
#include <WiFiUdp.h>
WiFiClient wifiClient;

#include "CRC16.h"

const char* ssid = "****";
const char* password = "*****";
const bool outputOnSerial = true;

String raw_data;

#define MAXLINELENGTH 4096  // longest normal line is 47 char (+3 for \r\n\0)
char telegram[MAXLINELENGTH];

unsigned long currentTime = 0;
unsigned long lastTime = 0;
unsigned int currentCRC = 0;
const int rts = 4;  //declaration
///////////////////////////////////////
void setup() {
  Serial.begin(115200);
  Serial.println("Booting");

#include "connect.h"

  server.on("/test", test);

  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2, true);  // le true est pour inverser la polarité

  ElegantOTA.begin(&server);  // Start ElegantOTA
  Serial.println("Setup complete!");
  server.begin();
  pinMode(rts, OUTPUT);    // in setup pour rts
  digitalWrite(rts, HIGH);  // pour rts
  raw_data = "";
    readTelegram();
  digitalWrite(rts, LOW);  
}

/////////////////////////////////////////////
void test() {
  server.send(200, "text/plain", raw_data);  //sValue);
  delay(10);
raw_data="";
}
///////////////////////////////////////////
bool isNumber(char* res, int len) {
  for (int i = 0; i < len; i++) {
    if (((res[i] < '0') || (res[i] > '9')) && (res[i] != '.' && res[i] != 0)) {
      return false;
    }
  }
  return true;
}
///////////////////////////////////////////
int FindCharInArrayRev(char array[], char c, int len) {
  for (int i = len - 1; i >= 0; i--) {
    if (array[i] == c) {
      return i;
    }
  }
  return -1;
}
/////////////////////////////////////////////////
long getValue(char* buffer, int maxlen) {
  int s = FindCharInArrayRev(buffer, '(', maxlen - 2);
  if (s < 8) return 0;
  if (s > 32) s = 32;
  int l = FindCharInArrayRev(buffer, '*', maxlen - 2) - s - 1;
  if (l < 4) return 0;
  if (l > 12) return 0;
  char res[16];
  memset(res, 0, sizeof(res));
  raw_data += "370";
  if (strncpy(res, buffer + s + 1, l)) {
    raw_data += "370";
    raw_data += res;
    if (isNumber(res, l)) {
      return (1000 * atof(res));
    }
  }
  return 0;
}
///////////////////////////////////////////////
long getValueWithoutStar(char* buffer, int maxlen) {
  //mess=buffer;
  int s = FindCharInArrayRev(buffer, '(', maxlen - 2);
  if (s < 8) return 0;
  int e = FindCharInArrayRev(buffer, ')', maxlen - 2);
  if (e < 0 || e <= s) return 0;
  int l = e - s - 1;
  char res[16];
  memset(res, 0, sizeof(res));
  if (strncpy(res, buffer + s + 1, l)) {
     if (isNumber(res, l)) {
      return atof(res);
    }
  }
  return 0;
}
///////////////////////////////////////////
bool decodeTelegram(int len) {
  //need to check for start
  int startChar = FindCharInArrayRev(telegram, '/', len);
  int endChar = FindCharInArrayRev(telegram, '!', len);
  bool validCRCFound = false;
  if (startChar >= 0) {
    //start found. Reset CRC calculation
    currentCRC = CRC16(0x0000, (unsigned char*)telegram + startChar, len - startChar);
    if (outputOnSerial) {
      for (int cnt = startChar; cnt < len - startChar; cnt++)
        Serial.print(telegram[cnt]);
    }
    //Serial.println("Start found!");
  } else if (endChar >= 0) {
    //add to crc calc
    currentCRC = CRC16(currentCRC, (unsigned char*)telegram + endChar, 1);
    char messageCRC[5];
    strncpy(messageCRC, telegram + endChar + 1, 4);
    messageCRC[4] = 0;  //thanks to HarmOtten (issue 5)
    if (outputOnSerial) {
      for (int cnt = 0; cnt < len; cnt++)
        Serial.print(telegram[cnt]);
    }

    validCRCFound = (strtol(messageCRC, NULL, 16) == currentCRC);
    if (validCRCFound)
      Serial.println("\nVALID CRC FOUND!");
    else
      Serial.println("\n===INVALID CRC FOUND!===");
    currentCRC = 0;
  } else {
    currentCRC = CRC16(currentCRC, (unsigned char*)telegram, len);
    if (outputOnSerial) {
      for (int cnt = 0; cnt < len; cnt++)
        Serial.print(telegram[cnt]);
    }
  }
  return validCRCFound;
}
//////////////////////////////////////
void readTelegram() {
  if (Serial2.available()) {
    memset(telegram, 0, sizeof(telegram));
    while (Serial2.available()) {
      int len = Serial2.readBytesUntil('\n', telegram, MAXLINELENGTH);
      telegram[len] = '\n';
      telegram[len + 1] = 0;
      yield();
      if (decodeTelegram(len + 1)) {
      }
      raw_data += telegram;
    }
  }
}
/////////////////////////////////////
void loop() {
  server.handleClient();
  ElegantOTA.loop();  // ArduinoOTA.handle();
  
}