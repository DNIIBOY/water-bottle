#include <Adafruit_NeoPixel.h>

// Setup pins for both LEDs
#define TopLEDPin 0  // Bottom right
#define BotLEDPin 15  // 1 up from bottom right

// How many NeoPixels are attached to the Arduino?
#define TOPNUMPIXELS 45
#define BOTNUMPIXELS 41

Adafruit_NeoPixel topPixels = Adafruit_NeoPixel(TOPNUMPIXELS, TopLEDPin, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel botPixels = Adafruit_NeoPixel(BOTNUMPIXELS, BotLEDPin, NEO_GRB + NEO_KHZ800);

int delayval = 20; // delay for half a second

void setup() 
{
  topPixels.begin(); // This initializes the NeoPixel library.
  botPixels.begin();
}

void loop() 
{
  for(int i=0;i<TOPNUMPIXELS;i++)
  {
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    topPixels.setPixelColor(i, topPixels.Color(64,0,0)); // Moderately bright green color.
    topPixels.show(); // This sends the updated pixel color to the hardware.
    delay(delayval); // Delay for a period of time (in milliseconds).
  }

  for(int i=0;i<TOPNUMPIXELS;i++)
  {
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    topPixels.setPixelColor(i, topPixels.Color(0,64,0)); // Moderately bright green color.
    topPixels.show(); // This sends the updated pixel color to the hardware.
    delay(delayval); // Delay for a period of time (in milliseconds).
  }

  for(int i=0;i<TOPNUMPIXELS;i++)
  {
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    topPixels.setPixelColor(i, topPixels.Color(0,0,64)); // Moderately bright green color.
    topPixels.show(); // This sends the updated pixel color to the hardware.
    delay(delayval); // Delay for a period of time (in milliseconds).
  }

  for(int i=0;i<BOTNUMPIXELS;i++)
  {
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    botPixels.setPixelColor(i, botPixels.Color(0,0,64)); // Moderately bright green color.
    botPixels.show(); // This sends the updated pixel color to the hardware.
    delay(delayval); // Delay for a period of time (in milliseconds).
  }
  for(int i=0;i<BOTNUMPIXELS;i++)
  {
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    botPixels.setPixelColor(i, botPixels.Color(0,64,0)); // Moderately bright green color.
    botPixels.show(); // This sends the updated pixel color to the hardware.
    delay(delayval); // Delay for a period of time (in milliseconds).
  }
}
