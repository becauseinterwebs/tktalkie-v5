/****
 * General utils
 */

/**
 * Uses the "F" FlashStringHelper (to help save memory)
 * to output a formatted string.
 * This code is adapted from http://playground.arduino.cc/Main/Printf
 */
void debug(const __FlashStringHelper *fmt, ... ) {
  if (Config.debug == 0) {
    return;
  }
  char buf[1025]; // resulting string limited to 1M chars
  va_list args;
  va_start (args, fmt);
#ifdef __AVR__
  vsnprintf_P(buf, sizeof(buf), (const char *)fmt, args); // progmem for AVR
#else
  vsnprintf(buf, sizeof(buf), (const char *)fmt, args); // for the rest of the world
#endif
  va_end(args);
  Serial.print(F("[DEBUG] "));
  Serial.print(F(buf));
}

/**
 * Convert char string to upcase
 */
void upcase(char *str)
{
   int i = 0;
   if (strcasecmp(str, "") == 0) {
    return;
   }
   while (str[i] != '\0') {
    str[i] = toupper(str[i]);
    i++;
   }
}

/**
 * Convert array of char strings to comma-delimited char-based string 
 */
char *arrayToStringJson(char result[], const char arr[][14], int len) 
{
  strcpy(result, "[");
  for (int i = 0 ; i < len; i++) {
    strcat(result, "\"");
    strcat(result, arr[i]);
    strcat(result, "\"");
    if (i < len-1) {
      strcat(result, ",");
    }
  }
  strcat(result, "]");
  return result;
}

byte getCommand(const char cmd[14])
{
  
  char commands[39][12] = { 
    "debug", 
    "echo", 
    "default", 
    "delete", 
    "load", 
    "play", 
    "play_effect", 
    "play_sound", 
    "play_glove", 
    "play_loop", 
    "stop_loop", 
    "config", 
    "mute", 
    "unmute", 
    "save", 
    "access_code", 
    "connect", 
    "disconnect", 
    "download",
    "backup", 
    "restore", 
    "settings", 
    "files", 
    "sounds", 
    "effects",
    "loops",
    "glove",
    "profiles",
    "ls",
    "help",
    "calibrate",
    "reset",
    "sleep",
    "baud",
    "mem",
    "beep", 
    "berp",
    "show",
    "profile_dir"
  };

  byte min   = 0;
  byte max   = 37;
  byte r     = round((min+max)/2);
  byte l     = r-1;
  byte index = 255;

  while (1) {
    if (strcasecmp(cmd, commands[l]) == 0) { 
      index = l;
      break;
    } else if (strcasecmp(cmd, commands[r]) == 0) {
      index = r;
      break;
    } else {
      if (l == min && r == max && index == 255) {
        break;
      }
      if (l > min) {
        l--;
      }
      if (r < max) {
        r++;
      }
    }
  }

  debug(F("cmd: %s index: %d\n"), cmd, index);

  return (index);
  
}
