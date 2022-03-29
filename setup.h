#include <Snooze.h>
//#include <SnoozeBlock.h>

SnoozeDigital snoozeDigital;
SnoozeUSBSerial usb;
SnoozeBlock config_teensy40(usb, snoozeDigital);
