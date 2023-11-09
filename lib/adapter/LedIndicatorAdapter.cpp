/*
 * LedIndicatorAdapter.cpp
 *
 *  Created on: 04.11.2019
 *      Author: nid
 */

#include "LedIndicatorAdapter.h"

#include <Arduino.h>

LedIndicatorAdapter::LedIndicatorAdapter(const uint16_t u16_LedPin)
{
  mu16_LedPin=u16_LedPin;

  // initialize built in LED pin as output
  pinMode(mu16_LedPin, OUTPUT);

  // switch LED off
  setLed(false);
}

LedIndicatorAdapter::~LedIndicatorAdapter()
{ }

void LedIndicatorAdapter::notifyStatusChange(bool status)
{
  setLed(status);
}

void LedIndicatorAdapter::setLed(bool isOn)
{
#ifdef ESP8266
  // the built-in LED logic on ESP8266 module is inverted!
  if(mu16_LedPin==LED_BUILTIN)
    isOn = !isOn;
#endif
  digitalWrite(mu16_LedPin, isOn);
}
