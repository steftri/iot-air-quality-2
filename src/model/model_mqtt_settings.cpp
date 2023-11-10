#include <memory.h>

#include "model_mqtt_settings.h"


MqttSettings::MqttSettings(void)
{
  init();
}


void MqttSettings::init(void)
{
  mu16_BrokerPort = DEFAULT_BROKER_PORT;
  mac_BrokerAddr[0] = '\0';
}



void MqttSettings::setBroker(const char *pc_Addr, const uint16_t u16_Port)
{
  strncpy(mac_BrokerAddr, pc_Addr, MAX_BROKER_ADDR_LENGTH);
  mac_BrokerAddr[MAX_BROKER_ADDR_LENGTH] = '\0';
  mu16_BrokerPort = u16_Port;
}



const char *MqttSettings::getBrokerAddr(void)
{
  return mac_BrokerAddr;
}



uint16_t MqttSettings::getBrokerPort(void)
{
  return mu16_BrokerPort;
}


uint16_t MqttSettings::serialize(uint8_t *pu8_Buffer, const uint16_t u16_BufferSize)
{
  uint16_t u16_BufferPos = 0;

  if((!pu8_Buffer) || (u16_BufferSize<MQTT_SETTINGS_SIZE))
    return 0;

  for(auto i=0; i<MAX_BROKER_ADDR_LENGTH; i++)
      pu8_Buffer[u16_BufferPos++] = mac_BrokerAddr[i];
  pu8_Buffer[u16_BufferPos++] = static_cast<uint8_t>(mu16_BrokerPort>>8);
  pu8_Buffer[u16_BufferPos++] = static_cast<uint8_t>(mu16_BrokerPort); 

  return u16_BufferPos;
}


void MqttSettings::unserialize(const uint8_t *pu8_Buffer, const uint16_t u16_Size)
{
  uint16_t u16_BufferPos = 0;

  if((!pu8_Buffer) || (u16_Size<MQTT_SETTINGS_SIZE))
    return;

  for(auto i=0; i<MAX_BROKER_ADDR_LENGTH; i++)
    mac_BrokerAddr[i] = pu8_Buffer[u16_BufferPos++];
  mac_BrokerAddr[MAX_BROKER_ADDR_LENGTH]='\0';   

  mu16_BrokerPort  = static_cast<uint16_t>(pu8_Buffer[u16_BufferPos++])<<8;
  mu16_BrokerPort |= static_cast<uint16_t>(pu8_Buffer[u16_BufferPos++]);
}
