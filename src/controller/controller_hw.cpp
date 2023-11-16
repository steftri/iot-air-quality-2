#include <Arduino.h>

#include "controller_hw.h"  
  
  
#define LED_USER1   17  
#define PIN_ANALOG  32


void HwController::setup(void)
{
  pinMode(LED_USER1, OUTPUT);
  digitalWrite(LED_USER1, LOW);
}



void HwController::loop(void)
{
}



void HwController::reset(void)
{
  ESP.restart();
}



uint64_t HwController::getUid(void)
{
#ifdef ESP8266
  return ESP.getChipId();
#else
  return ESP.getEfuseMac();
#endif  
}



void HwController::setLed(const uint8_t u8_Led, bool b_On)
{
  if(u8_Led==0)
    digitalWrite(LED_USER1, b_On?HIGH:LOW);
}



float HwController::readAnalogValue(void)
{
  return static_cast<float>(analogRead(PIN_ANALOG))/4095.0*3.3;
}
