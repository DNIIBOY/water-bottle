/*
 * ESP32 AJAX Demo
 * Updates and Gets data from webpage without page refresh
 * https://circuits4you.comM
 */
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

#include "index.h"  //Web page header file

WebServer server(80);

//Enter your SSID and PASSWORD
const char* ssid = "Water Bottle";
const char* password = "123456789";


//===============================================================
// Setup for DS1820B Temp Sensor
//===============================================================
#include <OneWire.h>
#include <DallasTemperature.h>

// GPIO where the DS18B20 is connected to
const int oneWireBus = 4;  // 7 Pins up from bottom right.

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

//===============================================================
// Setup for Pressure Sensor
//===============================================================

int analogPressure = A0;
int pressureVal = 0;

//===============================================================
// This routine is executed when you open its IP in browser
//===============================================================
void handleRoot() {
 String s = MAIN_page; //Read HTML contents
 server.send(200, "text/html", s); //Send web page
}

void handleTemp() {
 sensors.requestTemperatures(); 
 float tempC = sensors.getTempCByIndex(0) - 0.5;
 String outVal = String(tempC);
 
 server.send(200, "text/plane", outVal); //Send output value only to client ajax request
}
 
void handlePressure() {
 int pressure = analogRead(A0);
 String outVal = String(pressure);
 
 server.send(200, "text/plane", outVal); //Send ADC value only to client ajax request
}

//===============================================================
// Setup
//===============================================================

void setup(void){
  Serial.begin(115200);
  Serial.println();
  Serial.println("Booting Sketch...");


  //ESP32 As access point
  WiFi.mode(WIFI_AP); //Access Point mode
  WiFi.softAP(ssid, password);

  // Start the DS18B20 Temp sensor
  sensors.begin();


/*
//ESP32 connects to your wifi -----------------------------------
  WiFi.mode(WIFI_STA); //Connectto your wifi
  WiFi.begin(ssid, password);

  Serial.println("Connecting to ");
  Serial.print(ssid);

  //Wait for WiFi to connect
  while(WiFi.waitForConnectResult() != WL_CONNECTED){      
      Serial.print(".");
    }
    
  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
//----------------------------------------------------------------
 */
  server.on("/", handleRoot);      //This is display page
  server.on("/readTemp", handleTemp);//To get update of Temp Value only
  server.on("/readPressure", handlePressure);//To get update of Temp Value only
 
  server.begin();                  //Start server
  Serial.println("HTTP server started");
}

//===============================================================
// This routine is executed when you open its IP in browser
//===============================================================
void loop(void){
  server.handleClient();
  delay(1);
}
