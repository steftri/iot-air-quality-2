#ifndef _CONTROLLER_MQTT_ACTION_H_
#define _CONTROLLER_MQTT_ACTION_H_

#include "controller/controller_mqtt.h"

class MyMqttAction : public MqttAction
{
  void idle(void) override;
  void connecting(void) override;
  void connected(void) override;
  void disconnected(void) override;
  void error(void) override;

public:
  void publishMessurementData(float f32_AnalogueValue);
};

#endif