/***** SUPPORT LIBRARIES *****/

#include <Wire.h>
#include <Math.h>
#include "Adafruit_TCS34725.h"
#include <EasyTransferI2C.h>
#include <AccelStepper.h>

#include "colorsensor.h"
#include "functions.h"
#include "sortingmachine.h"
#include "stepper.h"

/***** PIN DEFINITIONS *****/

// Microswitch input pins (internal pullup)
#define MICRO1  A0
#define MICRO2  A1

// Button input pins (internal pullup)
#define BUTTON1 A2
#define BUTTON2 A3

// EasyDriver 1 pin definitions (measurement wheel)
#define STEP1   3
#define DIR1    4
#define EN1     5

// EasyDriver 2 pin definitions (carousel motor)
#define STEP2   8
#define DIR2    9
#define EN2     10

/***** HARDWARE & COMMS *****/

// TCS34725 initialization
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

// Stepper initialization
AccelStepper stepper1(1,STEP1,DIR1);
AccelStepper stepper2(1,STEP2,DIR2);

// Variable sync initialization
EasyTransferI2C ET;

struct SYNC_CONTAINER {
  ItemColor nextcolor;
  ItemColor prevcolor;
  State state;
  CandyType candytype;
};

SYNC_CONTAINER sync;

/***** SETTINGS *****/

// Default candy type - 'mms' for M&Ms, 'skittles' for Skittles
CandyType candytype = mms;
// Stepper motor delay between a STEP transition [microseconds]
const unsigned int delayBetweenTransitions = 1000;
// Time between color measurements [milliseconds]
const int timeBetweenMeasurements = 25;
// The amount of measurements with an 'unknown' result before the machine goes into standby.
const int maxconsecUnknown = 30;
// The maximum amount of time allowed for the measurement wheel to load a piece of candy
const int maxLoadTime = 250;
// If true, send all debugging information over serial.
const bool debug = false;
// If true, only send important/critical information over serial.
const bool debug_important = false;

void setup() {
  // Set up serial communication at 115200 baudrate
  Serial.begin(250000);
  // Start I2C communication (address 7) and set up variable sync communication
  Wire.begin(7);
  Wire.onReceive(receive);
  ET.begin(details(sync), &Wire);
  
  Serial.println(F("== Machine was programmed with a calibration sketch =="));

  // Initialize the color sensor
  initializeTCS34725();
  // Initialize stepper motors
  initializeSteppers();
  // Initialize the stepper motor output pins and write default values
  initializePins();
}

void loop() {
  // The sorting process handles the different machine states and executes the bulk of the commands
  sortingProcess();

  // Check buttons and handle them if inputs are detected
  buttonHandler();

  stepper1.runSpeed();
  stepper2.run();
}
