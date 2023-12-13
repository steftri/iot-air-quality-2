#ifndef _MODEL_SETTINGS_H_
#define _MODEL_SETTINGS_H_


#include <inttypes.h>

#include "model_wifi_settings.h"
#include "model_mqtt_settings.h"


class Settings
{
public: 
  typedef enum 
  {
    Ok = 0,
    Error = -1
  } ERc;


private:
  WifiSettings m_WifiSettings;
  MqttSettings m_MqttSettings;

public:
  Settings(void);

  void begin(void);
  void end(void);

  ERc save(void);
  ERc load(void);
  void clear(void);

  WifiSettings *getWifiSettings(void);
  MqttSettings *getMqttSettings(void);
};


#endif