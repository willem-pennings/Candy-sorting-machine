/*
 * The sorting process
 */

void sortingProcess() {
  // Initialize these variables once at program start
  static int nColorSample = 0;
  static int measuredColor[3];
  static int HSVColor[3];
  static int consecUnknown = 0;
  static int reverseCounter = 0;
  static ItemColor foundColor[3] = {unknown, unknown, unknown};
  static int carouselDistance;
  static bool init_standby = false;
  static bool init_TCS_led = false;
  static bool init_reverse = false;
  
  switch(state) {
    case standby:   
      if(init_standby == false) {
        // Calibrate the measurement wheel motor (if necessary)
        if(!opFix) {
          // If an operator fix is required, do not calibrate the motor since it won't work anyway
          calibrateFeedMotor();
        }

        // Cut motor power
        digitalWrite(EN1, HIGH);
        digitalWrite(EN2, HIGH);

        // Send the sync variables to the second Arduino
        sendSyncVariables();

        // Disable the LED on the color sensor
        tcs.setInterrupt(true);

        // Print a candy summary if candy was sorted before going into standby
        if(candyNumber != 0) {
          if(debug || debug_important){
            printSummary();
          }

          // Reset the candy counter array for next session
          for(int i = 0; i < 11; i++) {
            amounts[i] = 0;
          }
        }
      
        if(debug || debug_important){Serial.println(F("INFO\t\tMachine is now in stand-by mode."));}
        init_standby = true;
        break;
      } else {
        break;
      }
      
    case loaditem:
      // Reset the standby initialization boolean
      init_standby = false;

      // Check if the measurement wheel is stuck
      if((millis() - loadStartTime) > maxLoadTime) {
        state = reverse;
        reverseCounter++;

        if(reverseCounter == 3) {
          state = standby;
          opFix = true;
          if(debug || debug_important){Serial.println(F("CRITICAL\tAutomatic unjamming is not working - manual repairs required! Stopping machine now!"));}
        }
        break;
      }

      // If the microswitch hits the indent, stop the motor
      if(digitalRead(MICRO1) == LOW && (millis() - loadStartTime) > 50) {
        stepper1.setSpeed(0);
        state = analyzecolor;
        candyNumber++;
        break;
      } else {
        break;
      }
      
    case analyzecolor:
      // Reset the jam reverse counter
      reverseCounter = 0;
      // Take three samples before making conclusions about the item color
      if(nColorSample < 3) {
        // Delay first measurement to stabilize sensor
        if (nColorSample == 0) {
          delay(50);
        }
        
        // Delay between measurements without using delay()
        if(millis() - measurementTime > timeBetweenMeasurements) {
          if(debug){Serial.println(F("INFO\t\tWill now take a color sample since n < 3."));}
          // Measure R,G,B color on the TCS34725, store values in the measuredColor array
          measureColor(measuredColor);
          // Convert the measured R,G,B to H,S,V for easier analysis
          RGBtoHSV(measuredColor, HSVColor);
          // Store the detected color (enum value) in an array
          foundColor[(nColorSample)] = determineColor(HSVColor);
          nColorSample++;
          
          measurementTime = millis();
        }
        break;
      } else {
        // If three measurements were taken, reset the counter and switch into the next state
        nColorSample = 0;
        state = determinecolor;
        
        if(debug){
          Serial.println(F("INFO\t\tThree samples were taken. Next state is determinecolor."));
        }
      }
      
    case determinecolor:
      // The previous color is equal to the determined color in the last cycle, so update it before it gets overwritten
      prevcolor = nextcolor;

      // If any two measurements match, conclude that the color was determined succesfully
      if(foundColor[0] == foundColor[1]) {
        finalcolor = foundColor[0];
      } else if(foundColor[1] == foundColor[2]) {
        finalcolor = foundColor[1];
      } else if(foundColor[2] == foundColor[0]) {
        finalcolor = foundColor[2];
      } else {
        // If none of the measurements match, the color must be unknown
        finalcolor = unknown;
      }

      // Update the nextcolor variable to match the determined color
      nextcolor = finalcolor;

      if(debug || debug_important){
        // Print the detected color
        switch(finalcolor) {
          case mm_red: Serial.println(F("INFO\t\tCurrent piece is determined to be a RED M&M!")); break;
          case mm_green: Serial.println(F("INFO\t\tCurrent piece is determined to be a GREEN M&M!")); break;
          case mm_blue: Serial.println(F("INFO\t\tCurrent piece is determined to be a BLUE M&M!")); break;
          case mm_yellow: Serial.println(F("INFO\t\tCurrent piece is determined to be a YELLOW M&M!")); break;
          case mm_orange: Serial.println(F("INFO\t\tCurrent piece is determined to be a ORANGE M&M!")); break;
          case mm_brown: Serial.println(F("INFO\t\tCurrent piece is determined to be a BROWN M&M!")); break;
      
          case sk_red: Serial.println(F("INFO\t\tCurrent piece is determined to be a RED Skittle!")); break;
          case sk_green: Serial.println(F("INFO\t\tCurrent piece is determined to be a GREEN Skittle!")); break;
          case sk_yellow: Serial.println(F("INFO\t\tCurrent piece is determined to be a YELLOW Skittle!")); break;
          case sk_orange: Serial.println(F("INFO\t\tCurrent piece is determined to be a ORANGE Skittle!")); break;
          case sk_purple: Serial.println(F("INFO\t\tCurrent piece is determined to be a PURPLE Skittle!")); break;
      
          case unknown: Serial.println(F("WARNING\t\tCurrent piece was not recognized and was marked as unknown!")); break;
        }
      }

      // Keep track of the amounts of sorted candy by incrementing elements in amounts[]
      for(int i = 0; i < 12; i++) {
        if(nextcolor == (ItemColor)i) {
          amounts[i]++;
        }
      }

      // Send the sync variables to the second Arduino for lighting
      sendSyncVariables();

      // If a color is unknown, we must start counting to check if the hopper is empty
      if(finalcolor == unknown) {
        consecUnknown++;
        if(debug){Serial.println(F("WARNING\t\tMost recent piece of candy was not recognized. Next state is setreleasepos."));}
        state = setreleasepos;
        // If the amount of unknowns in a row is greater than a predefined value, the hopper must be empty
        if(consecUnknown == maxconsecUnknown) {
          // Switch into standby mode and reset counter; await user input
          state = standby;
          consecUnknown = 0;
          if(debug || debug_important){Serial.print(F("CRITICAL\t")); Serial.print(maxconsecUnknown); Serial.println(F(" pieces of candy were registered as unknown. Exiting!"));}
        }
        break;
      } else {
        // Most recent two pieces of candy were not registered as unknown.
        consecUnknown = 0;
        state = setreleasepos;
        break;
      }
      
    case setreleasepos:
      // Set the release position for the carousel in terms of the amount of steps to go and the direction

      // Get the amount of steps to make
      carouselDistance = shortestPath(getStepDistance(nextcolor,prevcolor));

      // Configure the motor direction based on the determined direction
      if(!lastRound) {
        // Machine might jump to 'blue' container after pressing the button; this check prevents that
        stepper2.setCurrentPosition(0);
        stepper2.runToNewPosition(carouselDistance);
      }
      state = releaseitem;
      break;
      
    case releaseitem:
      // Start dropping the piece of candy before the carousel is done turning to improve speed & performance
      if(stepper2.distanceToGo() == 0) {
        if(debug){Serial.println(F("INFO\t\tThe carousel has been positioned; feed wheel will drop item now."));}

        if(lastRound) {
          // If the standby button was pressed, finish up by putting the machine in standby mode and resetting consecUnknown
          state = standby;

          // Calculate how long this sorting session took
          sortTime = millis() - sortTime;
          
          consecUnknown = 0;
          lastRound = false;
        } else {
          // Else, continue normal operation and setup to load a new piece of candy
          stepper1.setSpeed(3000);
          state = loaditem;

          // Set up a timer to register the time needed to load a piece of candy
          loadStartTime = millis();
        }
        break;
      } else {
        break;
      }
    
    case reverse:
      // Reverse the wheel 90 degrees in case of a jam
      if(init_reverse == false) {
        if(debug || debug_important){Serial.println(F("CRITICAL\tDetected that the measurement wheel is stuck. Reversing wheel now."));}
        init_reverse = true;
      }

      stepper1.setCurrentPosition(0);
      stepper1.runToNewPosition(-400);

      init_reverse = false;
      calibrateFeedMotor();
      
      stepper1.setSpeed(3200);
      state = loaditem;
      loadStartTime = millis();
      break;
  }
}
