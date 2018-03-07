#ifndef COLORSENSOR_H
  #define COLORSENSOR_H
  
  // Color type definition: M&Ms exist in all but purple; Skittles exist in all but brown and blue
  typedef enum {mm_brown, mm_green, mm_blue, mm_yellow, mm_orange, mm_red, sk_red, sk_green, sk_yellow, sk_orange, sk_purple, unknown} ItemColor;

  // Candy type definition: M&Ms or skittles
  typedef enum {mms, skittles} CandyType;

  // Calibrated color values for M&Ms and Skittles, mean and standard deviation
  // Array structure: calibratedvalues[color][candytype][min/max][H,S,V]
  const double calibratedvalues[6][2][2][3] = {
    {
      { // Brown M&M's
        {53.00, 65.00, 0}, // range minimum
        {72.00, 81.00, 0}, // range maximum
      },
      { // Red Skittles
        {15.00, 75.00, 90.00}, // range minimum
        {38.00, 110.0, 107.0}, // range maximum
      },
    },
    {
      { // Green M&Ms
        {70.00, 85.00, 0}, // range minimum
        {82.00, 130.0, 0}, // range maximum
      },
      { // Green Skittles
        {71.37, 85.95, 95.92}, // range minimum
        {78.15, 163.0, 127.2}, // range maximum
      },
    },
    {
      { // Blue M&Ms
        {83.00, 48.87, 0}, // range minimum
        {150.0, 120.0, 0}, // range maximum
      },
      { // Yellow Skittles
        {38.01, 98.92, 93.83}, // range minimum
        {54.03, 168.6, 101.4}, // range maximum
      },
    },
    {
      { // Yellow M&Ms
        {40.00, 90.00, 0}, // range minimum
        {52.00, 160.0, 0}, // range maximum
      },
      { // Orange Skittles
        {10.00, 110.1, 90.00}, // range minimum
        {35.50, 180.0, 140.0}, // range maximum
      },
    },
    {
      { // Orange M&Ms
        {15.00, 99.00, 0}, // range minimum
        {39.00, 160.0, 0}, // range maximum
      },
      { // Purple Skittles
        {54.15, 36.75, 81.53}, // range minimum
        {79.53, 77.01, 101.6}, // range maximum
      },
    },
    {
      { // Red M&Ms / unknown
        {15.00, 61.47, 0}, // range minimum 23.55, 61.47, 0
        {50.00, 130.0, 0}, // range maximum 43.27, 99.01, 0
      },
      { // Unknown Skittles
        {65.00, 85.00, 95.00}, // range minimum
        {68.00, 90.00, 100.0}, // range maximum
      },
    },
  };

  void measureColor(int measuredColor[]);
  void RGBtoHSV(int RGBColor[], int HSVColor[]);
  ItemColor determineColor(int measuredColor[]);
  
#endif
