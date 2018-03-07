#ifndef SORTINGMACHINE_H
  #define SORTINGMACHINE_H

  /* Do not put user settings here - only globally used internal variables */
  
  // Machine state enumerator
  typedef enum {standby, loaditem, analyzecolor, determinecolor, setreleasepos, releaseitem, reverse} State;
  State state = standby;

  // Array to keep track of the amount of candy sorted - order according to ItemColor enumerator (colorsensor.h)
  int amounts[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  // Item colors
  ItemColor nextcolor = unknown;
  ItemColor prevcolor = unknown;
  ItemColor finalcolor = unknown;

  // Carousel movement variables
  int carouselPrevPos = 0;
  int carouselNextPos = 0;

  // The amount of candy sorted will be stored in this variable
  unsigned int candyNumber = 0;

  // Time between color measurements
  unsigned long measurementTime = 0;

  // Time variable for button bouncing
  unsigned long buttonTime = 0;

  // Time variable that registers the time required to sort a certain amount of candy
  unsigned long sortTime = 0;

  // Variable that checks if the second arduino was properly configured
  unsigned short I2CMillis;
  bool I2COK = false;

  // Time variable that registers the moment at which the load process was started
  unsigned long loadStartTime = 0;

  // This variable goes true if the standby button is pressed and false again after machine is in standby
  bool lastRound = false;

  // This variable goes true if a fix by an operator is required (when the machine can't unjam itself)
  bool opFix = false;
  
#endif
