
/*https://github.com/Raeymeister/BelgianSmartMeter2Domoticz/blob/master/BelgianSmartMeterToDomoticz.ino*/
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include <ESPmDNS.h>
#include <ElegantOTA.h>

WebServer server(80);

#define RXD2 16  //RXD2 16  //ESP32 firebeetle
#define TXD2 17   //TXD2 17  //ESP32 firebeetle

WiFiClient wifiClient;

#include "CRC16.h"

//===Change values from here===
const char* ssid = "****";
const char* password = "****";
const bool outputOnSerial = true;
//===Change values to here===
  char sValue[512];   //255];
  String Teleg;
  String dest;  //pour conversion char to string
// Vars to store meter readings
long Cons_Provider = 0;
long Prod_Local = 0;
long ETAR = 0;
//long MEID = 0;
long MESN = 0;
String METS;
long ECLT = 0; 
long ECHT = 0; 
long ERLT = 0; 
long ERHT = 0; 
long EAC = 0;   
long EAR = 0;  
long EL1V = 0;
long EL2V = 0;
long EL3V = 0;
long EL1R = 0;
long EL2R = 0;
long EL3R = 0;
long EL1I = 0;
long EL2I = 0;
long EL3I = 0;
long EL1C = 0;
long EL2C = 0;
long EL3C = 0;
long ETPC = 0;
//String ETPC;

#define MAXLINELENGTH 4096 // longest normal line is 47 char (+3 for \r\n\0)
char telegram[MAXLINELENGTH];

unsigned int currentCRC=0;

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
 
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

