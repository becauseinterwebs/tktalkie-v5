/****
 * Global variables/constants
 */

// version flag
#define VERSION     5.00
#define MIN_APP_VER 2.00

/***************************
   BUTTON TYPE VALUES
   NOTE: Only digital pins (like 2) work for Sleep/Wake
*/
#define BUTTON_PTT              1   // PTT (SLEEP/WAKE unless otherwise configured)
#define BUTTON_SOUND            2   // Play/Stop Specified Sound
#define BUTTON_VOL_UP           3   // Spkr Volume Up
#define BUTTON_VOL_DOWN         4   // Spkr Volume Down
#define BUTTON_MUTE             5   // Mute Everything
#define BUTTON_SLEEP            6   // Sleep/Wake (Overrides PTT)
#define BUTTON_LINE_UP          7   // LineOut Volume Up
#define BUTTON_LINE_DOWN        8   // LineOut Volume Down
#define BUTTON_MIC_UP           9   // MIC_GAIN up
#define BUTTON_MIC_DOWN         10  // MIC_GAIN down
#define BUTTON_LOOP             11  // Start/Stop Loop
#define BUTTON_LOOP_UP          12  // Loop gain up
#define BUTTON_LOOP_DOWN        13  // Loop gain down
#define BUTTON_VOICE_UP         14  // Voice gain up
#define BUTTON_VOICE_DOWN       15  // Voice gain down
#define BUTTON_DRY_UP           16  // Dry gain up
#define BUTTON_DRY_DOWN         17  // Dry gain down
#define BUTTON_EFFECTS_UP       18  // Effects gain up
#define BUTTON_EFFECTS_DOWN     19  // Effects gain down

/***********************
 * Command Mapping
 */
#define CMD_DEBUG       0
#define CMD_ECHO        1
#define CMD_DEFAULT     2
#define CMD_DELETE      3    
#define CMD_LOAD        4
#define CMD_PLAY        5
#define CMD_PLAY_EFFECT 6
#define CMD_PLAY_SOUND  7
#define CMD_PLAY_GLOVE  8
#define CMD_PLAY_LOOP   9
#define CMD_STOP_LOOP   10
#define CMD_CONFIG      11
#define CMD_MUTE        12
#define CMD_UNMUTE      13
#define CMD_SAVE        14
#define CMD_ACCESS_CODE 15
#define CMD_CONNECT     16
#define CMD_DISCONNECT  17
#define CMD_DOWNLOAD    18
#define CMD_BACKUP      19
#define CMD_RESTORE     20
#define CMD_SETTINGS    21
#define CMD_FILES       22
#define CMD_SOUNDS      23
#define CMD_EFFECTS     24
#define CMD_LOOPS       25
#define CMD_GLOVE       26
#define CMD_PROFILES    27
#define CMD_LS          28
#define CMD_HELP        29
#define CMD_CALIBRATE   30
#define CMD_RESET       31
#define CMD_SLEEP       32
#define CMD_BAUD        33
#define CMD_MEM         34
#define CMD_BEEP        35
#define CMD_BERP        36
#define CMD_SHOW        37
#define CMD_PROFILE_DIR 38
#define CMD_NONE        255

/**********************
 * Settings commands
 */
