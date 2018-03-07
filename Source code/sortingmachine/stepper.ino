/*
 * Global stepper movement functions
 */

void calibrateFeedMotor() {
  if(debug){Serial.println(F("INFO\t\tFeed motor will now be calibrated."));}

  // Enable feed motor
  digitalWrite(EN1, LOW);

  stepper1.setSpeed(1000);
  
  while(digitalRead(MICRO1) != 0) {
    stepper1.runSpeed();
  }

  stepper1.stop();
  stepper1.setSpeed(0);
  
  if(debug){Serial.println(F("INFO\t\tMotor 1 has been calibrated!"));}
}
