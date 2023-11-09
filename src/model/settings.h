#ifndef _MODEL_SETTINGS_H_
#define _MODEL_SETTINGS_H_


#include <inttypes.h>

#include "model_wifi_settings.h"


class Settings
{
  WifiSettings m_WifiSettings;

public:
  Settings(void);

  void begin(void);

  int16_t save(void);
  int16_t load(void);
  void clear(void);

  WifiSettings *getWifiSettings(void);
};


#endif