#include <Arduino.h>

#include "controller_hw.h"  
  

// LED_USER1 and PIN_ANALOG_IN is defined in platformio.ini (different for each target)
//#define LED_USER1   17  
//#define PIN_ANALOG_IN  32


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
#ifdef ESP8266
  return static_cast<float>(analogRead(PIN_ANALOG_IN))/1023.0*3.3;
#else
  return static_cast<float>(analogRead(PIN_ANALOG_IN))/4095.0*3.3;
#endif
}
