#include <Arduino.h>

#include "controller.h"
#include "controller_wifi_action.h"

#include "LedIndicatorAdapter.h"


// LED_WIFI is defined in platformio.ini (different for each target)
//#define LED_WIFI    2    // LED_BUILTIN on ESP8266     

#ifndef LED_WIFI_INVERT 
#define LED_WIFI_INVERT false
#endif


LedIndicatorAdapter myWifiIndicatorAdapter(LED_WIFI, LED_WIFI_INVERT);
Indicator myWifiIndicator(&myWifiIndicatorAdapter);



extern ControllerFacade myController;



void MyWifiAction::idle(void) 
{
  myWifiIndicator.clear();
  myController.disconnectMqtt(); 
}

void MyWifiAction::connecting(void) 
{
  myWifiIndicator.blink();
}

void MyWifiAction::connected(void) 
{
  myWifiIndicator.set(); 
  myController.connectMqtt();
}

void MyWifiAction::disconnected(void) 
{
  myWifiIndicator.clear();
  myController.disconnectMqtt();  
}

void MyWifiAction::error(void) 
{
  myWifiIndicator.blink();
  myController.disconnectMqtt(); 
}  