if (!MDNS.begin("p1")) {
      Serial.println("Error setting up MDNS responder!");
    } else {
      Serial.printf("mDNS responder started '%s'\n", "p1.local");
    }

 server.on("/p1_data", data_web);
 server.on("/html", html);

    Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2, true);   // le true est pour inverser la polarité

    ElegantOTA.begin(&server);    // Start ElegantOTA
  Serial.println("Setup complete!");
  server.begin();
}
/////////////////////////////////////////////////
void data_web() {
  server.send(200, "text/plain", Teleg);
}
////////////////////////////////////////////////
void html() {
////  UpdateValues();
String html = "<!DOCTYPE html><html><head><title>ESP32 P1_Meter</title></head><body>";
html += "<h4>ESP32 P1_Meter</h4>";
html += "<table border='1'>";
// <!-- Ligne 1 (souvent l'en-tête) -->
html += "<tr>";    
html += "<th>p1 consommation </th>";   
html += "<th>p1 production</th>";    
html += "<th> autres infos </th>";  
html += "</tr>";
//  <!-- Ligne 2 ) -->
html += "<tr>"; 
html += "<td>consommation total :" + String(Cons_Provider) + " kW &nbsp; </td>"; 
html += "<td>production total :" + String(Prod_Local) + " kW &nbsp; </td>";  
html += "<td> tarif 1 / 2  : " + String(ETAR) + " &nbsp; </td>";
html += "</tr>";  
//  <!-- Ligne 3 ) -->
html += "<tr>";
html += "<td>Conso. provider tarif 1 ECLT : " + String(ECLT/1000) +" kW &nbsp; </td>";    ///Ligne 3 - Cellule 1&nbsp; </td>";
html += "<td>Production tarif 1 ERLT : " + String(ERLT/1000) +" kW  &nbsp; </td>";  
//html += "<td>Tarif 1=haut/2=bas ETAR : " + String(ETAR) +"&nbsp; </td>"; 
html += "<td>Timestamp METS : " + String(METS) +" &nbsp; </td>"; 
html += "</tr>";
//  <!-- Ligne 4 ) -->
html += "<tr>";
html += "<td>Conso. provider tarif 2 ECHT : " + String(ECHT/1000) +" kW   &nbsp; </td>";  
html += "<td>Production tarif 2 ERHT : " + String(ERHT/1000) +" kW   &nbsp; </td>"; 
html += "<td>Numero de serie MESN : " + String(MESN) +" &nbsp; </td>"; 
html += "</tr>";
html += "<tr>";
html += "</tr>";  
//  <!-- Ligne 5 ) -->
html += "<tr>";
html += "<td>Tension L1 El1V : " + String(EL1V/1000) + " V&nbsp; </td>";  
html += "<td>Production L1 EL1R : " + String(EL1R) +" W&nbsp; </td>";  
html += "<td>   &nbsp; </td>"; 
html += "</tr>"; 
//  <!-- Ligne 6 ) -->
html += "<tr>";
html += "<td>Tension L2 El2V : " + String(EL2V/1000) +" V&nbsp; </td>";  
html += "<td>Production L2 EL2R : " + String(EL2R) +" W&nbsp; </td>";  
html += "<td>   &nbsp; </td>"; 
html += "</tr>";
html += "<tr>";  
//  <!-- Ligne 7 ) -->
html += "<td>Tension L3 El3V : " + String(EL3V/1000) +" V&nbsp; </td>";  
html += "<td>Production L3 EL3R : " + String(EL3R) +" W&nbsp; </td>";  
html += "<td>    &nbsp; </td>"; 
html += "</tr>";
//  <!-- Ligne 8 ) -->
html += "<tr>";
html += "</tr>";  
html += "<tr>";  
html += "<td>Courant L1 El1C : " + String(EL1C) + " W&nbsp; </td>";  
html += "<td>Intensite L1 El1I : " + String(EL1I) + " W&nbsp; </td>";  
html += "<td>    &nbsp; </td>"; 
html += "</tr>";
//  <!-- Ligne 9 ) -->
html += "<tr>";  
html += "<td>Courant L2 El2C : " + String(EL2C) + " W&nbsp; </td>";  
html += "<td>Intensite L2 El2I : " + String(EL2I) + " W&nbsp; </td>"; 
html += "<td>    &nbsp; </td>"; 
html += "</tr>";
//  <!-- Ligne 10 ) -->
html += "<tr>";  
html += "<td>Courant L3 El1C : " + String(EL3C) + " W&nbsp; </td>";  
html += "<td>Intensite L3 El3I : " + String(EL3I) + " W&nbsp; </td>"; 
html += "<td>    &nbsp; </td>"; 
html += "</tr>";
html += "<tr>";
//  <!-- Ligne 11 ) -->
html += "</tr>";  
html += "<tr>";  
html += "<td>Puissance actuelle conso. EAC :" + String(EAC) +" W  &nbsp; </td>"; 
html += "<td>Puissance produite EAR : " + String(EAR) +" W  &nbsp; </td>";  
html += "<td>    &nbsp; </td>"; 
html += "</tr>";
//  <!-- Ligne 12 ) -->
html += "<tr>";  
html += "<td>    &nbsp; </td>"; 
html += "<td>    &nbsp; </td>";  
html += "<td>    &nbsp; </td>"; 
html += "</tr>";

html += "</table>";
html += "<p>&nbsp;&nbsp;&nbsp;&nbsp; Groupe-e tarif 1 haut <-> 7.00 a 12:00 / 17:00 a 23:00 &nbsp;&nbsp;&nbsp;&nbsp;  tarif 2 bas <-> 23:00 a 7:00 12:00 / 17:00</p>";
html += "</body></html>";
  server.send(200, "text/html", html);
}
/////////////////////////////////////////////7
void UpdateElectricity()
{ Teleg += "Production et Consomation\n";
  Cons_Provider = (ECLT + ECHT) / 1000;
  Teleg += "Cons_Provider :";
  Teleg += Cons_Provider;
  Teleg += "  kW\n";
  Prod_Local = (ERLT + ERHT) / 1000;
   Teleg += "Prod_Local :";
  Teleg += Prod_Local;
  Teleg += "  kW\n";
}

bool isNumber(char* res, int len) {
  for (int i = 0; i < len; i++) {
    if (((res[i] < '0') || (res[i] > '9'))  && (res[i] != '.' && res[i] != 0)) {
      return false;
    }
  }
  return true;
}

int FindCharInArrayRev(char array[], char c, int len) {
  for (int i = len - 1; i >= 0; i--) {
    if (array[i] == c) {
      return i;
    }
  }
  return -1;
}

long getValue(char* buffer, int maxlen) {
  int s = FindCharInArrayRev(buffer, '(', maxlen - 2);
  if (s < 8) return 0;
  if (s > 32) s = 32;
  int l = FindCharInArrayRev(buffer, '*', maxlen - 2) - s - 1;
  if (l < 4) return 0;
  if (l > 12) return 0;
  char res[16];
  memset(res, 0, sizeof(res));

  if (strncpy(res, buffer + s + 1, l)) {
    if (isNumber(res, l)) {
      return (1000 * atof(res));
    }
  }
  return 0;
}

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

