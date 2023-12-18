#ifndef _MODEL_MQTT_SETTINGS_H_
#define _MODEL_MQTT_SETTINGS_H_

#include <inttypes.h>

#define MAX_BROKER_ADDR_LENGTH   64
#define DEFAULT_BROKER_PORT    1883

#define MQTT_SETTINGS_SIZE        (MAX_BROKER_ADDR_LENGTH+2)

class MqttSettings
{
  char mac_BrokerAddr[MAX_BROKER_ADDR_LENGTH+1];
  uint16_t mu16_BrokerPort;

public:
  MqttSettings(void);

  void init(void);
  void setBroker(const char *pc_Addr, const uint16_t u16_Port = DEFAULT_BROKER_PORT);
  const char *getBrokerAddr(void);
  uint16_t getBrokerPort(void);

  uint16_t serialize(uint8_t *pu8_Buffer, const uint16_t u16_BufferSize);
  void unserialize(const uint8_t *pu8_Buffer, const uint16_t u16_Size);
};

#endif
