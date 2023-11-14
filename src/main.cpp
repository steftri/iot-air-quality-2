/*
 *  spacecraft using SECCT
 *
 *  main.cpp
 *
 *  Created on: 2023-09-12
 *      Author: TRST
 */

#define PROJECT_NAME "iot-arduino"

#include <Arduino.h>
#include <SpinTimer.h>      // see https://github.com/dniklaus/spin-timer

#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif


#include "model/settings.h"
#include "controller/controller.h"
#include "view/view.h"


#include "LedIndicatorAdapter.h"



ViewFacade myView;
ControllerFacade myController(&myView);


void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println(PROJECT_NAME);
  Serial.println(__DATE__ ", " __TIME__);
  Serial.print("UID: ");
  Serial.println(ESP.getEfuseMac());
  Serial.println();
  
  myController.setup();
  myView.setup();
}




void loop()
{
  myView.loop();
  myController.loop();

  scheduleTimers();         // process Timers
}

