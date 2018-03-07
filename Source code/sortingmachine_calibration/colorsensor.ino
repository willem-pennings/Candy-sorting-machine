/*
 * Color acquisition and analyzation functions
 */

/*
  TCS34725  ---------------  Arduino
  VCC       ---------------  5V
  GND       ---------------  GND
  SCL       ---------------  SCL
  SDA       ---------------  SDA
  LED       ---------------  2
*/

void measureColor(int measuredColor[]) {
  float r, g, b;
  uint16_t red, green, blue, clear;
  uint32_t sum;

  // Get raw data and store it in variables
  tcs.getRawData(&red, &green, &blue, &clear);

  // Do calculations to convert the measured values to adjusted 0-255 RGB values
  sum = clear;
  r = red; g = green; b = blue;
  r /= sum; g /= sum; b /= sum;
  r *= 256; g *= 256; b *= 256;
  measuredColor[0] = r; measuredColor[1] = g; measuredColor[2] = b;
}

void RGBtoHSV(int RGBColor[], int HSVColor[]) {
  double color[3];
  double Cmax, Cmin, delta;
  double hue, sat, val;
  double hueAngle, satPerc, valPerc;

  color[0] = RGBColor[0]; color[1] = RGBColor[1]; color[2] = RGBColor[2];

  if(debug){
    Serial.println("INFO\t\t-- Converting RGB to HSV color --");
    Serial.print("INFO\t\tInput colors are: \t");
    Serial.print(F("R: ")); Serial.print(color[0]); Serial.print(F("\t"));
    Serial.print(F("G: ")); Serial.print(color[1]); Serial.print(F("\t"));
    Serial.print(F("B: ")); Serial.println(color[2]);
  }
  
  color[0] /= 255; color[1] /= 255; color[2] /= 255;
  
  Cmax = arrayMax(color, 3);
  Cmin = arrayMin(color, 3);
  delta = Cmax - Cmin;

  if(delta == 0) {
    hue = 0;
  }
  if(Cmax == color[0]) {
    hue = (60 * fmod(((color[1] - color[2]) / delta), 6)) * 0.7083;
    if(color[1] - color[2] < 0) {
      hue += 255;
    }
  }
  if(Cmax == color[1]) {
    hue = (60 * (((color[2] - color[0]) / delta) + 2)) * 0.7083;
  }
  if(Cmax == color[2]) {
    hue = (60 * (((color[0] - color[1]) / delta) + 4)) * 0.7083;
  }

  if(Cmax == 0) {
    sat = 0;
  } else  {
    sat = (delta / Cmax) * 255;
  }
  
  val = Cmax * 255;

  HSVColor[0] = (int)hue; HSVColor[1] = (int)sat; HSVColor[2] = (int)val;

  // Print HSV values for calibration purposes //
  Serial.print(HSVColor[0]); Serial.print("\t");
  Serial.print(HSVColor[2]); Serial.print("\t");
  Serial.println(HSVColor[3]);
  
  if(debug){
    Serial.print(F("INFO\t\tOutput colors are: \t"));
    Serial.print(F("H: ")); Serial.print(HSVColor[0]); Serial.print(F("\t\t"));
    Serial.print(F("S: ")); Serial.print(HSVColor[1]); Serial.print(("\t\t"));
    Serial.print(F("V: ")); Serial.println(HSVColor[2]);
    hueAngle = (hue / 255) * 360; satPerc = (sat / 255) * 100; valPerc = (val / 255) * 100;
    Serial.print(F("INFO\t\tAngle-percent equiv.: \t"));
    Serial.print(F("H: ")); Serial.print(hueAngle); Serial.print(F("\t"));
    Serial.print(F("S: ")); Serial.print(satPerc); Serial.print(F("\t"));
    Serial.print(F("V: ")); Serial.println(valPerc);
    Serial.println(F("INFO\t\t-- End RGB to HSV color conversion --"));
  }
}

ItemColor determineColor(int measuredColor[]) {
  int i;
  ItemColor itemcolor = unknown;

  for(i = 0; i < 6; i++) {
    if(candytype == mms) {
      if((measuredColor[0] >= calibratedvalues[i][0][0][0]) && // H lower bound check
         (measuredColor[0] <= calibratedvalues[i][0][1][0]) && // H upper bound check
         (measuredColor[1] >= calibratedvalues[i][0][0][1]) && // S lower bound check
         (measuredColor[1] <= calibratedvalues[i][0][1][1])) { // S upper bound check
           // Set the item color when a check is succesful
           itemcolor = (ItemColor)i;
           i = 6;
      }
    }
    if(candytype == skittles) {
      if((measuredColor[0] >= calibratedvalues[i][1][0][0]) && // H lower bound check
         (measuredColor[0] <= calibratedvalues[i][1][1][0]) && // H upper bound check
         (measuredColor[1] >= calibratedvalues[i][1][0][1]) && // S lower bound check
         (measuredColor[1] <= calibratedvalues[i][1][1][1])) { // S upper bound check
           itemcolor = (ItemColor)(i + 6);
           i = 6;
      }
    }
  }

  if(0){
    // Print the detected color
    switch(itemcolor) {
      case mm_red: Serial.println(F("MM_RED")); break;
      case mm_green: Serial.println(F("MM_GREEN")); break;
      case mm_blue: Serial.println(F("MM_BLUE")); break;
      case mm_yellow: Serial.println(F("MM_YELLOW")); break;
      case mm_orange: Serial.println(F("MM_ORANGE")); break;
      case mm_brown: Serial.println(F("MM_BROWN")); break;
  
      case sk_red: Serial.println(F("SK_RED")); break;
      case sk_green: Serial.println(F("SK_GREEN")); break;
      case sk_yellow: Serial.println(F("SK_YELLOW")); break;
      case sk_orange: Serial.println(F("SK_ORANGE")); break;
      case sk_purple: Serial.println(F("SK_PURPLE")); break;
  
      case unknown: Serial.println(F("UNKNOWN")); break;
    }
  }
  return itemcolor;
}
