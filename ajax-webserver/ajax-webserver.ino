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

// Current water temperature.
float temp = 0;

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
// Setup for LED timer + temp indicator
//===============================================================

#include <TimedAction.h>
#include <Adafruit_NeoPixel.h>

#define LEDPin 1 // GPIO 1 for led strip

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 45

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LEDPin, NEO_GRB + NEO_KHZ800);

const int MAX_POWER = 64; // Max led pwoer

const int START_TIME = 300;  // Timer start time.
int timer = START_TIME; // 5 min
int color[3] = {0, 0, MAX_POWER};  // Color of temp RGB strip.

const int COLD_TEMP = 5; // Temp for full blue lights
const int HOT_TEMP = 20; // Temp for full red lights

// Seperate thread for time, to prevent pausing the system
void updateTimer(){
  if (timer > 0){
    timer--; 
  }
}
TimedAction counterThread = TimedAction(1000, updateTimer);

//===============================================================
// This routine is executed when you open its IP in browser
//===============================================================
void handleRoot() {
 String s = MAIN_page; //Read HTML contents
 server.send(200, "text/html", s); //Send web page
}

void handleTime() {
  int mins = floor(timer / 60);
  int secs = timer % 60;
  String outVal = "";
  if (mins > 0){
    outVal += String(mins) + ":";
  }
  if (secs < 10){
    outVal += "0" + String(secs);
  }
  else{
    outVal += String(secs);
  }
  server.send(200, "text/plane", outVal); //Send output value only to client ajax request
}

void handleTemp() {
 sensors.requestTemperatures(); 
 float temp = sensors.getTempCByIndex(0) - 0.5;
 String outVal = String(temp);

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
  Serial.begin(9600);
  Serial.println();
  Serial.println("Booting Sketch...");


  //ESP32 As access point
  WiFi.mode(WIFI_AP); //Access Point mode
  WiFi.softAP(ssid, password);

  // Start the DS18B20 Temp sensor
  sensors.begin();

  // Start the LED pixels
  pixels.begin();

  server.on("/", handleRoot);      //This is display page
  server.on("/readTime", handleTime);//To get update of Time Value only
  server.on("/readTemp", handleTemp);//To get update of Temp Value only
  server.on("/readPressure", handlePressure);//To get update of Temp Value only
 
  server.begin();                  //Start server
  Serial.println("HTTP server started");
}

//===============================================================
// LED Functions
//===============================================================

void updateTemp(){
  sensors.requestTemperatures(); 
  temp = sensors.getTempCByIndex(0) - 0.5;
}

void updateColorArray(){
  int red = MAX_POWER;
  int blue = MAX_POWER;

  if (temp <= COLD_TEMP){
    red = 0;
    blue = MAX_POWER;
    return;
  }
  else if (temp >= HOT_TEMP){
    red = MAX_POWER;
    blue = 0;
    return;
  }
  
  float part = (temp-COLD_TEMP)/(HOT_TEMP-COLD_TEMP);

  if (part < 0.5){
    red = part*2*MAX_POWER;
  }
  else{
    blue = (1-((part-0.5)*2))*MAX_POWER;
  }
  color[0] = red;
  color[1] = 0;
  color[2] = blue;
}

void updateTempColor(){
  for(int i=0;i<NUMPIXELS;i++)
  {
    pixels.setPixelColor(i, pixels.Color(color[0], color[1], color[2]));
  }
  pixels.show(); // This sends the updated pixel color to the hardware.
}

//===============================================================
// This routine is executed when you open its IP in browser
//===============================================================
void loop(void){
  server.handleClient();
  counterThread.check();
  updateTemp();
  updateColorArray();
  Serial.println(temp);
  updateTempColor();  

  delay(1);
}
