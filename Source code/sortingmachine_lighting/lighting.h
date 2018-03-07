/*
 * This file stores all lighting-related functions and code. Will be controlled externally.
 * Code by Bill Earl.
 */

// Pattern types supported:
enum pattern {NONE, RAINBOW_CYCLE, THEATER_CHASE, COLOR_WIPE, SCANNER, FADE};

// Patern directions supported:
enum direction {FORWARD, REVERSE};

// NeoPattern Class - derived from the Adafruit_NeoPixel class
class NeoPatterns:public Adafruit_NeoPixel {
  public:
  // Member Variables:  
  pattern  ActivePattern;  // which pattern is running
  direction Direction;     // direction to run the pattern
  
  unsigned long Interval;   // milliseconds between updates
  unsigned long lastUpdate; // last update of position
  
  uint32_t Color1, Color2;  // What colors are in use
  uint16_t TotalSteps;  // total number of steps in the pattern
  uint16_t Index;  // current step within the pattern
  
  void (*OnComplete)();  // Callback on completion of pattern
  
  // Constructor - calls base-class constructor to initialize strip
  NeoPatterns(uint16_t pixels, uint8_t pin, uint8_t type, void (*callback)())
  :Adafruit_NeoPixel(pixels, pin, type) {
    OnComplete = callback;
  }
  
  // Update the pattern
  void Update() {
    if((millis() - lastUpdate) > Interval) {
        lastUpdate = millis();
        switch(ActivePattern) {
          case RAINBOW_CYCLE:
              RainbowCycleUpdate();
              break;
          case THEATER_CHASE:
              TheaterChaseUpdate();
              break;
          case COLOR_WIPE:
              ColorWipeUpdate();
              break;
          case SCANNER:
              ScannerUpdate();
              break;
          case FADE:
              FadeUpdate();
              break;
          default:
              break;
      }
    }
  }

  // Increment the Index and reset at the end
  void Increment() {
    if (Direction == FORWARD) {
     Index++;
     if (Index >= TotalSteps) {
        Index = 0;
        if (OnComplete != NULL) {
          OnComplete(); // call the comlpetion callback
        }
      }
    } else {
      --Index;
      if (Index <= 0) {
        Index = TotalSteps-1;
        if (OnComplete != NULL) {
          OnComplete(); // call the comlpetion callback
        }
      }
    }
  }
  
  // Reverse pattern direction
  void Reverse() {
    if (Direction == FORWARD) {
      Direction = REVERSE;
      Index = TotalSteps-1;
    } else {
      Direction = FORWARD;
      Index = 0;
    }
  }
  
  // Initialize for a RainbowCycle
  void RainbowCycle(uint8_t interval, direction dir = FORWARD) {
    ActivePattern = RAINBOW_CYCLE;
    Interval = interval;
    TotalSteps = 255;
    Index = 0;
    Direction = dir;
  }
  
  // Update the Rainbow Cycle Pattern
  void RainbowCycleUpdate() {
    for(int i=0; i< numPixels(); i++) {
      setPixelColor(i, Wheel(((i * 256 / numPixels()) + Index) & 255));
    }
    show();
    Increment();
  }

  // Initialize for a Theater Chase
  void TheaterChase(uint32_t color1, uint32_t color2, uint8_t interval, direction dir = FORWARD) {
    ActivePattern = THEATER_CHASE;
    Interval = interval;
    TotalSteps = numPixels();
    Color1 = color1;
    Color2 = color2;
    Index = 0;
    Direction = dir;
 }
  
  // Update the Theater Chase Pattern
  void TheaterChaseUpdate() {
    for(int i=0; i< numPixels(); i++) {
      if ((i + Index) % 3 == 0) {
        setPixelColor(i, Color1);
      } else {
        setPixelColor(i, Color2);
      }
    }
    show();
    Increment();
  }

  // Initialize for a ColorWipe
  void ColorWipe(uint32_t color, uint8_t interval, direction dir = FORWARD) {
    ActivePattern = COLOR_WIPE;
    Interval = interval;
    TotalSteps = numPixels();
    Color1 = color;
    Index = 0;
    Direction = dir;
  }
  
