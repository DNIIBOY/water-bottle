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

#include <TimedAction.h>

const int avgCount = 20; // Amount of meassurements used for the avg value
const int analogPressurePin = A4;
int pressureVal = 0;  // Current value of pressure sensor

// Calibration values for how full/empty the bottle is
int mini = 505;  // Value for empty bottle
int maxi = 540;  // Value for full bottle

int pressureVals[avgCount];  // For taking average of last avgCount values
int pressureAvg;  // average value over avgCount meassurements
int avgIndex = 0;  // Index value for calculating average.
int pressureState = 0;  // How many quarters full the container is

int halfdiff = (maxi-mini)/2;
int quarters[4] = {mini, mini+halfdiff, maxi-halfdiff, maxi};


int updateAvgVal(){
  avgIndex = avgIndex % avgCount;
  pressureVals[avgIndex] = pressureVal;
  avgIndex++;
  
  int sum = 0;
  for (int j = 0; j<avgCount; j++){
    sum += pressureVals[j];
  }
  return sum/avgCount; 
}

int updatePressureState(){
  int state;
  if (pressureAvg > quarters[3]){
    state = 3;
  }
  else if (pressureAvg > quarters[2]){
    state = 2;
  }
  else if (pressureAvg > quarters[1]){
    state = 1;
  }
  else if (pressureAvg > quarters[0]){
    state = 0;
  }
  else{
    state = -1;
  }
  return state;
}

// To make excactly avgCount meassurements per second.
int pressureCount = 0;  // Current number
int lastMeassure;  // Last time it was done

void updateMeassure(){
  pressureCount++;
}
TimedAction pressureCountThread = TimedAction((1000/avgCount), updateMeassure);

//===============================================================
// Setup for LED timer + temp indicator
//===============================================================

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
const int LOW_MULTI = 24; // Multiplier for lower LEDS. Max is 64

const int START_TIME = 45;  // Timer start time in secs.
int timer = START_TIME;  // Current timer value
int color[3] = {0, 0, MAX_POWER};  // Color of temp RGB strip.

const int COLD_TEMP = 16; // Temp for full blue lights
const int HOT_TEMP = 23; // Temp for full red lights

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
// Setup
//===============================================================

int emptyCount = 0;  // Amount of time in a row, the counter has said empty.

void setup(void){
  Serial.begin(115200);
  Serial.println();
  Serial.println("Booting Sketch...");

  // Start the DS18B20 Temp sensor
  sensors.begin();

  // Start the LED pixels
  topPixels.begin();
  botPixels.begin();

  // Setup buzzer
  pinMode(buzzPin, OUTPUT);
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
  pressureVal = analogRead(analogPressurePin);
  timerThread.check();  // Update timer variable each second.
  pressureCountThread.check();  // Update pressure avgCount times per second.
  updateTemp();  // Update temp sensor value
  temp = 9;
  
  updateColorArray();

  if (emptyCount >= 10)
  {
    //timer = START_TIME;
    Serial.println("Water time");
  }
  
  if (timer > 0){
    updateTempColor();
    displayClock();
  }
  else if (timer == 0 || timer == -1){
    digitalWrite(buzzPin, HIGH);
    delay(60);
    digitalWrite(buzzPin, LOW);
    delay(150);
  }
  if (timer <= 0){
    drinkAlert();
  }

  if (pressureCount != lastMeassure){
    pressureAvg = updateAvgVal();
    pressureState = updatePressureState();
    Serial.print("AVG: ");
    Serial.println(pressureAvg);
    Serial.print("State: ");
    Serial.println(pressureState);
    lastMeassure = pressureCount; 
  }

  if (pressureState == -1){
    emptyCount++;
  }
  else{
    emptyCount = 0;
  }

  delay(1);
  topPixels.show(); // This sends the updated pixel color to the hardware.
  botPixels.show();
}