#define SETTING_NAME            "name"
#define SETTING_VOLUME          "volume"
#define SETTING_LINEOUT         "lineout"
#define SETTING_LINEIN          "linein"
#define SETTING_HIPASS          "highpass"
#define SETTING_MIC             "mic"
#define SETTING_BUTTON_ON       "button_click"
#define SETTING_BUTTON_OFF      "button_release"
#define SETTING_VOICE_OFF       "voice_off"
#define SETTING_VOICE_ON        "voice_on"
#define SETTING_STARTUP_SOUND   "startup_sound"
#define SETTING_LOOP_FILE       "loop_file"
#define SETTING_NOISE_GAIN      "noise_gain"
#define SETTING_VOICE_GAIN      "voice_gain"
#define SETTING_DRY_GAIN        "dry_gain"
#define SETTING_EFFECTS_GAIN    "effects_gain"
#define SETTING_LOOP_GAIN       "loop_gain"
#define SETTING_SILENCE_TIME    "silence_time"
#define SETTING_VOICE_START     "voice_start"
#define SETTING_VOICE_STOP      "voice_stop"
#define SETTING_EQ              "eq"
#define SETTING_EQ_BANDS        "eq_bands"
#define SETTING_BITCRUSHER      "bitcrusher"
#define SETTING_EFFECTS_DIR     "effects_dir"
#define SETTING_SOUNDS_DIR      "sounds_dir"
#define SETTING_LOOP_DIR        "loop_dir"
#define SETTING_GLOVE_DIR       "glove_dir"
#define SETTING_MUTE_LOOP       "mute_loop"
#define SETTING_MUTE_EFFECTS    "mute_effects"
#define SETTING_SLEEP_TIME      "sleep_time"
#define SETTING_SLEEP_SOUND     "sleep_sound"
#define SETTING_CHORUS          "chorus"
#define SETTING_SHIFTER         "shifter"
#define SETTING_CHORUS_DELAY    "chorus_delay"
#define SETTING_CHORUS_VOICES   "chorus_voices"
#define SETTING_FLANGER_DELAY   "flanger_delay"
#define SETTING_FLANGER_FREQ    "flanger_freq"
#define SETTING_FLANGER_DEPTH   "flanger_depth"
#define SETTING_FLANGER_OFFSET  "flanger_offset"
#define SETTING_FLANGER         "flanger"
#define SETTING_BUTTON          "button"
#define SETTING_BUTTONS         "buttons"

/**********************
 * BLE Connect Errors
 */
#define CONNECT_SUCCESS "0"
#define CONNECT_BAD_UID "1"
#define CONNECT_BAD_PWD "2"
#define CONNECT_IN_USE  "3"
#define CONNECT_BAD_VER "4"
 
#define MAX_GAIN     10
#define MAX_FILENAME 14

#define BUTTON_UNSET 255

#define GRANULAR_MEMORY_SIZE 2048  // enough for 290 ms at 44.1 kHz
int16_t granularMemory[GRANULAR_MEMORY_SIZE];

// Memory buffer for encoding/decoding JSON data
#define JSON_BUFFER_SIZE  6*JSON_ARRAY_SIZE(2) + JSON_ARRAY_SIZE(5) + JSON_ARRAY_SIZE(6) + 4*JSON_OBJECT_SIZE(2) + 3*JSON_OBJECT_SIZE(3) + 2*JSON_OBJECT_SIZE(4) + 2*JSON_OBJECT_SIZE(5) + 2*JSON_OBJECT_SIZE(9) + 1024

// Other defaults
#define SOUND_EXT   ".WAV"
#define FILE_EXT    ".TXT"
#define BACKUP_EXT  ".BAK"
#define BEEP_VOLUME 0.5

// loop and serial command handlers
#define MAX_DATA_SIZE 100
      
#define MAX_FILE_COUNT     99
#define MAX_SETTINGS_COUNT 40
#define SETTING_ENTRY_MAX  150

#define EFFECTS_PLAYER 1
#define LOOP_PLAYER    2
#define FX_DELAY       16
#define FILENAME_SIZE  14

#define DEFAULT_EFFECTS_DIR "/effects/"
#define DEFAULT_SOUNDS_DIR  "/sounds/"
#define DEFAULT_LOOPS_DIR   "/loops/"
#define DEFAULT_GLOVE_DIR   "/glove/"
#define DEFAULT_PROFILE_DIR "/profiles/"

#define RANDOM        "*"
#define BLANK         ""
#define CMD_SEPARATOR "|"
#define FWD_SLASH     "/"

#define DEFAULT_SLEEP_SOUND   "SLEEP.WAV"
#define DEFAULT_STARTUP_SOUND "STARTUP.WAV"
#define DEFAULT_PROFILE       "DEFAULT.TXT"
#define CONFIG_FILE           "CONFIG.TXT"

/**
 * OPERATIONAL STATES - Used for tracking at what stage the app is currently running
 */
#define STATE_NONE      0
#define STATE_BOOTING   1
#define STATE_RUNNING   2
#define STATE_SLEEPING  3

struct Shifter_t {
  int length   = 0;
  int speed    = 512;
  float range  = 5.0;
  byte enabled = 1;
  void reset() {
    length = 0;
    speed = 512;
    range = 5.0;
    enabled = 0;
  }
};

