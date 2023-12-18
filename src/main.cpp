/*
 *  IOT skeleton for arduino-based ESP8266/ESP32 devices
 *
 *  main.cpp
 *
 *  Created on: 2023-09-12
 *      Author: TRST
 */

#define PROJECT_NAME "iot-arduino"

#include <Arduino.h>
#include <SpinTimer.h>      // see https://github.com/dniklaus/spin-timer


#include "model/settings.h"
#include "controller/controller.h"
#include "view/view.h"



ViewFacade myView;
ControllerFacade myController(&myView);


void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println(PROJECT_NAME);
  Serial.println(__DATE__ ", " __TIME__);
  Serial.print("LED WIFI: ");
  Serial.println(LED_WIFI);
  Serial.print("LED MQTT: ");
  Serial.println(LED_MQTT);
  Serial.print("LED USER1: ");
  Serial.println(LED_USER1);
  Serial.print("PIN_ANALOG_IN: ");
  Serial.println(PIN_ANALOG_IN);

  myController.setup();
  myView.setup();
}




void loop()
{
  myView.loop();
  myController.loop();

  scheduleTimers();         // process Timers
}

