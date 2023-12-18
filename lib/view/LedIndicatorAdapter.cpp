/*
 * LedIndicatorAdapter.cpp
 *
 *  Created on: 04.11.2019
 *      Author: nid
 */

#include "LedIndicatorAdapter.h"

#include <Arduino.h>

LedIndicatorAdapter::LedIndicatorAdapter(const uint16_t u16_LedPin, const bool b_Inverted)
{
  mu16_LedPin=u16_LedPin;
  mb_Inverted = b_Inverted;

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
  if(mb_Inverted)
    isOn = !isOn;
  digitalWrite(mu16_LedPin, isOn);
}
