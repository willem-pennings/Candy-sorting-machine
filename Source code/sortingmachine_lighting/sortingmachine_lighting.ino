#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include <EasyTransferI2C.h>
#include <Servo.h>

#define NUM_LEDS1   32 // The amount of LEDs in the color ring
#define NUM_LEDS2   6 // The amount of LEDs for the spot lights
#define LED_PIN1    10 // Digital pin for the WS2812B circle
#define LED_PIN2    11 // Digital pin for the WS2812B spot lights
#define SERVO1      3 // Digital pin for the servo
#define BRIGHTNESS1 80 // LED ring brightness
#define BRIGHTNESS2 80 // LED spots brightness

int servoSpeed = 100; // Rotational speed of the servo

typedef enum {mm_red, mm_green, mm_blue, mm_yellow, mm_orange, mm_brown, sk_red, sk_green, sk_yellow, sk_orange, sk_purple, unknown} ItemColor;
typedef enum {standby, loaditem, analyzecolor, determinecolor, setreleasepos, releaseitem, reverse} State;
typedef enum {mms, skittles} CandyType;

// Initialize servo
Servo mixservo;

// Variable sync initialization
EasyTransferI2C ET;

struct SYNC_CONTAINER {
  ItemColor nextcolor;
  ItemColor prevcolor;
  State state;
  CandyType candytype;
};

SYNC_CONTAINER sync;

State state = standby;

ItemColor nextcolor;
ItemColor prevcolor;

CandyType candytype = mms;

const int ringDelay = 9;

unsigned long lastTime = 0;
const int delayTime = 500;

unsigned int currentLed = 5;

uint32_t ringColor;

#include "lighting.h"

void setup() {
  Wire.begin(8);
  ET.begin(details(sync), &Wire);
  Wire.onReceive(receive);
  Serial.begin(115200);
  
  // Initialize all the pixelStrips and set the ring brightness
  ring.begin();
  ring.setBrightness(BRIGHTNESS1);
  spots.begin();
  spots.setBrightness(BRIGHTNESS2);
  
  Serial.println(F("INFO\t\tLighting Arduino was initialized and I2C communication was succesfully started!"));

  // Send all-good signal to master
  Wire.beginTransmission(7);
  Wire.write(millis());
  Wire.endTransmission();
}

void loop() {
  if(ET.receiveData()) {
    state = sync.state;
    prevcolor = sync.prevcolor;
    nextcolor = sync.nextcolor;
    Serial.println(F("INFO\t\tReceived & updated sync variables from device 1."));
    
    switch(state) {
    case standby:
      if(sync.candytype != candytype) {
        Serial.println(F("INFO\t\tCandy type has changed! Providing feedback through the spot lights."));
        if(sync.candytype == mms) {
          for(int i = 0; i < NUM_LEDS2; i++) {
            spots.setPixelColor(i, blue);
          }
          spots.show();
          delay(1000);
          for(int i = 0; i < NUM_LEDS2; i++) {
            spots.setPixelColor(i, black);
          }
          spots.show();
        } else if(sync.candytype == skittles) {
          for(int i = 0; i < NUM_LEDS2; i++) {
            spots.setPixelColor(i, red);
          }
          spots.show();
          delay(1000);
          for(int i = 0; i < NUM_LEDS2; i++) {
            spots.setPixelColor(i, black);
          }
          spots.show();
        }
        candytype = sync.candytype;
      }

      // Initialize a rainbow pattern on the spot lights
      spots.ActivePattern = RAINBOW_CYCLE;
      spots.TotalSteps = 255;
      spots.Interval = min(10, spots.Interval);
      spots.RainbowCycle(ringDelay);

      // Enable a rainbow effect on the LED ring
      ring.ActivePattern = RAINBOW_CYCLE;
      ring.TotalSteps = 255;
      ring.Interval = min(10, ring.Interval);
      ring.RainbowCycle(ringDelay);
      Serial.println(F("INFO\t\tReceived state is standby. Rainbow patterns were activated."));

      mixservo.detach();
      
      break;
    case determinecolor:
      ring.ActivePattern = NONE;
      spots.ActivePattern = NONE;
      Serial.println(F("INFO\t\tCurrently not in standby mode. Setting new ring color & updating spots."));

      // Turn off all spots before setting a new color
      for(int i = 0; i < NUM_LEDS2; i++) {
        spots.setPixelColor(i, black);
      }
      
      switch(nextcolor) {
        case mm_red: spots.setPixelColor(4, white); ringColor = red; break;
        case mm_green: spots.setPixelColor(0, white); ringColor = green; break;
        case mm_blue: spots.setPixelColor(1, white); ringColor = blue; break;
        case mm_yellow: spots.setPixelColor(2, white); ringColor = yellow; break;
        case mm_orange: spots.setPixelColor(3, white); ringColor = orange; break;
        case mm_brown: spots.setPixelColor(5, white); ringColor = brown; break;
        case sk_red: spots.setPixelColor(5, white); ringColor = red; break;
        case sk_green: spots.setPixelColor(0, white); ringColor = green; break;
        case sk_yellow: spots.setPixelColor(1, white); ringColor = yellow; break;
        case sk_orange: spots.setPixelColor(2, white); ringColor = orange; break;
        case sk_purple: spots.setPixelColor(3, white); ringColor = purple; break;
        case unknown: spots.setPixelColor(4, white); ringColor = white; break;
      }
      
      spots.show();
      Serial.println(F("INFO\t\tSpot lights colors were set!"));

      for(int i = 0; i < NUM_LEDS1; i++) {
        ring.setPixelColor(i, ringColor);
      }
      ring.show();

      mixservo.attach(SERVO1);
      mixservo.write(servoSpeed);
      
      break;
    }
  }
  
  // Handle ring & spots lighting for standby mode (mostly in sortingprocess)
  if(ring.ActivePattern != NONE) {
    ring.Update();
    spots.Update();
  }

  /*if((millis() - lastTime > delayTime) && state == standby) {
    for(int i = 0; i < NUM_LEDS2; i++) {
      spots.setPixelColor(i, black);
    }
    spots.setPixelColor(currentLed, white);
    spots.show();
    if(currentLed == 0) {
      currentLed = 5;
    } else {
      currentLed--;
    }
    lastTime = millis();
  }*/
}

void receive(int numBytes) {}
