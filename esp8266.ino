#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "fauxmoESP.h"
#include <ESP8266WebServer.h>
#include <EEPROM.h>

fauxmoESP fauxmo;

// -----------------------------------------------------------------------------

#define SERIAL_BAUDRATE 115200
#define DEVICE "Password"

ESP8266WebServer server(3000);

struct settings {
  char ssid[30];
  char password[30];
  char computerpass[30];
} user_wifi = {};

void wifiSetup() {

    // Set WIFI module to STA mode
    WiFi.mode(WIFI_STA);

    // Connect
    WiFi.begin(user_wifi.ssid, user_wifi.password);
    int tries = 0;
    IPAddress IP ;
    
    // Wait
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      IP = WiFi.localIP();
      if (tries++ > 15) {
        WiFi.mode(WIFI_AP);
        WiFi.softAP("Setup Portal Device", "zigma123");
        IP=WiFi.softAPIP();
        break;
      }
    }
    
    Serial.println("Pagina de Configuracion en ");
    Serial.print(IP);
    Serial.println(":3000" );
}

void setup() {

    // Init serial port and clean garbage
    Serial.begin(SERIAL_BAUDRATE);
    
    //Check EEPROM
    EEPROM.begin(sizeof(struct settings) );
    EEPROM.get( 0, user_wifi );
    
    // Wifi
    wifiSetup();
    
    // set up Fauxmo
    fauxmo.createServer(true); // not needed, this is the default value
    fauxmo.setPort(80); // This is required for gen3 devices
    fauxmo.enable(true);
    fauxmo.addDevice(DEVICE);
    fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {
        if (strcmp(device_name, DEVICE)==0) {
          Serial.println(user_wifi.computerpass);
          if(state){
            delay(500);
            fauxmo.setState(DEVICE, false, 255);
          }
        }

    });

    server.on("/",  handlePortal);
    server.begin();
}

void(* resetFunc) (void) = 0; //declare reset function @ address 0

void handlePortal() {
  if (server.method() == HTTP_POST) {
    strncpy(user_wifi.ssid,     server.arg("ssid").c_str(),     sizeof(user_wifi.ssid) );
    strncpy(user_wifi.password, server.arg("password").c_str(), sizeof(user_wifi.password) );
    strncpy(user_wifi.computerpass, server.arg("computerpass").c_str(), sizeof(user_wifi.computerpass) );
    user_wifi.ssid[server.arg("ssid").length()] = user_wifi.password[server.arg("password").length()] =user_wifi.computerpass[server.arg("computerpass").length()] = '\0';
    EEPROM.put(0, user_wifi);
    EEPROM.commit();
    server.send(200,   "text/html",  "<!doctype html><html lang='en'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'><title>Wifi Setup</title><style>*,::after,::before{box-sizing:border-box;}body{margin:0;font-family:'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans';font-size:1rem;font-weight:400;line-height:1.5;color:#212529;background-color:#f5f5f5;}.form-control{display:block;width:100%;height:calc(1.5em + .75rem + 2px);border:1px solid #ced4da;}button{border:1px solid transparent;color:#fff;background-color:#007bff;border-color:#007bff;padding:.5rem 1rem;font-size:1.25rem;line-height:1.5;border-radius:.3rem;width:100%}.form-signin{width:100%;max-width:400px;padding:15px;margin:auto;}h1,p{text-align: center}</style> </head> <body><main class='form-signin'> <h1>Wifi Setup</h1> <br/> <p>Your settings have been saved successfully!<br />Please restart the device.</p></main></body></html>" );
    delay(5000);
    resetFunc();  //call reset
  } else {
    server.send(200,   "text/html", "<!doctype html><html lang='en'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'><title>Wifi Setup</title> <style>*,::after,::before{box-sizing:border-box;}body{margin:0;font-family:'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans';font-size:1rem;font-weight:400;line-height:1.5;color:#212529;background-color:#f5f5f5;}.form-control{display:block;width:100%;height:calc(1.5em + .75rem + 2px);border:1px solid #ced4da;}button{cursor: pointer;border:1px solid transparent;color:#fff;background-color:#007bff;border-color:#007bff;padding:.5rem 1rem;font-size:1.25rem;line-height:1.5;border-radius:.3rem;width:100%}.form-signin{width:100%;max-width:400px;padding:15px;margin:auto;}h1{text-align: center}</style> </head> <body><main class='form-signin'> <form action='/' method='post'> <h1 class=''>Wifi Setup</h1><br/><div class='form-floating'><label>Nombre Red</label><input type='text' class='form-control' name='ssid' value='"+String(user_wifi.ssid)+"'> </div><div class='form-floating'><br/><label>Clave WIFI</label><input type='password' class='form-control' name='password' value='"+String(user_wifi.password)+"'></div><div class='form-floating'><br/><label>Clave Computador</label><input type='text' class='form-control' name='computerpass' value='"+String(user_wifi.computerpass)+"'></div><br/><br/><button type='submit'>Save</button></form></main> </body></html>" );
  }
}

void loop() {
    server.handleClient();
    fauxmo.handle();
}
