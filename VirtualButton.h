/*
  VirtualButton.h - Library for TK-Talkie Control Buttons.
  B. Williams Oct 30, 2017
*/

#ifndef VirtualButton_h
#define VirtualButton_h

#include "Arduino.h"

class VirtualButton
{
  public:
    VirtualButton();
    VirtualButton(byte buttonType);
    void setType(byte buttonType);
    byte getType();
    void setup(byte buttonType);
    void setSound(const char *sound);
    char *getSound(char sound[]);
    char *getSettings();
  private:
    byte _type;
    char _sound[14];
    void setup();
};

#endif
