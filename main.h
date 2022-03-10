// Main execution block


// Perform a restart without having to cycle power
#define RESTART_ADDR       0xE000ED0C
#define READ_RESTART()     (*(volatile uint32_t *)RESTART_ADDR)
#define WRITE_RESTART(val) ((*(volatile uint32_t *)RESTART_ADDR) = (val))

#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__

int freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}
// END

void showMemory() {
    Serial.print("Proc = ");
    Serial.print(AudioProcessorUsage());
    Serial.print(" (");    
    Serial.print(AudioProcessorUsageMax());
    Serial.print("),  Mem = ");
    Serial.print(AudioMemoryUsage());
    Serial.print(" (");    
    Serial.print(AudioMemoryUsageMax());
    Serial.println(")");
    Serial.print("Free Mem: ");
    Serial.println(freeMemory());
}


void softreset() {
 WRITE_RESTART(0x5FA0004);
}

// Check if the PTT button was pressed 
bool checkPTTButton() 
{
  if (App.ptt_button == BUTTON_UNSET) {
    //debug(F("No PTT button specifed...returning\n"));
    return false;
  }
  Settings.glove.ControlButtons[App.ptt_button].update();
  if (Settings.glove.ControlButtons[App.ptt_button].fell()) {
    if (App.state == STATE_RUNNING) {
      sendButtonPress(App.ptt_button, 1);
      playCommEffect(Settings.sounds.button);
    }
    return true;
  } else {
    return false;
  }
}

bool buttonHeld(unsigned long msecs) {
    elapsedMillis duration = 0;
    if (App.wake_button == BUTTON_UNSET) {
      return false;
    }
    while (duration < msecs) {
        Settings.glove.ControlButtons[App.wake_button].update();
        if (Settings.glove.ControlButtons[App.wake_button].read() != 0) {
            return false;
        }
    }
    return true;
}

void gotoSleep() {
  App.state = STATE_SLEEPING;
  if (loopPlayer.isPlaying()) {
    loopPlayer.stop();
  }
  long l = playSound(Settings.sleep.file);
  delay(l+250);
  SLEEP:
    Settings.glove.ControlButtons[App.wake_button].update();
    //pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    elapsedMillis timeout = 0;
    while (digitalRead(LED_BUILTIN) > 0 && timeout < 2000) {
      digitalWrite(LED_BUILTIN, LOW);
    }
    //Snooze.hibernate( config_teensy40 );
    timeout = 0;
    // debouce set to 15ms, so have to wait and check button status
    while (timeout < 16) Settings.glove.ControlButtons[App.wake_button].update();
    bool awake = buttonHeld(100);
    if (!awake) goto SLEEP;
    softreset();
}