struct Loop_t {
  char    dir[MAX_FILENAME]   = DEFAULT_LOOPS_DIR;
  char    file[MAX_FILENAME]  = BLANK;
  boolean mute      = true;
  float   volume    = 1;  
  void reset() {
    strcpy(dir, DEFAULT_LOOPS_DIR);
    strcpy(file,BLANK);
    mute = true;
    volume = 1;
  }
};

struct Voice_t {
  float         volume = 1.0000;
  float         dry    = 0.5000;
  float         start  = 0.0300;
  float         stop   = 0.0200;
  unsigned int  wait   = 275;
  void reset() {
    volume = 1.0000;
    dry    = 0.5000;
    start  = 0.0300;
    stop   = 0.0200;
    wait   = 275;
  }
};

struct Sounds_t {
  char dir[MAX_FILENAME]       = DEFAULT_SOUNDS_DIR;
  char start[MAX_FILENAME]     = DEFAULT_STARTUP_SOUND;
  char button[MAX_FILENAME]    = RANDOM;
  char buttonOff[MAX_FILENAME] = RANDOM;
  char voiceOn[MAX_FILENAME]   = BLANK;
  char voiceOff[MAX_FILENAME]  = RANDOM;
  void reset() {
    strcpy(dir, DEFAULT_SOUNDS_DIR);
    strcpy(start, BLANK);
    strcpy(button, RANDOM);
    strcpy(buttonOff, RANDOM);
    strcpy(voiceOff, RANDOM);
    strcpy(voiceOn, BLANK);
  }
};

struct Flanger_t {
  byte  delay  = FX_DELAY;
  byte  offset = 1;
  byte  depth  = 0;
  float freq   = 0.0625;
  short buffer[FX_DELAY*AUDIO_BLOCK_SAMPLES];
  byte enabled = 1;
  void reset() {
    delay = FX_DELAY;
    offset = 1;
    depth = 0;
    freq = 0.625;
    enabled = false;
  }
};

struct Chorus_t {
  byte voices  = 1;
  byte delay   = FX_DELAY;
  short buffer[FX_DELAY*AUDIO_BLOCK_SAMPLES];
  byte enabled = 1;
  void reset() {
    enabled = 1;
    voices = 1;
    delay = FX_DELAY;
  }
};

struct Bitcrusher_t {
  byte bits    = 16;
  int  rate    = 44100;
  void reset() {
    bits = 16;
    rate = 44100;
  }
};


struct Effects_t {
  char          dir[MAX_FILENAME]   = DEFAULT_EFFECTS_DIR;
  float         volume    = 1.0000;
  byte          highpass  = 1;
  float         noise     = 0.0140;
  boolean       mute      = true;   
  Bitcrusher_t  bitcrusher;
  Chorus_t      chorus;
  Flanger_t     flanger;
  Shifter_t     shifter;
  char          files[MAX_FILE_COUNT][MAX_FILENAME];
  byte          count;
  void reset() {
    strcpy(dir, DEFAULT_EFFECTS_DIR);
    bitcrusher.reset();
    chorus.reset();
    flanger.reset();
    shifter.reset();
  }
};

struct Eq_t {
  boolean active   = true;
  float   bands[5] = { -1.0,0,1,0,-1.0 };
  void reset() {
    active = false;
    for (byte i = 0; i < 5; i++) {
      bands[i] = 0;
    }
  }
};

struct Sleep_t {
  unsigned int  timer     = 0;
  char          file[MAX_FILENAME]  = DEFAULT_SLEEP_SOUND;  
  void reset() {
    timer = 0;
    strcpy(file, DEFAULT_SLEEP_SOUND);
  }
};

struct Glove_t {
  char dir[MAX_FILENAME] = DEFAULT_GLOVE_DIR;
  char settings[6][30] = { "0","0","0","0","0","0" };
  char files[MAX_FILE_COUNT][MAX_FILENAME];
  byte count;
  // This could be turned into a management class, but there is not a lot of stuff to do with it...so....
  ControlButton ControlButtons[6] = { ControlButton(), ControlButton(), ControlButton(), ControlButton(), ControlButton(), ControlButton() }; 
  void reset() {
    strcpy(dir, DEFAULT_GLOVE_DIR);
    for (byte i = 0; i < 6; i++) {
      strcpy(settings[i], "0");
      ControlButtons[i].reset();
    }
  }
};