  // Update the Color Wipe Pattern
  void ColorWipeUpdate() {
    setPixelColor(Index, Color1);
    show();
    Increment();
  }
  
  // Initialize for a SCANNNER
  void Scanner(uint32_t color1, uint8_t interval) {
    ActivePattern = SCANNER;
    Interval = interval;
    TotalSteps = (numPixels() - 1) * 2;
    Color1 = color1;
    Index = 0;
  }

  // Update the Scanner Pattern
  void ScannerUpdate() { 
    for (int i = 0; i < numPixels(); i++) {
      if (i == Index) {
         setPixelColor(i, Color1);
      } else if (i == TotalSteps - Index) {
         setPixelColor(i, Color1);
      } else {
         setPixelColor(i, DimColor(getPixelColor(i)));
      }
    }
    show();
    Increment();
  }
  
  // Initialize for a Fade
  void Fade(uint32_t color1, uint32_t color2, uint16_t steps, uint8_t interval, direction dir = FORWARD) {
    ActivePattern = FADE;
    Interval = interval;
    TotalSteps = steps;
    Color1 = color1;
    Color2 = color2;
    Index = 0;
    Direction = dir;
  }
  
  // Update the Fade Pattern
  void FadeUpdate() {
    // Calculate linear interpolation between Color1 and Color2
    // Optimise order of operations to minimize truncation error
    uint8_t red = ((Red(Color1) * (TotalSteps - Index)) + (Red(Color2) * Index)) / TotalSteps;
    uint8_t green = ((Green(Color1) * (TotalSteps - Index)) + (Green(Color2) * Index)) / TotalSteps;
    uint8_t blue = ((Blue(Color1) * (TotalSteps - Index)) + (Blue(Color2) * Index)) / TotalSteps;
    
    ColorSet(Color(red, green, blue));
    show();
    Increment();
  }
 
  // Calculate 50% dimmed version of a color (used by ScannerUpdate)
  uint32_t DimColor(uint32_t color) {
    // Shift R, G and B components one bit to the right
    uint32_t dimColor = Color(Red(color) >> 1, Green(color) >> 1, Blue(color) >> 1);
    return dimColor;
  }

  // Set all pixels to a color (synchronously)
  void ColorSet(uint32_t color) {
    for (int i = 0; i < numPixels(); i++) {
      setPixelColor(i, color);
    }
    show();
  }

  // Returns the Red component of a 32-bit color
  uint8_t Red(uint32_t color) {
    return (color >> 16) & 0xFF;
  }

  // Returns the Green component of a 32-bit color
  uint8_t Green(uint32_t color) {
    return (color >> 8) & 0xFF;
  }

  // Returns the Blue component of a 32-bit color
  uint8_t Blue(uint32_t color) {
    return color & 0xFF;
  }
  
  // Input a value 0 to 255 to get a color value.
  // The colours are a transition r - g - b - back to r.
  uint32_t Wheel(byte WheelPos) {
    WheelPos = 255 - WheelPos;
    if(WheelPos < 85) {
      return Color(255 - WheelPos * 3, 0, WheelPos * 3);
    } else if(WheelPos < 170) {
      WheelPos -= 85;
      return Color(0, WheelPos * 3, 255 - WheelPos * 3);
    } else {
      WheelPos -= 170;
      return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
    }
  }
};

// Callback function for when a pattern is done on the LED strip.
void ringComplete();

// Define a NeoPattern for the ring
NeoPatterns ring(NUM_LEDS1, LED_PIN1, NEO_GRB + NEO_KHZ800, &ringComplete);

// Define a NeoPattern for the spots (use the same callback)
NeoPatterns spots(NUM_LEDS2, LED_PIN2, NEO_GRB + NEO_KHZ800, &ringComplete);

const uint8_t PROGMEM gamma[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 
};

