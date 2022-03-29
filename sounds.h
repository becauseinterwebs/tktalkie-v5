/****
 * Sound utils
 */

/**
 * Emit a warning tone
 */
void beep(const byte times = 1)
{
  debug(F("Play beep x %d at volume %d"), times, BEEP_VOLUME);
  if (App.muted) {
    audioShield.unmuteHeadphone();
    audioShield.unmuteLineout();
  }
  for (byte i=0; i<times; i++) {
    waveform1.frequency(720);
    waveform1.amplitude(BEEP_VOLUME);
    delay(150);
    waveform1.amplitude(0);
    delay(150);
  }
  if (App.muted) {
    audioShield.muteHeadphone();
    audioShield.muteLineout();
  }
}

void boop(int freq, const byte dir=1, const byte distance=75) {
   waveform1.frequency(freq);
   waveform1.amplitude(.2);
   for (byte i = 0; i < distance; i++) {
      if (dir == 1) {
        freq++;
      } else {
        freq--;
      }
      waveform1.frequency(freq);  
      delay(1);
   }
   waveform1.amplitude(0);
}

void boopUp() {
  boop(440);
}

void boopDown() {
  boop(480, 0, 100);
}

void berp() {
  for (byte i = 0; i < 2; i++) {
    waveform1.frequency(2440);
    waveform1.amplitude(BEEP_VOLUME);
    delay(50);
    waveform1.frequency(380);
    delay(75);
    waveform1.amplitude(0);
    delay(30);
  }  
}

// Turns the volume down on the chatter loop
void loopOff() 
{
  effectsMixer.gain(1, 0);
}

//Turns the volume up on the chatter loop
void loopOn() 
{
  // gradually raise level to avoid pops 
  if (Settings.loop.volume > 1) {
    for (byte i=0; i<=Settings.loop.volume; i++) {
      effectsMixer.gain(1, i);
    }
  }
  effectsMixer.gain(1, Settings.loop.volume);
}

// Read the contents of the SD card and put any files ending with ".WAV" 
// into the array.  It will recursively search directories.  
void loadSoundEffects() 
{
  if (strcasecmp(Settings.effects.dir, BLANK) == 0) {
    Settings.effects.count = 0;
    debug(F("No effects directory specified"));
    return;
  }
  AudioNoInterrupts();
  Settings.effects.count = listFiles(Settings.effects.dir, Settings.effects.files, MAX_FILE_COUNT, SOUND_EXT, false, false);
  AudioInterrupts();
  debug(F("%d Sound effects loaded\n"), Settings.effects.count);
}

// Read the contents of the SD card and put any files ending with ".WAV" 
// into the array.  It will recursively search directories.  
void loadGloveSounds() 
{
  if (strcasecmp(Settings.glove.dir, BLANK) == 0) {
    Settings.glove.count = 0;
    debug(F("No glove directory specified"));
    return;
  }
  AudioNoInterrupts();
  Settings.glove.count = listFiles(Settings.glove.dir, Settings.glove.files, MAX_FILE_COUNT, SOUND_EXT, false, false);
  AudioInterrupts();
  debug(F("%d Glove sounds loaded\n"), Settings.glove.count);
}

// Play the specified sound effect from the SD card
long playSoundFile(byte player, char *filename) 
{
  if (strcasecmp(filename, BLANK) == 0) {
    debug(F("Exit play sound -> blank file name\n"));
    return 0;
  }
  char *ret = strstr(filename, ".");
  if (ret == NULL) {
    char ext[5];
    strcpy(ext, SOUND_EXT);
    strcat(filename, ext);
  }
  debug(F("Play sound file %s on player %d\n"), filename, player);
  unsigned long len = 0;
  switch (player) {
    case LOOP_PLAYER:
      if (loopPlayer.isPlaying()) {
        loopPlayer.stop();
      }
      loopPlayer.play(filename);
      delay(10);
      len = loopPlayer.lengthMillis();
      break;
    default:
      effectsPlayer.play(filename);
      delay(10);
      len = effectsPlayer.lengthMillis();
      break;
  }
  debug(F("Sound File Length: %d\n"), len);
  return len;
}