struct Volume_t {
  float master     = 0.5;
  byte  lineout    = 29;  // Valid values 13 to 31. Default teensy setting is 29.
  byte  linein     = 5;   // Value values 0 to 15. Default teensy setting is 5;
  byte  microphone = 3;
  void reset() {
    master = 0;
    lineout = 0;
    linein = 0;
    microphone = 0;
  }
};

struct Settings_t {
  char      name[25] = "Default Profile";
  char      file[MAX_FILENAME] = DEFAULT_PROFILE;
  Volume_t  volume;
  Loop_t    loop;
  Voice_t   voice;
  Sounds_t  sounds;
  Effects_t effects;
  Eq_t      eq;
  Sleep_t   sleep;
  Glove_t   glove;
  void reset() {
    memset(name, 0, sizeof(name));
    memset(file, 0, sizeof(file));
    volume.reset();
    loop.reset();
    voice.reset();
    sounds.reset();
    effects.reset();
    eq.reset();
    sleep.reset();
    glove.reset();
  }
} Settings;

struct Config_t {
  char profile[MAX_FILENAME] = DEFAULT_PROFILE;
  char profile_dir[MAX_FILENAME] = DEFAULT_PROFILE_DIR;
  // These define the pins the (up to) 6 control buttons can be connected to
// NOTE:  Only digital pins can be used for waking:
//        2,4,6,7,9,10,11,13,16,21,22,26,30,33
  byte buttons[6]       = {0,0,0,0,0,0};
  boolean debug         = true;          // Set to true to have debug messages printed out...useful for testing
  boolean echo          = false;          // Set to true to have BLE TX/RX messages displayed in serial monitor
  char input[5]         = "BOTH";  // Set to "MIC", "LINE" or "BOTH" (default)
  char output[5]        = "BOTH";  // Set to "SPKR", "LINE" or "BOTH" (default)
  char access_code[25]  = "1138";      // the password for remote apps to access this device (specified in global settings file)
  long baud              = 9600;
} Config;

struct App_t {
  byte    state               = STATE_NONE;   // tracks the operational state of the application
  boolean SD_OK               = true;
  boolean silent              = false;        // used for PTT and to switch back to Voice Activated mode
  boolean speaking            = false;        // flag to let us know if the user is speaking or not
  boolean button_initialized  = false;        // flag that lets us know if the PTT has been pushed or not to go into PTT mode
  boolean ble_connected       = false;        // flag to indicate whether a remote app is connected or not
  boolean muted               = false;        // flag to indicate whether all sounds should be muted
  byte    lastRnd             = -1;           // Keeps track of the last random sound effects file played so that it is different each time
  byte    lastGloveRnd        = -1;           // Keeps track of the last random sound file played via the sound glove so that it is different each time
  byte    wake_button         = BUTTON_UNSET;
  byte    ptt_button          = BUTTON_UNSET;
  char    device_id[50];
  unsigned int  loopLength      = 0;
  unsigned int  muteLoopTimeout = 0;
  elapsedMillis loopMillis      = 0;
  elapsedMillis autoSleepMillis = 0;
  elapsedMillis muteLoopMillis  = 0;
  elapsedMillis stopped         = 0;          // used to tell how long user has stopped talking
  void reset() {
    wake_button = BUTTON_UNSET;
    ptt_button = BUTTON_UNSET;
    muted = false;
    button_initialized = false;
  }
} App;

