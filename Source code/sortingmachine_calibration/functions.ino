/*
 * Global sorting functions
 */

void initializeTCS34725() {
  // Check if the color sensor was succesfully initialized
  if (tcs.begin()) {
    if(debug){Serial.println(F("INFO\t\tTCS34725 color sensor found and succesfully initialized!"));}
    tcs.setInterrupt(true);
  } else {
    if(debug || debug_important){Serial.println(F("CRITICAL\tError while initializing the TCS34725 sensor! Program will continue nontheless..."));}
  }
}

void initializeSteppers() {
  stepper1.setMaxSpeed(3200);
  stepper1.setAcceleration(500000);
  stepper2.setMaxSpeed(5000);
  stepper2.setAcceleration(1e5);
  if(debug){Serial.println(F("INFO\t\tStepper motors succesfully initialized!"));}
}

void initializePins() {
  // Stepper pin defintions and standard values
  pinMode(EN1, OUTPUT); digitalWrite(EN1, HIGH);
  pinMode(EN2, OUTPUT); digitalWrite(EN2, HIGH);
  pinMode(STEP1, OUTPUT); digitalWrite(STEP1, LOW);
  pinMode(STEP2, OUTPUT); digitalWrite(STEP2, LOW);
  pinMode(DIR1, OUTPUT); digitalWrite(DIR1, HIGH);
  pinMode(DIR2, OUTPUT); digitalWrite(DIR2, HIGH);
  pinMode(LED_BUILTIN, OUTPUT); // pin 13

  // The microswitch inputs are pulled up internally, switch itself must be connected to ground to be pulled LOW
  pinMode(MICRO1, INPUT_PULLUP);
  pinMode(MICRO2, INPUT_PULLUP);

  // Button pins are initialized in the same way as the microswitch pins
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  
  if(debug){Serial.println(F("INFO\t\tPins succesfully initialized!"));}
}

double arrayMax(double array[], int sizeOf) {
  double maximum = array[0];

  for(int i = 1; i < sizeOf; i++) {
    if(array[i] > maximum) {
      maximum = array[i];
    }
  }

  return maximum;
}

double arrayMin(double array[], int sizeOf) {
  double minimum = array[0];

  for(int i = 1; i < sizeOf; i++) {
    if(array[i] < minimum) {
      minimum = array[i];
    }
  }

  return minimum;
}

int getStepDistance(ItemColor nextcolor, ItemColor prevcolor) {
  // Travel distance in steps
  int distance;
  // Container positions are based on eight-microstepping which means 1600 step transitions per revolution
  int containerPositions[6] = {134, 401, 668, 935, 1202, 1469};
  // M&Ms order: Red, Green, Blue, Yellow, Orange, Brown/Unknown
  // Skittles order: Red, Green, Yellow, Orange, Purple, Unknown
  if(candytype == mms) {
    switch(prevcolor) {
      case mm_red: carouselPrevPos = containerPositions[0]; break;
      case mm_green: carouselPrevPos = containerPositions[1]; break;
      case mm_blue: carouselPrevPos = containerPositions[2]; break;
      case mm_yellow: carouselPrevPos = containerPositions[3]; break;
      case mm_orange: carouselPrevPos = containerPositions[4]; break;
      case mm_brown: carouselPrevPos = containerPositions[5]; break;
      case unknown: carouselPrevPos = containerPositions[5]; break; // Unrecognized items are put in brown container    
    }
    switch(nextcolor) {
      case mm_red: carouselNextPos = containerPositions[0]; break;
      case mm_green: carouselNextPos = containerPositions[1]; break;
      case mm_blue: carouselNextPos = containerPositions[2]; break;
      case mm_yellow: carouselNextPos = containerPositions[3]; break;
      case mm_orange: carouselNextPos = containerPositions[4]; break;
      case mm_brown: carouselNextPos = containerPositions[5]; break;
      case unknown: carouselNextPos = containerPositions[5]; break; // Unrecognized items are put in brown container    
    }
  }
  if(candytype == skittles) {
    switch(prevcolor) {
      case sk_red: carouselPrevPos = containerPositions[0]; break;
      case sk_green: carouselPrevPos = containerPositions[1]; break;
      case sk_yellow: carouselPrevPos = containerPositions[2]; break;
      case sk_orange: carouselPrevPos = containerPositions[3]; break;
      case sk_purple: carouselPrevPos = containerPositions[4]; break;
      case unknown: carouselPrevPos = containerPositions[5]; break; // Unrecognized items go in this spare container
    }
    switch(nextcolor) {
      case sk_red: carouselNextPos = containerPositions[0]; break;
      case sk_green: carouselNextPos = containerPositions[1]; break;
      case sk_yellow: carouselNextPos = containerPositions[2]; break;
      case sk_orange: carouselNextPos = containerPositions[3]; break;
      case sk_purple: carouselNextPos = containerPositions[4]; break;
      case unknown: carouselNextPos = containerPositions[5]; break; // Unrecognized items go in this spare container
    }
  }
  
  distance = carouselNextPos - carouselPrevPos;

  if(debug){Serial.print(F("INFO\t\tcarouselPrevPos is ")); Serial.print(carouselPrevPos); Serial.print(F(", and carouselNextPos is "));
  Serial.print(carouselNextPos); Serial.print(F(". Will now order a movement of ")); Serial.print(distance);
  Serial.println(F(" steps on motor 2."));}
  
  return distance;
}