bool decodeTelegram(int len) {
  //need to check for start
  int startChar = FindCharInArrayRev(telegram, '/', len);
  int endChar = FindCharInArrayRev(telegram, '!', len);
  bool validCRCFound = false;
  if(startChar>=0)
  {
    //start found. Reset CRC calculation
    currentCRC=CRC16(0x0000,(unsigned char *) telegram+startChar, len-startChar);
    if(outputOnSerial)
    {
      for(int cnt=startChar; cnt<len-startChar;cnt++)
        Serial.print(telegram[cnt]);
    }    
    //Serial.println("Start found!");
  }
  else if(endChar>=0)
  {
    //add to crc calc 
    currentCRC=CRC16(currentCRC,(unsigned char*)telegram+endChar, 1);
    char messageCRC[5];
    strncpy(messageCRC, telegram + endChar + 1, 4);
    messageCRC[4]=0; //thanks to HarmOtten (issue 5)
    if(outputOnSerial)
    {
      for(int cnt=0; cnt<len;cnt++)
        Serial.print(telegram[cnt]);
    }    
    validCRCFound = (strtol(messageCRC, NULL, 16) == currentCRC);
    if(validCRCFound)
      Serial.println("\nVALID CRC FOUND!"); 
    else
      Serial.println("\n===INVALID CRC FOUND!===");
    currentCRC = 0;
  }
  else
  {
    currentCRC=CRC16(currentCRC, (unsigned char*)telegram, len);
    if(outputOnSerial)
    {
      for(int cnt=0; cnt<len;cnt++)
        Serial.print(telegram[cnt]);
    }
  }
Teleg = "";
  long val =0;
  long val2=0;

  if (strncmp(telegram, "0-0:96.14.0", strlen("0-0:96.14.0")) == 0)  //Tarif haut / bas  ok
    ETAR = getValueWithoutStar(telegram, len);
    Teleg += "ETAR: ";
    Teleg += ETAR;
    Teleg +=" \n";

 //   ETAR = getValue(telegram, len);
  if (strncmp(telegram, "0-0:96.1.1", strlen("0-0:96.1.1")) == 0)   //Serial number meter
    MESN = getValueWithoutStar(telegram, len);
    Teleg += "MESN: ";
    Teleg += MESN;
    Teleg +=" \n";    

  if (strncmp(telegram, "0-0:1.0.0", strlen("0-0:1.0.0")) == 0)
    METS = telegram;  //getValueWithoutStar(telegram, len);  //n //mest OK
    Teleg += "METS: ";
    String data = METS.substring(10, METS.length());  //mise en form pour p1_data
    Teleg += data;  //METS;
    Teleg +=" \n";    
  Teleg += "Consomation haut et bas tarif\n";

  // 1-0:1.8.1 = Elektra verbruik dagtarief (Fluvius) - Totale afname van energie in kWh dagtarief
  if (strncmp(telegram, "1-0:1.8.1", strlen("1-0:1.8.1")) == 0) 
    ECLT =  getValue(telegram, len);
      Teleg += "ECLT: ";
      Teleg += ECLT;
      Teleg +="  W \n";

  // 1-0:1.8.2 = Elektra verbruik nachttarief (Fluvius) - Totale afname van energie in kWh nachttarief
  if (strncmp(telegram, "1-0:1.8.2", strlen("1-0:1.8.2")) == 0) 
    ECHT = getValue(telegram, len);
      Teleg += "ECHT :"; 
      Teleg += ECHT;
      Teleg +="  W \n";
      Teleg += "Production haut et bas tarif\n";
  // 1-0:2.8.1 = Elektra opbrengst dagtarief (Fluvius) - Totale injectie van energie in kWh dagtarief
  if (strncmp(telegram, "1-0:2.8.1", strlen("1-0:2.8.1")) == 0) 
    ERLT = getValue(telegram, len);
      Teleg += "ERLT :";
      Teleg += ERLT;
      Teleg +="  W \n";

  // 1-0:2.8.2 = Elektra opbrengst nachttarief (Fluvius) - Totale injectie van energie in kWh nachttarief
  if (strncmp(telegram, "1-0:2.8.2", strlen("1-0:2.8.2")) == 0) 
    ERHT = getValue(telegram, len);
      Teleg += "ERHT: ";
      Teleg += ERHT;
      Teleg +="  W \n"; 
     Teleg += "Consomation actuel\n";
  // 1-0:1.7.x = Electricity consumption actual usage (DSMR v4.0)
  if (strncmp(telegram, "1-0:1.7.0", strlen("1-0:1.7.0")) == 0) 
    EAC = getValue(telegram, len);
      Teleg += "EAC: ";
      Teleg += EAC;
      Teleg +="  W \n";
 
  if (strncmp(telegram, "1-0:2.7.0", strlen("1-0:2.7.0")) == 0)
    EAR = getValue(telegram, len);
      Teleg += "EAR: ";
      Teleg += EAR;
      Teleg +="  W \n";
      Teleg += "Tension reseau\n";
  if (strncmp(telegram, "1-0:32.7.0", strlen("1-0:32.7.0")) == 0)
    EL1V = getValue(telegram, len);
      Teleg += "EL1V: ";
      Teleg += EL1V/1000;
      Teleg +=" V; \n";
  if (strncmp(telegram, "1-0:52.7.0", strlen("1-0:52.7.0")) == 0)
    EL2V = getValue(telegram, len);
      Teleg += "EL2V:";
      Teleg += EL2V/1000;
      Teleg +=" V; \n";
  if (strncmp(telegram, "1-0:72.7.0", strlen("1-0:72.7.0")) == 0)
    EL3V = getValue(telegram, len);
      Teleg += "EL3V:";
      Teleg += EL3V/1000;  
      Teleg +=" V; \n";
      Teleg += "Puissance produite\n";
    if (strncmp(telegram, "1-0:22.7.0", strlen("1-0:22.7.0")) == 0)   //insertion Watts L1 prod
    EL1R = getValue(telegram, len);
        Teleg += "EL1R: ";
        Teleg += EL1R;
        Teleg +="  W \n";
  if (strncmp(telegram, "1-0:42.7.0", strlen("1-0:42.7.0")) == 0)   //insertion Watts L2 prod
    EL2R = getValue(telegram, len);
        Teleg += "EL2R: ";
        Teleg += EL2R;
        Teleg +="  W \n";

  if (strncmp(telegram, "1-0:62.7.0", strlen("1-0:62.7.0")) == 0)   //insertion Watts L3 prod
    EL3R = getValue(telegram, len);
        Teleg += "EL3R: ";
        Teleg += EL3R;
        Teleg +="  W \n";
        Teleg += "Consomation de pointe moyenne\n";

  if (strncmp(telegram, "1-0:2.6.0", strlen("1-0:2.6.0")) == 0)     //1-0:1.6.0", strlen("1-0:1.6.0")) == 0)   //consomation de pointe moyenne
    ETPC = getValue(telegram, len); 
        Teleg += "ETPC: ";
        Teleg += ETPC;   
        Teleg +="  W \n";
        Teleg += "Puissance soutire le mois en cours\n";
    
/*
  if (strncmp(telegram, "1-0:2.6.0", strlen("1-0:2.6.0")) == 0)
    ETPC = telegram;  //getValueWithoutStar(telegram, len);  //n //mest OK
    Teleg += "ETPC: ";
    String data1 = ETPC; //.substring(10, ETPC.length());  //mise en form pour p1_data
    Teleg += data1;  //METS;
    Teleg +=" \n";    
*/
if (strncmp(telegram, "1-0:31.7.0", strlen("1-0:31.7.0")) == 0)
    EL1I = getValue(telegram, len);
  Teleg += "EL1I: ";
  Teleg += EL1I;
  Teleg += "  A \n";
if (strncmp(telegram, "1-0:51.7.0", strlen("1-0:51.7.0")) == 0)
    EL2I = getValue(telegram, len);
  Teleg += "EL2I: ";
  Teleg += EL2I;
  Teleg += "  A \n";

if (strncmp(telegram, "1-0:71.7.0", strlen("1-0:71.7.0")) == 0)
    EL3I = getValue(telegram, len);
  Teleg += "EL3I: ";
  Teleg += EL3I;
  Teleg += "  A \n";
  Teleg += "Courant consomme\n";
if (strncmp(telegram, "1-0:21.7.0", strlen("1-0:21.7.0")) == 0)
    EL1C = getValue(telegram, len);
    Teleg += "EL1C: ";
  Teleg += EL1C;
  Teleg += "  W \n";

if (strncmp(telegram, "1-0:41.7.0", strlen("1-0:41.7.0")) == 0)
    EL2C = getValue(telegram, len);
  Teleg += "EL2C: ";
  Teleg += EL2C;
  Teleg += "  W \n";

if (strncmp(telegram, "1-0:61.7.0", strlen("1-0:61.7.0")) == 0)
    EL3C = getValue(telegram, len);
  Teleg += "EL3C: ";
  Teleg += EL3C;
  Teleg += "  W \n";

  return validCRCFound;
}

void readTelegram() {
  if (Serial2.available()) {
    memset(telegram, 0, sizeof(telegram));
        while (Serial2.available()) {
      int len = Serial2.readBytesUntil('\n', telegram, MAXLINELENGTH);

      telegram[len] = '\n';
      telegram[len+1] = 0;
      yield();
      if(decodeTelegram(len+1))
      {
         UpdateElectricity();
      }
    } 
  }
}

void loop() {
        server.handleClient();
     ElegantOTA.loop();    

      readTelegram();
}