#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
WiFi.mode(WIFI_AP_STA); // explicitly set mode, esp defaults to STA+AP  
    // it is a good practice to make sure your code sets wifi mode how you want it.
   
    //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wm;

    // reset settings - wipe stored credentials for testing
    // these are stored by the esp library
//wm.resetSettings();

    // Automatically connect using saved credentials,
    // if connection fails, it starts an access point with the specified name ( "AutoConnectAP"),
    // if empty will auto generate SSID, if password is blank it will be anonymous AP (wm.autoConnect())
    // then goes into a blocking loop awaiting configuration and will return success result

    bool res;
//res = wm.autoConnect(); // auto generated AP name from chipid   //192.168.4.1 avec esp id
res = wm.autoConnect("AutoConnectAP"); // anonymous ap      // 192.168.4.1  c'est la que l'on peut donner un nouveau reseau
//res = wm.autoConnect("AutoConnectAP","ppassword"); // password protected ap on ne rempli le password qu'a la 1er connection

    if(!res) {
        Serial.println("Failed to connect");
        // ESP.restart();
    } 
    else {
        //if you get here you have connected to the WiFi    
        Serial.println("connected...yeey :)");

    }