int gammaconv(unsigned int val) {
  int x;
  x = pgm_read_byte(&gamma[val]);
  return x;
}

// Declare a bunch of colors
uint32_t red = spots.Color(255, 0, 0);
uint32_t green = spots.Color(0, 255, 0);
uint32_t blue = spots.Color(0, 0, 255);
uint32_t yellow = spots.Color(255, 255, 0);
uint32_t orange = spots.Color(255, gammaconv(140), 0); // darkorange
uint32_t brown = spots.Color(gammaconv(165), gammaconv(42), gammaconv(42));
uint32_t purple = spots.Color(gammaconv(128), 0, gammaconv(128));
uint32_t white = spots.Color(255, 255, 255);
uint32_t black = spots.Color(0, 0, 0);

void ringInitFade(ItemColor prevcolor, ItemColor nextcolor) {
  uint32_t color1;
  uint32_t color2;
  switch(prevcolor) {
    case mm_red: color1 = red; Serial.println(F("INFO\t\tFirst color was received as red.")); break;
    case mm_green: color1 = green; Serial.println(F("INFO\t\tFirst color was received as green.")); break;
    case mm_blue: color1 = blue; Serial.println(F("INFO\t\tFirst color was received as blue.")); break;
    case mm_yellow: color1 = yellow; Serial.println(F("INFO\t\tFirst color was received as yellow.")); break;
    case mm_orange: color1 = orange; Serial.println(F("INFO\t\tFirst color was received as orange.")); break;
    case mm_brown: color1 = brown; Serial.println(F("INFO\t\tFirst color was received as brown.")); break;
    case sk_red: color1 = red; Serial.println(F("INFO\t\tFirst color was received as red.")); break;
    case sk_green: color1 = green; Serial.println(F("INFO\t\tFirst color was received as green.")); break;
    case sk_yellow: color1 = yellow; Serial.println(F("INFO\t\tFirst color was received as yellow.")); break;
    case sk_orange: color1 = orange; Serial.println(F("INFO\t\tFirst color was received as orange.")); break;
    case sk_purple: color1 = purple; Serial.println(F("INFO\t\tFirst color was received as purple.")); break;
    case unknown: color1 = white; Serial.println(F("INFO\t\tFirst color was received as unknown (white color).")); break;
    default: color1 = red; break;
  }
  switch(nextcolor) {
    case mm_red: color2 = red; Serial.println(F("INFO\t\tSecond color was received as red.")); break;
    case mm_green: color2 = green; Serial.println(F("INFO\t\tSecond color was received as green.")); break;
    case mm_blue: color2 = blue; Serial.println(F("INFO\t\tSecond color was received as blue.")); break;
    case mm_yellow: color2 = yellow; Serial.println(F("INFO\t\tSecond color was received as yellow.")); break;
    case mm_orange: color2 = orange; Serial.println(F("INFO\t\tSecond color was received as orange.")); break;
    case mm_brown: color2 = brown; Serial.println(F("INFO\t\tSecond color was received as brown.")); break;
    case sk_red: color2 = red; Serial.println(F("INFO\t\tSecond color was received as red.")); break;
    case sk_green: color2 = green; Serial.println(F("INFO\t\tSecond color was received as green.")); break;
    case sk_yellow: color2 = yellow; Serial.println(F("INFO\t\tSecond color was received as yellow.")); break;
    case sk_orange: color2 = orange; Serial.println(F("INFO\t\tSecond color was received as orange.")); break;
    case sk_purple: color2 = purple; Serial.println(F("INFO\t\tSecond color was received as purple.")); break;
    case unknown: color2 = white; Serial.println(F("INFO\t\tSecond color was received as unknown (white color).")); break;
    default: color2 = yellow; break;
  }
  ring.Fade(color2, color1, 1, 1000000); // flipped the colors to make the final color appear (should be temp until fix)
}

void ringComplete() {
  if(state == standby) {
    ring.RainbowCycle(ringDelay);
  } else {
    // When doing the fading patterns, simply stop after the fade is completed until a new fade is initialized
  }
}