// Shortcut to play a sound from the SOUNDS directory
unsigned long playGloveSound(const char *filename)
{
  if (strcasecmp(filename, BLANK) == 0) {
    return 0;
  }
  char buf[FILENAME_SIZE*2];
  strcpy(buf, Settings.glove.dir);
  // random sound from the glove directory
  if (strcasecmp(filename, RANDOM) == 0) {
    // generate a random number between 0 and the number of files read - 1
    byte rnd = 0;
    byte count = 0;
    rnd = App.lastGloveRnd;
    while (rnd == App.lastGloveRnd && count < 50) { 
     rnd = random(0, Settings.glove.count);
     count++;
    }
    App.lastGloveRnd = rnd;
    strcat(buf, Settings.glove.files[rnd]);
  } else {
    strcat(buf, filename);
  }
  return playSoundFile(EFFECTS_PLAYER, buf);
}

// Shortcut to play a sound from the SOUNDS directory
unsigned long playSound(const char *filename)
{
  if (strcasecmp(filename, BLANK) == 0) {
    return 0;
  }
  char buf[FILENAME_SIZE*2];
  strcpy(buf, Settings.sounds.dir);
  strcat(buf, filename);
  return playSoundFile(EFFECTS_PLAYER, buf);
}

// Shortcut to play a sound from the EFFECTS directory
unsigned long playEffect(const char *filename)
{
  if (strcasecmp(filename, BLANK) == 0) {
    return 0;
  }
  char buf[FILENAME_SIZE*2];
  strcpy(buf, Settings.effects.dir);
  strcat(buf, filename);
  debug(F("Play effect: %s\n"), buf);
  return playSoundFile(EFFECTS_PLAYER, buf);
}

// Play sound loop and set counters
void playLoop() 
{
  debug(F("Playing loop file: %s\n"), Settings.loop.file);
  App.loopLength = 0;
  if (strcasecmp(Settings.loop.file, BLANK) != 0 && strlen(Settings.loop.file) > 0) {
    char buf[FILENAME_SIZE*2];
    strcpy(buf, Settings.loop.dir);
    strcat(buf, Settings.loop.file);
    App.loopLength = playSoundFile(LOOP_PLAYER, buf);
  }
  App.loopMillis = 0;
}

// Play a random sound effect from the SD card
void addSoundEffect()
{
  if (App.speaking == true || Settings.effects.count < 1 || Settings.effects.mute == true || Settings.effects.mute == 1) return;
  // generate a random number between 0 and the number of files read - 1
  byte rnd = 0;
  byte count = 0;
  rnd = App.lastRnd;
  while (rnd == App.lastRnd && count < 50) { 
   rnd = random(0, Settings.effects.count);
   count++;
  }
  App.lastRnd = rnd;
  // play the file
  playEffect(Settings.effects.files[rnd]);
}


// Play a specified or random sound effect after talking
void playCommEffect(const char *sound)
{
  if (Settings.effects.mute) return;
  if (strcasecmp(sound, RANDOM) == 0) {
    addSoundEffect();
  } else { 
    playEffect(sound);
  }
}

// Check the optional volume pot for output level
float readVolume()
{
    float vol = 0;
    if (Settings.volume.master) {
      audioShield.volume(Settings.volume.master);
      vol = Settings.volume.master;
    } else {
      // comment these lines if your audio shield does not have the optional volume pot soldered on
      vol = analogRead(15);
      vol = vol / 1023;
      audioShield.volume(vol);
    }
    if (vol > 1.0) {
      vol = 1.0;
    } else if (vol < 0) {
      vol = 0;
    }
    return vol;
}

// Turns off the voice channels on the mixer
void voiceOff() 
{
  //digitalWrite(LED_BUILTIN, LOW);
  App.autoSleepMillis = 0;
  App.speaking = false;
  App.silent = false;
  App.stopped = 0;
  pink1.amplitude(0);
  voiceMixer.gain(0, 0);
  voiceMixer.gain(1, 0);
  voiceMixer.gain(2, 0);
  if (Settings.loop.mute == true) {
    loopOn();
  }
}

// Turns on the voice channels on the mixer
void voiceOn() 
{
  App.autoSleepMillis = 0;
  App.speaking = true;
  App.silent = true;
  if (Settings.loop.mute == true) {
    loopOff();
  }
  // Reset the "user is talking" timer
  App.stopped = 0;
  // pops are ok here ;)
  usb.print("VOICE: ");
  usb.println(Settings.voice.volume);
  pink1.amplitude(Settings.effects.noise);
  voiceMixer.gain(0, Settings.voice.volume);
  voiceMixer.gain(1, Settings.voice.volume);
  voiceMixer.gain(2, Settings.voice.dry);
  //digitalWrite(LED_BUILTIN, HIGH);
}