void sendSyncVariables() {
  sync.nextcolor = nextcolor;
  sync.prevcolor = prevcolor;
  sync.state = state;
  sync.candytype = candytype;

  ET.sendData(8);

  if(debug){Serial.println(F("INFO\t\tUpdated variables were sent to the lighting Arduino"));}
}

void receive(int numBytes) {
  if(I2COK == false) {
    I2COK = true;
    I2CMillis = Wire.read();
    if(debug){Serial.print(F("INFO\t\tI2C lighting slave was properly set up! This took "));
    Serial.print(I2CMillis);
    Serial.println(F(" millisecond(s)."));}
  } else {
    if(debug || debug_important){Serial.println(F("WARNING\t\tSlave encountered an unexpected reset! Possible system failure."));}
  }
}

int shortestPath(int distance) {
  int steps;
  
  // If the amount of steps to make is zero, do nothing and continue to item release
  if(distance == 0) {
    if(debug){Serial.println(F("INFO\t\tCarousel is already in position; no movement is required. Releasing item now."));}
  }
  
  // If CW turn angle is smaller than 180 degrees, configure motor to turn clockwise
  if(distance > 0 && distance <= 800) {
    steps = distance;
    if(debug){Serial.println(F("INFO\t\tCarousel is now moving to target position in clockwise direction."));}
  }
  
  // If CW turn angle is greater than 180 degrees, reverse direction and recalculate stepcount accordingly
  if(distance > 800) {
    steps = distance - 1600;
    if(debug){Serial.println(F("INFO\t\tCarousel is now moving to target position in counterclockwise direction."));}
  }
  
  // If the amount of steps is negative and greater than -800, reverse direction and make step count positive
  if(distance < 0 && distance >= -800) {
    steps = distance;
    if(debug){Serial.println(F("INFO\t\tCarousel is now moving to target position in counterclockwise direction."));}
  }
  
  // If the amount of steps is smaller than -800, go forward for 1600 - nSteps
  if(distance < -800) {
    steps = 1600 + distance;
    if(debug){Serial.println(F("INFO\t\tCarousel is moving to target position in clockwise direction."));}
  }

  return steps;
}

void printSummary() {
  // Extract the candy amounts from the counter array
  int red = amounts[5] + amounts[6],
      green = amounts[1] + amounts[7],
      blue = amounts[2],
      yellow = amounts[3] + amounts[8],
      orange = amounts[4] + amounts[9],
      brown = amounts[1],
      purple = amounts[10],
      total = red + blue + green + yellow + orange + brown + purple;
  
  // Calculate how long the sorting session took in minutes and seconds
  long sessionTime = sortTime;
  int seconds = sortTime / 1000;
  int minutes = 0;

  // Convert seconds to minutes + seconds
  while(seconds >= 60) {
    seconds -= 60;
    minutes++;
  }

  Serial.println(F("INFO\t\t-- Sorting summary: Candy amounts as registered --"));
  Serial.println(F("INFO"));

  // Print the amounts in aligned columns
  if(candytype == mms) {
    Serial.println(F("INFO\t\tRed\tGreen\tBlue\tYellow\tOrange\tBrown"));
    Serial.print(F("INFO\t\t")); Serial.print(red);
    Serial.print(F("\t")); Serial.print(green);
    Serial.print(F("\t")); Serial.print(blue);
    Serial.print(F("\t")); Serial.print(yellow);
    Serial.print(F("\t")); Serial.print(orange);
    Serial.print(F("\t")); Serial.println(brown);
  } else {
    Serial.println(F("INFO\t\tRed\tGreen\tYellow\tOrange\tPurple"));
    Serial.print(F("INFO\t\t")); Serial.print(red);
    Serial.print(F("\t")); Serial.print(green);
    Serial.print(F("\t")); Serial.print(yellow);
    Serial.print(F("\t")); Serial.print(orange);
    Serial.print(F("\t")); Serial.println(purple);
  }

  Serial.println(F("INFO"));

  // Print how long it took and the total amount of candy sorted in this session
  Serial.print(F("INFO\t\tSorted a total of ")); Serial.print(total);
  Serial.print(F(" pieces of candy in ")); Serial.print(minutes);
  if(minutes != 1) {
    Serial.print(F(" minutes and "));
  } else {
    Serial.print(F(" minute and "));
  }
  Serial.print(seconds);
  if(seconds != 1) {
    Serial.println(F(" seconds!"));
  } else {
    Serial.println(F(" second!"));
  }
  Serial.println(F("INFO\t\t-- End summary --"));
}

