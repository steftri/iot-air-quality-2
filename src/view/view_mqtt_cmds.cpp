#include <Arduino.h>
#include <ArduinoJson.h>

#include "controller/controller.h"

#include "view_mqtt_cmds.h"

#include "debug.h"


extern ControllerFacade myController;

void ViewMqttCommands::cmdReset(void)
{
  debug.println(Debug::Info, "reset command received via MQTT");
  myController.reset();
}


void ViewMqttCommands::cmdLed(const uint8_t u8_Led, const bool b_On)
{
  debug.println(Debug::Info, "LED command received via MQTT");
  myController.setLed(u8_Led, b_On);  
}


void ViewMqttCommands::parse(const char *pc_Topic, const char *pc_Content)
{
  StaticJsonDocument<200> jsonDoc;
  DeserializationError error; 

  if((error = deserializeJson(jsonDoc, pc_Content)))
    return;

  const char *pc_Operation = jsonDoc["operation"];

  if(!strcasecmp(pc_Operation, "reset"))
  {
    cmdReset();
  }
  else if(!strcasecmp(pc_Operation, "setled"))
  {
    uint8_t u8_Led = jsonDoc["number"].as<uint8_t>();
    bool b_On = jsonDoc["value"].as<bool>();
    cmdLed(u8_Led, b_On);
  }
  else
    debug.println(Debug::Warning, "unknown MQTT command received");
}