// Read the CONFIG.TXT file
void startup() 
{

  File file = SD.open(CONFIG_FILE);

  const size_t bufferSize = JSON_ARRAY_SIZE(6) + JSON_OBJECT_SIZE(8) + 170;
  DynamicJsonDocument root(bufferSize);
  
  DeserializationError err = deserializeJson(root, file);

  file.close();

  if (err) {
    Serial.println(F("Failed to read file, using default configuration."));
    Serial.print("Deserialization error: "); Serial.println(err.c_str());
    strlcpy(Config.profile, "DEFAULT.TXT", sizeof(Config.profile)); // "123456789012"
    strlcpy(Config.access_code, "1138", sizeof(Config.access_code)); // "1111111111111111111111111"
    strlcpy(Config.profile_dir, "/profiles/", sizeof(Config.profile_dir));
    Config.debug = 1;
    Config.baud  = 9600;
    strlcpy(Config.input,"BOTH", sizeof(Config.input)); // "both"
    Config.echo = 1;
    Config.buttons[0] = 0; // 1
    Config.buttons[1] = 0; // 2
    Config.buttons[2] = 0; // 3
    Config.buttons[3] = 0; // 4
    Config.buttons[4] = 0; // 5
    Config.buttons[5] = 0; // 6
  } else {
    strlcpy(Config.profile, (root["profile"] | BLANK), sizeof(Config.profile)); // "123456789012"
    strlcpy(Config.access_code, (root["access_code"] | "1138"), sizeof(Config.access_code)); // "1111111111111111111111111"
    strlcpy(Config.profile_dir, (root["profiles"] | "/profiles/"), sizeof(Config.profile_dir)); // "1111111111111111111111111"
    //Config.debug = ((root["debug"] | 0) == 1) ? true : false; // 1
    Config.debug = root["debug"];
    strlcpy(Config.input, (root["input"] | "BOTH"), sizeof(Config.input)); // "both"
    //Config.echo = ((root["echo"] | 0) == 1) ? true : false; // 0
    Config.echo = root["echo"];
    Config.baud = root["baud"] | 9600;
    JsonArray buttons = root["buttons"];
    Config.buttons[0] = buttons[0]; // 1
    Config.buttons[1] = buttons[1]; // 2
    Config.buttons[2] = buttons[2]; // 3
    Config.buttons[3] = buttons[3]; // 4
    Config.buttons[4] = buttons[4]; // 5
    Config.buttons[5] = buttons[5]; // 6
  }

  debug(F("Got startup  value Config.baud: %d\n"), Config.baud);
  debug(F("Got startup value Config.debug: %d\n"), Config.debug);
  debug(F("Got startup value Config.profile: %s\n"), Config.profile);
  debug(F("Got startup  value Config.echo: %d\n"), Config.echo);
  if (strcasecmp(Config.input, BLANK) == 0) {
    strlcpy(Config.input, "BOTH", sizeof(Config.input));
  }
  debug(F("Got startup value Config.input: %s\n"), Config.input);
  debug(F("Got startup value Config.access_code: %s\n"), Config.access_code);

  if (strcasecmp(Config.profile, BLANK) == 0) {
    Serial.println("Reading files");
    // No profile specified, try to find one and load it
    char files[MAX_FILE_COUNT][FILENAME_SIZE];
    byte total = listFiles(Config.profile_dir, files, MAX_FILE_COUNT, FILE_EXT, false, false);
    if (total > 0) {
      memset(Config.profile, 0, sizeof(Config.profile));
      strcpy(Config.profile, files[0]);
    } else {
      debug(F("No profiles found\n"));
    }
  }
  
  if (strcasecmp(Config.profile, BLANK) == 0) {
    debug(F("NO PROFILE FILE FOUND...USING DEFAULTS!\n"));
    strlcpy(Config.profile, "DEFAULT.TXT", sizeof(Config.profile));
  } else {
    debug(F("PROFILE: %s\n"), Config.profile);
  }

  if (strcasecmp(Config.input, "linein") == 0) {
    audioShield.inputSelect(AUDIO_INPUT_LINEIN);
  } else {
    audioShield.inputSelect(AUDIO_INPUT_MIC);
  }

  debug(F("Read access code %s\n"), Config.access_code);

  Serial.println(F("\n----------------------------------------------"));
  Serial.print(F("TKTalkie v"));
  Serial.println(VERSION);
  Serial.println(F("(c) 2022 TK81113/Because...Interwebs!\nwww.TKTalkie.com"));
  Serial.print(F("Debugging is "));
  Serial.println(Config.debug == 0 ? "OFF" : "ON");
  if (Config.debug == false) {
    Serial.println(F("Type debug=1 [ENTER] to enable debug messages"));
  } else {
    Serial.println(F("Type debug=0 [ENTER] to disable debug messages"));
  }
  Serial.println(F("----------------------------------------------\n"));

  // Load settings from specified file
  loadSettings(Config.profile, &Settings, false);
  
  // apply the settings so we can do stuff
  applySettings();
  
  voiceOff();
  
  // set the volume, either by config or volume pot
  readVolume();

  // turn on outputs
  audioShield.unmuteLineout();
  audioShield.unmuteHeadphone();

  float prevVol = Settings.effects.volume;

  // turn on volume for startup sound 
  // if effects volume is at 0
  if (prevVol <= 0) {
    effectsMixer.gain(0, 1);
    effectsMixer.gain(1, 1);
  }

  // play startup sound
  long l = playSound(Settings.sounds.start);

  // reset mixer volume if set to 0
  if (prevVol <= 0) {
    effectsMixer.gain(0, Settings.effects.volume);
    effectsMixer.gain(1, Settings.effects.volume);
  }

  // add a smidge of delay ;)
  delay(l+100); 

  // play background loop
  playLoop();
  
  App.state = STATE_RUNNING;

 // Serial1.begin(Config.baud);
  delay(250);

}

// Called from main loop
elapsedMillis checkMillis = 0;
byte lastButton = 0;
byte lastControlButton = 0;
// loop and serial command handlers
char cmd_key[15] = BLANK;
char cmd_val[MAX_DATA_SIZE] = BLANK;
char received[SETTING_ENTRY_MAX] = BLANK;
  
