#ifndef _VIEW_MQTT_H_
#define _VIEW_MQTT_H_

#include <inttypes.h>

class ViewMqttCommands
{
public:
  static void parse(const char *pc_Topic, const char *pc_Content); 

private:
  static void cmdReset(void);
  static void cmdLed(const uint8_t u8_Led, const bool b_On);
};


#endif