// GUItool: begin automatically generated code
AudioInputI2S            i2s1;           //xy=59,147
AudioEffectGranular      granular1;      //xy=175,91
AudioAnalyzeRMS          rms1;           //xy=248,201
AudioEffectBitcrusher    bitcrusher1;    //xy=311,100
AudioSynthNoisePink      pink1;          //xy=376,326
AudioPlaySdWav           loopPlayer;     //xy=383,257
AudioSynthWaveform       waveform1;      //xy=386,291
AudioPlaySdWav           effectsPlayer;  //xy=392,222
AudioEffectFlange        flange1;        //xy=452,101
AudioEffectChorus        chorus1;        //xy=583,100
AudioMixer4              effectsMixer;   //xy=594,264
AudioMixer4              voiceMixer;     //xy=748,129
AudioOutputI2S           i2s2;           //xy=913,129
AudioConnection          patchCord1(i2s1, 0, voiceMixer, 2);
AudioConnection          patchCord2(i2s1, 0, granular1, 0);
AudioConnection          patchCord3(i2s1, 1, rms1, 0);
AudioConnection          patchCord4(granular1, bitcrusher1);
AudioConnection          patchCord5(bitcrusher1, flange1);
AudioConnection          patchCord6(pink1, 0, effectsMixer, 3);
AudioConnection          patchCord7(loopPlayer, 0, effectsMixer, 1);
AudioConnection          patchCord8(waveform1, 0, effectsMixer, 2);
AudioConnection          patchCord9(effectsPlayer, 0, effectsMixer, 0);
AudioConnection          patchCord10(flange1, chorus1);
AudioConnection          patchCord11(chorus1, 0, voiceMixer, 0);
AudioConnection          patchCord12(effectsMixer, 0, voiceMixer, 3);
AudioConnection          patchCord13(voiceMixer, 0, i2s2, 0);
AudioConnection          patchCord14(voiceMixer, 0, i2s2, 1);
AudioControlSGTL5000     audioShield;    //xy=113,275
// GUItool: end automatically generated code

/*
// GUItool: begin automatically generated code
AudioInputI2S            i2s1;           //xy=55,329
AudioEffectGranular      granular1;      //xy=99,186
AudioEffectWaveshaper    waveshape1;     //xy=145,234
AudioEffectBitcrusher    bitcrusher1;    //xy=203,279
AudioAnalyzeRMS          rms1;           //xy=244,383
AudioEffectFlange        flange1;        //xy=302,230
AudioSynthNoisePink      pink1;          //xy=372,508
AudioPlaySdWav           loopPlayer;     //xy=379,439
AudioSynthWaveform       waveform1;      //xy=382,473
AudioPlaySdWav           effectsPlayer;  //xy=388,404
AudioEffectChorus        chorus1;        //xy=392,279
AudioEffectDelay         delay1;         //xy=503,189
AudioMixer4              effectsMixer;   //xy=590,446
AudioEffectReverb        reverb1;        //xy=606,281
AudioMixer4              voiceMixer;     //xy=744,311
AudioOutputI2S           i2s2;           //xy=909,311
AudioConnection          patchCord1(i2s1, 0, voiceMixer, 2);
AudioConnection          patchCord2(i2s1, 0, granular1, 0);
AudioConnection          patchCord3(i2s1, 1, rms1, 0);
AudioConnection          patchCord4(granular1, waveshape1);
AudioConnection          patchCord5(waveshape1, bitcrusher1);
AudioConnection          patchCord6(bitcrusher1, flange1);
AudioConnection          patchCord7(flange1, chorus1);
AudioConnection          patchCord8(pink1, 0, effectsMixer, 3);
AudioConnection          patchCord9(loopPlayer, 0, effectsMixer, 1);
AudioConnection          patchCord10(waveform1, 0, effectsMixer, 2);
AudioConnection          patchCord11(effectsPlayer, 0, effectsMixer, 0);
AudioConnection          patchCord12(chorus1, delay1);
AudioConnection          patchCord13(delay1, 0, reverb1, 0);
AudioConnection          patchCord14(effectsMixer, 0, voiceMixer, 3);
AudioConnection          patchCord15(reverb1, 0, voiceMixer, 0);
AudioConnection          patchCord16(voiceMixer, 0, i2s2, 0);
AudioConnection          patchCord17(voiceMixer, 0, i2s2, 1);
AudioControlSGTL5000     audioShield;    //xy=109,457
// GUItool: end automatically generated code

float WAVESHAPE1[17] = {
  -0.99,
  -0.916,
  -0.816,
  -0.701,
  -0.575,
  -0.439,
  -0.297,
  -0.15,
  0,
  0.15,
  0.297,
  0.439,
  0.575,
  0.701,
  0.816,
  0.916,
  0.99,
};
*/
