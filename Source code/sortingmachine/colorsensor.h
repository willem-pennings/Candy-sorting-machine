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
        {107.1, 70.00, 63.45}, // range minimum
        {115.1, 103.0, 74.55}, // range maximum
      },
      { // Red Skittles
        {12.50, 85.70, 85.70}, // range minimum
        {40.90, 104.9, 110.0}, // range maximum
      },
    },
    {
      { // Green M&Ms
        {97.11, 108.6, 57.48}, // range minimum
        {108.1, 130.0, 62.52}, // range maximum
      },
      { // Green Skittles
        {72.10, 106.0, 70.20}, // range minimum
        {75.10, 116.0, 73.40}, // range maximum
      },
    },
    {
      { // Blue M&Ms
        {126.2, 110.0, 36.63}, // range minimum
        {142.1, 200.0, 58.03}, // range maximum
      },
      { // Yellow Skittles
        {42.70, 95.60, 90.10}, // range minimum
        {47.90, 102.0, 97.50}, // range maximum
      },
    },
    {
      { // Yellow M&Ms
        {38.01, 87.88, 70.59}, // range minimum
        {62.54, 148.5, 149.4}, // range maximum
      },
      { // Orange Skittles
        {13.70, 105.0, 97.80}, // range minimum
        {25.90, 122.0, 122.0}, // range maximum
      },
    },
    {
      { // Orange M&Ms
        {12.00, 60.00, 86.69}, // range minimum
        {35.67, 120.5, 120.5}, // range maximum
      },
      { // Purple Skittles
        {56.90, 89.80, 74.50},
        {78.70, 95.10, 83.90},
      },
    },
    {
      { // Red M&Ms / unknown
        {0.000, 81.23, 73.87}, // range minimum 23.55, 61.47, 0
        {117.7, 105.9, 108.4}, // range maximum 43.27, 99.01, 0
      },
      { // Unknown Skittles
        {67.40, 95.50, 77.00}, // range minimum
        {66.70, 96.50, 78.00}, // range maximum
      },
    },
  };

  void measureColor(int measuredColor[]);
  void RGBtoHSV(int RGBColor[], int HSVColor[]);
  ItemColor determineColor(int measuredColor[]);
  
#endif
