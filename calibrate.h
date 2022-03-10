#ifndef CALIBRATE
  // disable this if you don't want to have the mic calibration wizard
  #define CALIBRATE
#endif

/***
 * Used for calibration wizard...takes sample readings from input
 */
#ifdef CALIBRATE
 
  void sampleMic(const char *prompt, float &avg, float &loBase, float &hiBase) {
    Serial.println(prompt);
    Serial.print(F("Listening in "));
    for (int i = 10; i >= 0; i--) {
      Serial.print(i);
      if (i > 0) {
        Serial.print(F(" > "));
        delay(1000);
      }
    }
    Serial.println(F(""));
    loBase = 0;
    hiBase = 0;
    avg = 0;
    float total = 0;
    int count = 0;
    elapsedMillis timer = 0;
    Serial.print(F("Listening for 10 seconds..."));
    voiceOn();
    while (timer < 10000) {
      if (rms1.available()) {
          // get the input amplitude...will be between 0.0 (no sound) and 1.0
          float val = rms1.read();
          if (val > 0) {
            count++;
            if (val < loBase) {
              loBase = val;
            }
            if (val > hiBase) {
              hiBase = val;
            }
            total += val;
          }
          delay(10);
      }
    }
    voiceOff();
    Serial.print("Done (");
    Serial.print(count);
    Serial.println(" samples)");
    avg = total/count;
  }
  
  /***
   * Calibration wizard
   */
  void calibrate() 
  {
  
    // disable normal operation
    App.state = STATE_BOOTING;  
  
    char recom[SETTING_ENTRY_MAX*2];
    
    loopOff();
  
    Serial.println(F("CALIBRATING...Please make sure your microphone is on!"));
    Serial.println(F(""));
  
    float avg = 0;
    float loBase = 0;
    float hiBase = 0;
  
    char buf[SETTING_ENTRY_MAX];
  
    sampleMic("Please speak normally into your microphone when the countdown reaches 0", avg, loBase, hiBase);
    Serial.print("Average Trigger level: ");
    dtostrf(avg, 0, 4, buf);
    Serial.print(buf);
    Serial.print(" (Low: ");
    memset(buf, 0, sizeof(buf));
    dtostrf(loBase, 0, 4, buf);
    Serial.print(buf);
    Serial.print(" Peak: ");
    memset(buf, 0, sizeof(buf));
    dtostrf(hiBase, 0, 4, buf);
    Serial.print(buf);
    Serial.println(")");
    memset(buf, 0, sizeof(buf));
    avg += .01;
    strcpy(recom,  "voice_start=");
    dtostrf(avg, 0, 4, buf);
    strcat(recom, buf);
    strcat(recom, "\n");
    memset(buf, 0, sizeof(buf));
    
    Serial.println("");
    sampleMic("Please leave your microphone on and keep silent so that we can get a baseline...", avg, loBase, hiBase);
    Serial.println("Average Baseline level: ");
    dtostrf(avg, 0, 4, buf);
    Serial.print(buf);
    Serial.print(" (Low: ");
    memset(buf, 0, sizeof(buf));
    dtostrf(loBase, 0, 4, buf);
    Serial.print(buf);
    Serial.print(" Peak: ");
    memset(buf, 0, sizeof(buf));
    dtostrf(hiBase, 0, 4, buf);
    Serial.print(buf);
    Serial.println(")");
    memset(buf, 0, sizeof(buf));
    
    Serial.println("");
    
    if (avg < 0.01) {
      avg = 0.01;
    }
    strcat(recom, "voice_off=");
    dtostrf(avg, 0, 4, buf);
    strcat(recom, buf);
    strcat(recom, "\n");
    memset(buf, 0, sizeof(buf));
  
    Serial.println(F(""));
    showFile("CALIBRATE.TXT");
    Serial.println(F(""));
    Serial.println(recom);
    Serial.println(F(""));
  
    loopOn();
    
    App.state = STATE_RUNNING;
    
  }
#endif
