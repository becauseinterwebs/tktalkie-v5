//#include "Arduino.h"
#include "Bounce2.h"
#include "ControlButton.h"

ControlButton::ControlButton()
{
  setup();
  this->_button = Bounce2::Button();
}

ControlButton::ControlButton(byte buttonPin)
{
  this->setup();
  this->_button = Bounce2::Button();
  this->setPin(buttonPin);
}

ControlButton::ControlButton(byte buttonPin, byte interval)
{
  this->setup();
  this->_interval = interval;
  this->_button = Bounce2::Button();
  this->setPin(buttonPin);
}

byte ControlButton::check()
{
  this->_value = 0;
  this->_button.update();
  if (this->_button.fell()) {
    boolean released = false;
    elapsedMillis t = 0;
    while (t < 350 && released == false) {
      Serial.println(t);
      this->_button.update();
      if (this->_button.read() != 0) {
        released = true;
        this->_value = 1;
      }
    }
    if (released == false) {
      this->_value = 2;
    }
    Serial.println("");
  }
  return this->_value;
}

void ControlButton::setup()
{
  this->_value = 0;
}

void ControlButton::setup(byte buttonPin)
{
  this->setup();
  this->setPin(buttonPin);
}

void ControlButton::setPin(byte buttonPin)
{
  this->_pin = buttonPin;
  if (buttonPin > 1) {
    pinMode(this->_pin, INPUT_PULLUP);
    this->_button.attach(this->_pin);
    this->_button.interval(this->_interval);
    this->_button.setPressedState(LOW);
  }  
}

byte ControlButton::getPin() {
  return this->_pin;
}

// Most of these are just pass-thrus so that
// we are not accessing the button directly
boolean ControlButton::pressed()
{
  return this->_button.pressed();
}

boolean ControlButton::fell()
{
  return this->_button.fell();
}

boolean ControlButton::rose()
{
  return this->_button.rose();
}

void ControlButton::update() {
  this->_button.update();
}

void ControlButton::setPTT(boolean val) {
  this->_isPTT = val;
}

boolean ControlButton::isPTT() {
  return this->_isPTT;
}

void ControlButton::setInterval(byte intv) {
  this->_interval = intv;
  this->_button.interval(intv);
}

byte ControlButton::getInterval() {
  return this->_interval;
}

byte ControlButton::read() {
  return this->_button.read();
}

void ControlButton::reset() {
  this->_pin = 0;
  this->_interval = 15;
  this->_value = 0;
  this->_type = 0;
  this->_isPTT = false;
}
