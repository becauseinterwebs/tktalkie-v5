#include "Arduino.h"
#include "VirtualButton.h"

VirtualButton::VirtualButton()
{
  setup();
}

VirtualButton::VirtualButton(byte buttonType)
{
  this->setup();
  this->setType(buttonType);
}

void VirtualButton::setup()
{
   //this->_value = 0;
}

void VirtualButton::setup(byte buttonType)
{
  this->setup();
  this->setType(buttonType);  
}

void VirtualButton::setType(byte buttonType)
{
  this->_type = buttonType;
}

byte VirtualButton::getType() 
{
  return this->_type;
}

void VirtualButton::setSound(const char *sound)
{
  strcpy(this->_sound, sound);
}

char *VirtualButton::getSound(char sound[])
{
  strcpy(sound, this->_sound);
  return sound;
}

char *VirtualButton::getSettings() 
{
  char * buf = (char *) malloc (16);
  //memset(settings, 0, sizeof(settings));
  sprintf(buf, "%d", this->_type);
  if (strcasecmp(this->_sound, "") != 0) {
    strcat(buf, ",");
    strcat(buf, this->_sound);
  }
  return buf;
}

