void buttonHandler() {
  // Handle button 1 input (machine on/off toggle)
  if((digitalRead(BUTTON2) == HIGH && millis() - buttonTime > 1000) || (char)Serial.read() == 's') {
    if(state == standby) { // START MACHINE
      digitalWrite(EN1, LOW);
      digitalWrite(EN2, LOW);
      
      // We will assume there is a piece of candy in the slot when starting up, so turn on the sensor and delay 120ms.
      tcs.setInterrupt(false);
      delay(120);
      state = loaditem;
      loadStartTime = millis();
      if(debug){Serial.println(F("INFO\t\tStart command has been issued. Machine will now be started!"));}
  
      // Start the timer that registers how long the sorting session will take
      sortTime = millis();
    } else { // STOP MACHINE
      tcs.setInterrupt(true);
      if(debug){Serial.println(F("INFO\t\tStop command has been issued. Machine will be stopped after cycle completion."));}
      lastRound = true;
    }
    buttonTime = millis();
  }
  
  // Handle button 2 input (mms/skittles toggle)
  if(digitalRead(BUTTON1) == HIGH && millis() - buttonTime > 1000 && state == standby) {
    if(candytype == mms) {
      // If the candytype is M&M's, switch to Skittles
      candytype = skittles;
      if(debug){Serial.println(F("INFO\t\tCandy type is now Skittles!"));}
    } else if(candytype == skittles) {
      // If the candytype is Skittles, switch to M&M's
      candytype = mms;
      if(debug){Serial.println(F("INFO\t\tCandy type is now M&Ms!"));}
    }
    buttonTime = millis();
    sendSyncVariables();
  }
}