void run() {

  // check loop
  if (App.state != STATE_RUNNING) return;
  
  if (App.loopLength > 0 && App.loopMillis > App.loopLength) {
      playLoop();
  }

  // check if loop was muted while a sound played
  if (App.muteLoopTimeout > 0) {
    if (App.muteLoopMillis >= App.muteLoopTimeout) {
      App.muteLoopMillis = 0;
      loopOn();  
    }
  } else {
    App.muteLoopMillis = 0;
  }
  
  if (Serial.available()) { 
    Serial.readBytesUntil('\n', received, MAX_DATA_SIZE);
    memset(cmd_key, 0, sizeof(cmd_key));
    memset(cmd_val, 0, sizeof(cmd_val));
    char *key, *val, *buf;
    key = strtok_r(received, "=", &buf);
    val = strtok_r(NULL, "=", &buf);
    strcpy(cmd_key, key);
    if (val) {
      strcpy(cmd_val, val);
    }
    debug(F("command %s, value %s\n"), cmd_key, cmd_val);
    App.autoSleepMillis = 0;
  } else if (Serial1.available() > 0) {
    char *key, *val, *buf, *buf2, *uid;      
    Serial1.readBytesUntil('\n', received, MAX_DATA_SIZE);
    debug(F("RX: %s\n"), received);
    // get command...will be int the format: cmd=UUID|value
    key = strtok_r(received, "=", &buf);
    // get other side of string after the '='
    val = strtok_r(NULL, "=", &buf);
    // extract device id, then "val" will hold the rest of the command
    uid = strtok_r(val, CMD_SEPARATOR, &buf2);
    if (uid != NULL) {
      debug(F("Received UID %s\n"), uid);
    } else {
      debug(F("Received NULL UID!"));
    }
    if (App.device_id != NULL) {
      debug(F("Current device id: %s\n"), App.device_id);
    } else {
      debug(F("Current device id is NULL"));
    }
    // if there is no device id sent...no soup for you!
    if (uid == NULL || strcasecmp(uid, BLANK) == 0) {
        App.ble_connected = true;
        debug(F("Invalid device id: %s\n"), uid);
        sendToApp("access", CONNECT_BAD_UID);
        App.ble_connected = false;
    } else {
        // store the command
        strcpy(cmd_key, key);
        // the rest of "val" has the remaining command parameters
        strcpy(cmd_val, buf2);
        // if the ble is already connected, make sure we are still talking
        // to the same device!
        if (App.ble_connected && strcmp(uid, App.device_id) != 0) {
            // this is not the same user...
            debug(F("Device in use\n"));
            memset(cmd_key, 0, sizeof(cmd_key));
            //App.ble_connected = true;
            //sendToApp("access", CONNECT_IN_USE);
            //App.ble_connected = false;
        } else {
          // store the device id
          if (strcasecmp(App.device_id, BLANK) == 0) {
            strcpy(App.device_id, uid);
          }
          App.autoSleepMillis = 0;
          debug(F("BLE Cmd: %s Value: %s Uid: %s\n"), cmd_key, cmd_val, uid);
      }
    }
  }
  
  if (strcasecmp(cmd_key, BLANK) != 0) {
      // translate character command into a number for 
      // faster processing
      byte cmdIdx = getCommand(cmd_key);
      debug(F("Got command index %d\n"), cmdIdx);
      switch (cmdIdx) {
        case CMD_CONNECT:
            {
              // validate data received from mobile device!
              char *pwd, *ver, *buf;
              // get the access code
              pwd = strtok_r(cmd_val, CMD_SEPARATOR, &buf);
              debug(F("Received access code %s\n"), pwd);
              if (pwd == NULL || strcmp(pwd, Config.access_code) != 0) {
                debug(F("Invalid access code: %s\n"), pwd);
                App.ble_connected = true;
                sendToApp("access", CONNECT_BAD_PWD);
                App.ble_connected = false;
              } else {
                // get the version of the app accessing the device
                ver = strtok_r(NULL, CMD_SEPARATOR, &buf);
                debug(F("Received app version %s\n"), ver);
                if (ver == NULL || strcasecmp(ver, BLANK) == 0) {
                  strcpy(ver, "0");
                }
                if (atof(ver) < MIN_APP_VER) {
                  // App is below min required version
                  debug(F("App below required version: %s\n"), ver);
                  memset(App.device_id, 0, sizeof(App.device_id));
                  App.ble_connected = true;
                  sendToApp("access", CONNECT_BAD_VER);
                  App.ble_connected = false;
                } else {
                  // All ok!
                  berp();
                  App.ble_connected = true;
                  debug(F("Access granted\n"));
                  sendToApp("access", CONNECT_SUCCESS);
                }
              }
            }
            break;
         case CMD_DISCONNECT:
            debug(F("Sending discconnect to app"));
            sendToApp("disconnect", 1);
            App.ble_connected = false;
            beep(2);
            memset(App.device_id, 0, sizeof(App.device_id));
            break;
         case CMD_DOWNLOAD:
            sendConfig();
            break;
         case CMD_CONFIG:
            {
              showFile(CONFIG_FILE);
            }
            break; 
         case CMD_SAVE:
            {
              char *pfile;
              if (strcasecmp(cmd_val, BLANK) != 0) {
                  char *ptr, *pname;
                  pfile = strtok_r(cmd_val, ";", &ptr);
                  if (strcasecmp(pfile, BLANK) != 0) {
                    memset(Settings.file, 0, sizeof(Settings.file));
                    strcpy(Settings.file, pfile);
                  }
                  pname = strtok_r(NULL, ";", &ptr);
                  if (strcasecmp(pname, BLANK) != 0) {
                    memset(Settings.name, 0, sizeof(Settings.name));
                    strcpy(Settings.name, pname);
                  }
               }
               addFileExt(Settings.file);
               debug(F("Save settings file %s with description %s\n"), Settings.file, Settings.name);
               boolean wasPlaying = false;
               if (loopPlayer.isPlaying()) {
                  wasPlaying = true;
                  loopPlayer.stop();
               }
               if (saveSettings(Settings.file, true) == true) {
                sendToApp("save", "1");
                berp();
               } else {
                sendToApp("save", "0");
               }
               if (wasPlaying == true) {
                  playLoop();
               } 
            }
            break;
         case CMD_ACCESS_CODE:
            if (strcasecmp(cmd_val, BLANK) != 0) {
              memset(Config.access_code, 0, sizeof(Config.access_code));
              strlcpy(Config.access_code, cmd_val, sizeof(Config.access_code));
              saveConfig();
            }
            break;
         case CMD_DEBUG:
            Config.debug = (atoi(cmd_val) == 0) ? false : true;
            saveConfig();
            break;
         case CMD_ECHO:
            Config.echo = atoi(cmd_val) | 0;
            saveConfig();
            break;
         case CMD_PROFILE_DIR:
            if (strcasecmp(cmd_val, BLANK) != 0) {
              memset(Config.profile_dir, 0, sizeof(Config.profile_dir));
              strcpy(Config.profile_dir, cmd_val);
              saveConfig();
            }  
            break;
         case CMD_BAUD:
            Config.baud = atol(cmd_val);
            debug(F("Setting BLE Baud Rate to %d\n"),Config.baud);
            saveConfig();
            break;
         case CMD_DEFAULT:
            {
              if (strcasecmp(cmd_val, BLANK) == 0) {
                strcpy(cmd_val, Settings.file);
              }
              char ret[16];
              if (setDefaultProfile(cmd_val)) {
                strcpy(ret, "1;");
              } else {
                strcpy(ret, "0;");
              }
              strcat(ret, cmd_val);
              sendToApp("default", ret);
            }
            break;
         case CMD_DELETE:
            {
              if (strcasecmp(cmd_val, BLANK) != 0) {
                char ret[FILENAME_SIZE];
                if (deleteProfile(cmd_val)) {
                  strcpy(ret, "1;");
                  strcat(ret, cmd_val);
                } else {
                  strcpy(ret, "0;Could not remove profile");
                }
                sendToApp("delete", ret);
              }
            }
            break;
         case CMD_LOAD:
            {
              loopPlayer.stop();
              loadSettings(cmd_val, &Settings, false);
              applySettings();
              // send to remote if connected
              if (App.ble_connected) {
                sendConfig();
              }
              long l = playSound(Settings.sounds.start);
              delay(l+100);
              playLoop();
            }
            break;
         case CMD_PLAY:
            debug(F("Playing %s"), cmd_val);
            effectsPlayer.play(cmd_val);
            break; 
         case CMD_PLAY_EFFECT:
            playCommEffect(cmd_val);
            break;
         case CMD_PLAY_SOUND:
            playSound(cmd_val);
            break;
         case CMD_PLAY_GLOVE:
            playGloveSound(cmd_val);
            break;
         case CMD_PLAY_LOOP:
            if (strcasecmp(cmd_val, BLANK) != 0) {
              memset(Settings.loop.file, 0, sizeof(Settings.loop.file));
              strcpy(Settings.loop.file, cmd_val);
              playLoop();
            }
            break;
         case CMD_STOP_LOOP:
            loopPlayer.stop();
            App.loopLength = 0;
            break;
         case CMD_BEEP:
            {
              int i = atoi(cmd_val);
              if (i < 1) {
                i = 1;
              }
              beep(i);
            }
            break;
         case CMD_BERP:
            berp();
            break;
         case CMD_MUTE:
            audioShield.muteHeadphone();
            audioShield.muteLineout();
            App.muted = true;
            break;
         case CMD_UNMUTE:
            audioShield.unmuteHeadphone();
            audioShield.unmuteLineout();
            App.muted = false;
            break;
         case CMD_BACKUP:
            if (strcasecmp(cmd_val, BLANK) == 0) {
              strcpy(cmd_val, Settings.file);
            }
            addBackupExt(cmd_val);
            break;
         case CMD_RESTORE:
            {
               loopPlayer.stop();
               addBackupExt(cmd_val);
               loadSettings(cmd_val, &Settings, false);    
               applySettings();
               long l = playSound(Settings.sounds.start);
               delay(l+100);
               playLoop();
            }
            break;
         case CMD_SETTINGS:
            {
              Serial.println(F(BLANK));
              Serial.println(Settings.file);
              Serial.println(F("--------------------------------------------------------------------------------"));
              char buffer[JSON_BUFFER_SIZE];
              char *p = settingsToString(buffer, true);
              Serial.println(p);
              Serial.println(F("--------------------------------------------------------------------------------"));
              Serial.println(F(BLANK));
            }
            break;
         case CMD_SHOW:
            showFile(cmd_val);
            break;
         case CMD_SOUNDS:   
         case CMD_EFFECTS:
         case CMD_LOOPS:
         case CMD_GLOVE:
         case CMD_PROFILES:
         case CMD_FILES:
            {
              char path[MAX_FILENAME];
              char ext[5];
              boolean recurse = false;
              boolean echo = false;
              strcpy(ext, SOUND_EXT);
              switch (cmdIdx) {
                case CMD_SOUNDS:
                  strcpy(path, Settings.sounds.dir);
                  break;
                case CMD_EFFECTS:
                  strcpy(path, Settings.effects.dir);
                  break;
                case CMD_LOOPS:
                  strcpy(path, Settings.loop.dir);
                  break;
                case CMD_GLOVE:
                  strcpy(path, Settings.glove.dir);
                  break;
                case CMD_PROFILES:
                  strcpy(path, Config.profile_dir);
                  strcpy(ext, FILE_EXT);
                  break;
                case CMD_FILES:
                  strcpy(path, FWD_SLASH);
                  strcpy(ext, BLANK);
                  recurse = true;
                  echo = true;
                  break;
              }
              char temp[MAX_FILE_COUNT][FILENAME_SIZE];
              debug(F("Listing files in %s\n"), path);
              int count = listFiles(path, temp, MAX_FILE_COUNT, ext, recurse, echo);
              debug(F("Found %d files in %s\n"), count, path);
              if (strcasecmp(cmd_val, "1") == 0) {
                debug(F("Sending file list to app\n"));
                char buffer[1024];
                char *files = arrayToStringJson(buffer, temp, count);
                sendToApp(cmd_key, files);
              }
              if (Config.debug && echo == false) {
                for (int i = 0; i < count; i++) {
                  debug(F("%s\n"), temp[i]);
                }
              }
            }
            break;
         case CMD_LS:
            {
              char paths[MAX_FILE_COUNT][FILENAME_SIZE];
              char buffer[1025];
              // return a list of directories on the card
              int count = listDirectories(FWD_SLASH, paths);
              char *dirs = arrayToStringJson(buffer, paths, count);
              sendToApp(cmd_key, dirs);
            }
            break;
         case CMD_HELP:
            showFile("HELP.TXT");
            debug(F("After show help file"));
            break;
         case CMD_CALIBRATE:
            calibrate();
            break;
         case CMD_RESET:
            //softreset();
            break;
         case CMD_SLEEP:
            //gotoSleep();
            break;
         case CMD_MEM:
            showMemory();
            break;
         default:
            debug(F("Default -> %s = %s\n"), cmd_key, cmd_val);
            parseSetting(cmd_key, cmd_val);
            if (strcasecmp(cmd_key, "loop") == 0) {
              playLoop();
            }
            break; 
      } // end case

      // clear command buffers
      memset(cmd_key, 0, sizeof(cmd_key));
      memset(cmd_val, 0, sizeof(cmd_val));

  }


  // Check sound glove buttons
  for (byte i = 0; i < 6; i++) {

    // The PTT button is a special case, so it is processed separately
    if (!Settings.glove.ControlButtons[i].isPTT()) {

      byte whichButton = Settings.glove.ControlButtons[i].check();
      
        byte btype = Settings.glove.ControlButtons[i].buttons[whichButton-1].getType();

        if (whichButton != 1 && whichButton != 2) {
          continue;
        }

        sendButtonPress(i, whichButton);
      
        switch(btype) {
          // Sound button
          case BUTTON_SOUND:
            {
              if (effectsPlayer.isPlaying() && lastButton == whichButton && lastControlButton == i) {
                debug(F("Stop current sound\n"));
                effectsPlayer.stop();
              } else {
                char buffer[FILENAME_SIZE];
                char *sound = Settings.glove.ControlButtons[i].buttons[whichButton-1].getSound(buffer);
                debug(F("Play glove sound: %s\n"), sound);
                if (Settings.loop.mute == true) {
                  App.muteLoopTimeout = 0;
                  App.muteLoopMillis = 0;
                  loopOff();
                }
                long l = playGloveSound(sound);
                lastButton = whichButton;
                lastControlButton = i;
                if (Settings.loop.mute == true) {
                  //delay(l+100);
                  App.muteLoopMillis = 0;
                  App.muteLoopTimeout = l + 100;
                  //loopOn();
                }
              }
            }  
            break;
          // Volume Up
          case BUTTON_VOL_UP:  
            {
              Settings.volume.master = Settings.volume.master + .01;
              if (Settings.volume.master > MAX_GAIN) {
                Settings.volume.master = MAX_GAIN;
                berp();
              } else {
                debug(F("VOLUME UP: %2f\n"), Settings.volume.master);
                audioShield.volume(Settings.volume.master);
                sendToApp("volume", Settings.volume.master, 3);
                boopUp();
              }
            }  
            break;
          // Volume down  
          case BUTTON_VOL_DOWN:
            {
              Settings.volume.master = Settings.volume.master - .01;
              if (Settings.volume.master < 0) {
                Settings.volume.master = 0;
                berp();
              } else {
                debug(F("VOLUME DOWN: %2f\n"), Settings.volume.master);
                audioShield.volume(Settings.volume.master);
                sendToApp("volume", Settings.volume.master, 3);
                boopDown();
              }
            }  
            break;
          // mute  
          case BUTTON_MUTE:
            {
              if (App.muted) {
                audioShield.unmuteHeadphone();
                audioShield.unmuteLineout();
                App.muted = false;
              } else {
                audioShield.muteHeadphone();
                audioShield.muteLineout();
                App.muted = true;  
              }
            }
            sendToApp("mute", App.muted ? 1 : 0);
            break;
          // sleep/wake  
          case BUTTON_SLEEP:
            {
              gotoSleep();
            }  
            break;
          // lineout up  
          case BUTTON_LINE_UP:
            {
              --Settings.volume.lineout;
              if (Settings.volume.lineout < 13) {
                Settings.volume.lineout = 13;
                berp();
              } else {
                debug(F("LINEOUT UP: %d\n"), Settings.volume.lineout);
                audioShield.lineOutLevel(Settings.volume.lineout);   
                sendToApp("lineout", Settings.volume.lineout);
                boopUp();
              }
            }
            break;
          // lineout down  
          case BUTTON_LINE_DOWN:
            {
              ++Settings.volume.lineout;
              if (Settings.volume.lineout > 31) {
                Settings.volume.lineout = 31;
                berp();
              } else {
                debug(F("LINEOUT DOWN: %d\n"), Settings.volume.lineout);
                audioShield.lineOutLevel(Settings.volume.lineout); 
                sendToApp("lineout", Settings.volume.lineout);
                boopDown();
              }
            }
            break;
          // mic gain up
          case BUTTON_MIC_UP:
            {
              ++Settings.volume.microphone;
              if (Settings.volume.microphone > 63) {
                Settings.volume.microphone = 63;  
                berp();
              } else {
                debug(F("MIC GAIN UP: %d\n"), Settings.volume.microphone);
                audioShield.micGain(Settings.volume.microphone);  
                sendToApp("mic", Settings.volume.microphone);
                boopUp();
              }
            }
            break;
          // mic gain down  
          case BUTTON_MIC_DOWN:
            {
              --Settings.volume.microphone;
              if (Settings.volume.microphone < 0) {
                Settings.volume.microphone = 0;  
                berp();
              } else {
                debug(F("MIC GAIN DOWN: %d\n"), Settings.volume.microphone);
                audioShield.micGain(Settings.volume.microphone);  
                sendToApp("mic", Settings.volume.microphone);
                boopDown();
              }
            }
            break;
          // Start/Stop Loop
          case BUTTON_LOOP:
            {
              if (loopPlayer.isPlaying()) {
                loopPlayer.stop();
              } else {
                playLoop();
              }
            }
            break; 
          // Loop Gain up
          case BUTTON_LOOP_UP:
            {
              Settings.loop.volume = Settings.loop.volume + .05;
              if (Settings.loop.volume > MAX_GAIN) {
                Settings.loop.volume = MAX_GAIN;
                berp();
              } else {
                debug(F("LOOP GAIN UP: %2f\n"), Settings.loop.volume);
                effectsMixer.gain(1, Settings.loop.volume);
                sendToApp("loop_gain", Settings.loop.volume, 3);
                boopUp();
              }
            }
            break;  
          // Loop gain down
          case BUTTON_LOOP_DOWN:
            {
              Settings.loop.volume = Settings.loop.volume - .10;
              if (Settings.loop.volume < 0) {
                Settings.loop.volume = 0;
                berp();
              } else {
                debug(F("LOOP GAIN DOWN: %2f\n"), Settings.loop.volume);
                effectsMixer.gain(1, Settings.loop.volume);
                sendToApp("loop_gain", Settings.loop.volume, 3);
                boopDown();
              }
            }
            break; 
          // Voice gain up
          case BUTTON_VOICE_UP:
            {
              Settings.voice.volume = Settings.voice.volume + .05;
              if (Settings.voice.volume > MAX_GAIN) {
                Settings.voice.volume = MAX_GAIN;
                berp();
              } else {
                debug(F("VOICE GAIN UP: %2f\n"), Settings.voice.volume);
                voiceMixer.gain(0, Settings.voice.volume);
                voiceMixer.gain(1, Settings.voice.volume);
                sendToApp("voice_gain", Settings.voice.volume, 3);
                boopUp();
              }
            }
            break;
          // Voice gain down
          case BUTTON_VOICE_DOWN:
            {
              Settings.voice.volume = Settings.voice.volume - .10;
              if (Settings.voice.volume < 0) {
                Settings.voice.volume = 0;
                berp();
              } else {
                debug(F("VOICE GAIN DOWN: %2f\n"), Settings.voice.volume);
                voiceMixer.gain(0, Settings.voice.volume);
                voiceMixer.gain(1, Settings.voice.volume);  
                boopDown();
                sendToApp("voice_gain", Settings.voice.volume, 3);
              }
            }
            break; 
          // Dry Voice gain up
          case BUTTON_DRY_UP:
            {
              Settings.voice.dry = Settings.voice.dry + .05;
              if (Settings.voice.dry > MAX_GAIN) {
                Settings.voice.dry = MAX_GAIN;
                berp();
              } else {
                debug(F("DRY GAIN UP: %2f\n"), Settings.voice.dry);
                voiceMixer.gain(2, Settings.voice.dry);  
                sendToApp("dry_gain", Settings.voice.dry, 3);
                boopUp();
              }
            }
            break;
          // Dry Voice gain down
          case BUTTON_DRY_DOWN:
            {
              Settings.voice.dry = Settings.voice.dry - .10;
              if (Settings.voice.dry < 0) {
                Settings.voice.dry = 0;
                berp();
              } else {
                debug(F("DRY GAIN DOWN: %2f\n"), Settings.voice.dry);
                voiceMixer.gain(2, Settings.voice.dry);  
                boopDown();
                sendToApp("dry_gain", Settings.voice.dry, 3);
              }
            }
            break;   
          // Effects gain up
          case BUTTON_EFFECTS_UP:
            {
              Settings.effects.volume = Settings.effects.volume + .05;
              if (Settings.effects.volume > MAX_GAIN) {
                Settings.effects.volume = MAX_GAIN;
                berp();
              } else {
                debug(F("EFFECTS GAIN UP: %2f\n"), Settings.effects.volume);
                effectsMixer.gain(0, Settings.effects.volume);
                effectsMixer.gain(1, Settings.effects.volume);  
                boopUp();
                sendToApp("effects_gain", Settings.effects.volume, 3);
              }
            }
            break;
          // Effects gain down
          case BUTTON_EFFECTS_DOWN:
            {
              Settings.effects.volume = Settings.effects.volume - .10;
              if (Settings.effects.volume < 0) {
                Settings.effects.volume = 0;
                berp();
              } else {
                debug(F("EFFECTS GAIN DOWN: %2f\n"), Settings.effects.volume);
                effectsMixer.gain(0, Settings.effects.volume);
                effectsMixer.gain(1, Settings.effects.volume);  
                boopDown();
                sendToApp("effects_gain", Settings.effects.volume, 3);  
              }  
            }
            break; 
        }
      }
       
  }  
  
  if (App.ptt_button != BUTTON_UNSET && App.button_initialized == false) {
    App.button_initialized = checkPTTButton();
    if (App.button_initialized) {
      // turn voice on with background noise
      voiceOn();
    }
  } else if (App.ptt_button != BUTTON_UNSET) {
    Settings.glove.ControlButtons[App.ptt_button].update();
  }
  
  if (App.ptt_button != BUTTON_UNSET && App.button_initialized) {

    float val = 0;
    
    // Check if there is silence.  If not, set a flag so that
    // we don't switch back to Voice Activated mode accidentally ;)
    if (App.speaking == true && App.silent == true) {
        if (rms1.available()) {
          val = rms1.read();
          // This check is here to make sure we don't get false readings
          // when the button click noise is played which would prevent 
          // the switch back to Voice Activated mode
          if ((val-Settings.voice.stop) >= .1) {
            App.silent = false;
          }
        }
    }

    // Play notification sound if there has been silence for 2 or 5 seconds:
    //    2 seconds: switching back to VA mode
    //    5 seconds: go into sleep mode
    if (App.speaking == true && App.silent == true && (App.stopped == 2000 || (App.stopped == 5000 && App.ptt_button == App.wake_button))) {
        if (App.stopped == 2000) {
          beep(2);
        } else {
          beep(3);
        }
    }

    // Button press

    if (Settings.glove.ControlButtons[App.ptt_button].fell()) {
      sendButtonPress(App.ptt_button, 1);
      playCommEffect(Settings.sounds.button);
      //ms = 0;
      voiceOn();
    }
    
    // BUTTON RELEASE
    // Switch back to Voice Activated mode if:
    //    1.  PTT button was pushed
    //    2.  There has been silence for at least 2 seconds
    // NOTE:  If you start talking before the 2 second time limit
    //        it will NOT switch back...or if you talk and pause for 
    //        2 seconds or more it will NOT switch back.
    if (Settings.glove.ControlButtons[App.ptt_button].rose()) {
      if (App.silent == true && App.stopped >= 5000 && App.ptt_button == App.wake_button) {
        gotoSleep();
        return;
      } else if (App.silent == true && App.stopped >= 2000) {
        voiceOff();
        App.button_initialized = false;
        return;
      } else {
        App.stopped = 0;
        //while (stopped < Settings.voice.wait) {}
        voiceOff();
        // Play comm sound
        playCommEffect(Settings.sounds.buttonOff);
      }
    }
    
  } else {

      // Check if we have audio
      if (rms1.available()) {
        
        // get the input amplitude...will be between 0.0 (no sound) and 1.0
        float val = rms1.read();
  
        // Uncomment this to see what your constant signal is
        //Serial.println(val);
        
        // Set a minimum and maximum level to use or you will trigger it accidentally 
        // by breathing, etc.  Adjust this level as necessary!
        if (val >= Settings.voice.start) {

           debug(F("Voice start: %4f\n"), val);
           
          // If user is not currently speaking, then they just started talking :)
          if (App.speaking == false) {

            playCommEffect(Settings.sounds.voiceOn);
            voiceOn();
        
          }

        } else if (App.speaking == true) {

            //debug(F("Voice val: %4f\n"), val);
            
            if (val < Settings.voice.stop) {
  
              // If the user has stopped talking for at least the required "silence" time and 
              // the mic/line input has fallen below the minimum input threshold, play a random 
              // sound from the card.  NOTE:  You can adjust the delay time to your liking...
              // but it should probably be AT LEAST 1/4 second (250 milliseconds.)
  
              if (App.stopped >= Settings.voice.wait) {
                //debug(F("Voice stop: %4f\n"), val);
                voiceOff();
                // play comm effect
                playCommEffect(Settings.sounds.voiceOff);
              }
  
            } else {
                
                // Reset the "silence" counter 
                App.stopped = 0;
                
            }
  
          }
        
       }

    }
    
    readVolume();

  // Sleep mode check
  if (Settings.sleep.timer > 0 && (App.autoSleepMillis >= (Settings.sleep.timer * 60000))) {
      //gotoSleep();
  }

}
