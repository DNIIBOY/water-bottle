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

int analogPressurePin = A0;
int pressureVal = 0;

//===============================================================
// Setup for LED timer + temp indicator
//===============================================================

#include <TimedAction.h>
#include <Adafruit_NeoPixel.h>

// Setup pins for both LEDs
#define TopLEDPin 0  // Bottom right
#define BotLEDPin 15  // 1 up from bottom right

// How many NeoPixels are attached to the Arduino?
#define TOPNUMPIXELS 45
#define BOTNUMPIXELS 41

Adafruit_NeoPixel topPixels = Adafruit_NeoPixel(TOPNUMPIXELS, TopLEDPin, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel botPixels = Adafruit_NeoPixel(BOTNUMPIXELS, BotLEDPin, NEO_GRB + NEO_KHZ800);

const int MAX_POWER = 4; // Max led power
const int LOW_MULTI = 8; // Multiplier for lower LEDS

const int START_TIME = 300;  // Timer start time in secs.
int timer = START_TIME;  // Current timer value
int color[3] = {0, 0, MAX_POWER};  // Color of temp RGB strip.

const int COLD_TEMP = 14; // Temp for full blue lights
const int HOT_TEMP = 25; // Temp for full red lights

// Seperate thread for time, to prevent pausing the system
void updateTimer(){
  timer--;
}
TimedAction timerThread = TimedAction(1000, updateTimer);

//===============================================================
// Setup for drink alert and buzzer.
//===============================================================

#define buzzPin 2

//===============================================================
// These routines are executed when you open its IP in browser
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
 int pressure = analogRead(analogPressurePin);
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

  // Start the LED pixels
  topPixels.begin();
  botPixels.begin();

  // Setup buzzer
  pinMode(buzzPin, OUTPUT);

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
  }
  else if (temp >= HOT_TEMP){
    red = MAX_POWER;
    blue = 0;
  }
  else{
    float part = (temp-COLD_TEMP)/(HOT_TEMP-COLD_TEMP);

    if (part < 0.5){
      red = part*2*MAX_POWER;
    }
    else{
      blue = (1-((part-0.5)*2))*MAX_POWER;
    }
  }
  
  color[0] = red;
  color[1] = 0;
  color[2] = blue;
}

void updateTempColor(){
  for(int i=0;i<TOPNUMPIXELS;i++)
  {
    topPixels.setPixelColor(i, topPixels.Color(color[0], color[1], color[2]));
    botPixels.setPixelColor(i, botPixels.Color(min(color[0] * LOW_MULTI, 255), min(color[1] * LOW_MULTI, 255), min(color[2] * LOW_MULTI, 255)));
  }
  if (timer > 0){
   // Green LED to display start of clock
  topPixels.setPixelColor(23, topPixels.Color(0, MAX_POWER, 0));
  }
}

float clockPixel(){
    float part = ((float)START_TIME-(float)timer)/(float)START_TIME;
    float pix = ((part*TOPNUMPIXELS)+(23));
    int pixel = (int)pix%TOPNUMPIXELS;
    return pixel;
}

void displayClock(){
  int pixel = clockPixel();
  topPixels.setPixelColor(pixel, topPixels.Color(MAX_POWER, MAX_POWER, MAX_POWER));
}

void drinkAlert(){
  if (timer % 2 == 0){
    for(int i=0;i<TOPNUMPIXELS;i++)
    {
      topPixels.setPixelColor(i, topPixels.Color(MAX_POWER, MAX_POWER, MAX_POWER));
      botPixels.setPixelColor(i, botPixels.Color(min(MAX_POWER * LOW_MULTI, 255), min(MAX_POWER * LOW_MULTI, 255), min(MAX_POWER * LOW_MULTI, 255)));
    }
  }
  else{
    for(int i=0;i<TOPNUMPIXELS;i++)
    {
      topPixels.setPixelColor(i, topPixels.Color(color[0], color[1], color[2]));
      botPixels.setPixelColor(i, botPixels.Color(min(color[0] * LOW_MULTI, 255), min(color[1] * LOW_MULTI, 255), min(color[2] * LOW_MULTI, 255)));
    }
  }
}

//===============================================================
// This routine is executed when you open its IP in browser
//===============================================================
void loop(void){
  timerThread.check();  // Update timer variable each second.
  updateTemp();  // Update temp sensor value
  updateColorArray();
  
  if (timer > 0){
    updateTempColor();
    displayClock();
  }
  else if (timer == 0){
    digitalWrite(buzzPin, HIGH);
    delay(60);
    digitalWrite(buzzPin, LOW);
    delay(250);
  }
  if (timer <= 0){
    drinkAlert();
  }

  server.handleClient();  // Update webpage
  topPixels.show(); // This sends the updated pixel color to the hardware.
  botPixels.show();
  delay(1);
}
