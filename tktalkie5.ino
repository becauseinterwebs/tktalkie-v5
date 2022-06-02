/****
 * TK TALKIE by TK-81113 (Brent Williams) <becauseinterwebs@gmail.com>
 * www.tktalkie.com / www.tk81113.com 
 * 
 * Version 5.1
 *
 * WhiteArmor.net User ID: lerxstrulz
 * 
 * This sketch is meant to use a Teensy 3.2 with a Teensy Audio Shield and 
 * reads sounds from an SD card and plays them after the user stops talking 
 * to simulate comm noise such as clicks and static. This version adds a lot  
 * of new features, including the ability to be controlled via a mobile app 
 * and Bluetooth.  This release also introduces memory optimizations and other 
 * improvements.
 * 
 * You are free to use this code in your own projects, provided they are for 
 * personal, non-commercial use.
 * 
 * The audio components and connections were made using the GUI tool 
 * available at http://www.pjrc.com/teensy/gui.  If you want to modify 
 * the signal path, add more effects, etc., you can copy and paste the 
 * code marked by 'GUITool' into the online editor at the above URL.
 * 
 * WHAT'S NEW:
 * 
 * v5.1 (06/02/2022)
 *  1. Added check for wake button in sleep check.  If no wake button is programmed,
 *     it will not go to sleep.
 *  2. Added sleep timeout reset to button press events (previously only on voice events.)
 *  
 * v5.0 (03/09/2022)
 *  1.  Updating sketch to work with Teensy 4.0.
 *  
 * v4.3 (04/29/18)
 *  1.  Reorganized include files for easier management. 
 *  2.  Upgraded ArduinoJson to version 6.
 *  
 * v4.2 (12/17/18)
 *  1.  Added ability to play random sound on PTT button release 
 *      (previously only random or none)
 *  2.  Added ability to play fixed, random or no sound when mic is
 *      activated in VA mode (prevously no sound played.)
 *  3.  Added ability to play fixed sound (previously only random or none)
 *      when user stops talking in VA mode.
 *  
 * v4.1 (12/10/18)
 *  1.  Misc bug fixes regarding loading profiles
 *  
 * v4.0 (12/03/17)
 *  1.  Added Chorus and Flange processing for the voice channel.
 *  2.  Added a dry (unprocessed) voice channel.
 * 
 * V3.15 (10/20/17)
 *  1.  Add support for 3 sound-glove buttons.
 *  
 * V3.14 (10/10/17)
 *  1.  Added AUTO SLEEP function to put the TK-Talkie to sleep to conserve
 *      battery if no sound is heard from the microphone for a pre-configured 
 *      amount of time (configurable by the user.)
 * 
 * V3.13 (10/1/17)
 *  1.  Add mute_effects option to stop effects from playing after talkings
 *  
 *  2.  Startup sound, if specified, will always play even if effects level is 0
 *  
 *  3.  Allowed for empty effects.dir option
 *  
 *  4.  Added App.app_ver so sketch will know what version of the APP is accessing
 *      it.  This is for future use.
 *  
 * V3.1 (5/30/2017)
 *  1.  Added new setting to the CONFIG.TXT control file to alert the 
 *      mobile app which input/volume controls should be presented.
 *      Valid values are:
 *        MIC  = Show the microphone jack controls only
 *        LINE = Show the line-in controls only 
 *        BOTH = Show mic/line-in controls and input switcher
 *      By default this is set to BOTH for backwards compatibility.  As of 
 *      version 3.1 of the TKTalkie hardware, inputs are wired to mic only
 *      as it will support both wired and wireless microphones.
 *      
 *  2.  Added new settings to the CONFIG.TXT control file to alert the     
 *      mobile app which output/volume controls should be presented.
 *      Valid values are:
 *         SPKR = Show only headphones/speaker controls 
 *         LINE = Show only line-out controls
 *         BOTH = Show both
 *      By default this is set to BOTH for backwards compatibility
 *      
 *  3.  Add new option (all versions) to allow background loop to continue 
 *      playing while talking (loop is paused by default)
 *      
 *  4.  Fixed bug that prevented PTT button setting up properly.
 *  
 * V3.0 (4/10/2017)
 *  1.  Modified to be able to communicate via Bluetooth Low Energy (BLE) 
 *      serial adapter with mobile app for control of settings and profile 
 *      switching.
 *  2.  Began optimizations on usage of C-type strings to reduce memory usage.
 *  3.  Added new commands specific to BLE adapter usage.
 *  4.  Added new commands to manage multiple configuration profiles.
 *  5.  Reorganized SD card files and folders
 *  
 *  Please visit http://www.tktalkie.com/changelog for prior changes.
 */

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Bounce2.h>
#include <ArduinoJson.h>
#include "setup.h"
#include "ControlButton.h"
#include "globals.h"
#include "utils.h"
#include "files.h"
#include "sounds.h"
#include "calibrate.h"
#include "settings.h"
#include "ble.h"
#include "main.h"

// Initial setup...runs only once when the board is turned on.
void setup() {

  App.state = STATE_BOOTING;
  
  // Serial connection for debugging
  //Serial.begin(9600);
  
  delay(250);

  // Always allocate memory for the audio shield!
  AudioMemory(16);
  
  // turn on audio shield
  audioShield.enable();
  
  // disable volume and outputs during setup to avoid pops
  audioShield.muteLineout();
  audioShield.muteHeadphone();
  audioShield.volume(0);

  // turn on post processing
  audioShield.audioPostProcessorEnable();

  // Initialize sound generator for warning tones when we switch modes from PTT back to Voice Activated
  waveform1.begin(WAVEFORM_SINE);
  
  // Check SD card
  // CS Pin is 10...do not change for Teensy!
  if (!(SD.begin(10))) {
     usb.println("Unable to access the SD card.");
     usb.println("Using default built-in profile");
     App.SD_OK = false;
     beep(2);
  }
  // this just makes sure we get truly random numbers each time
  // when choosing a file to play from the list later on...
  randomSeed(analogRead(0));

  // wait half a sec to give everything time to power up
  delay(500);

  // set volume of effects feed into master mixer
  voiceMixer.gain(3, 1);

  // load startup settings
  startup();

  usb.println("TK-Talkie started!");
}

void loop() 
{
  switch (App.state) {
    case STATE_NONE:
    case STATE_BOOTING:
    case STATE_SLEEPING:
      // do nothing
      break;
    case STATE_RUNNING:
      // normal program operation
      run();
      break;
  }
}
