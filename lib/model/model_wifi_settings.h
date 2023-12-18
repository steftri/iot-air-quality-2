#ifndef _MODEL_WIFI_SETTINGS_H_
#define _MODEL_WIFI_SETTINGS_H_

#include <inttypes.h>

#define MAX_WIFI_NETWORKS           4
#define MAX_SSID_LENGTH            32
#define MAX_WPA2_PASSWORD_LENGTH   63

#define WIFI_SETTINGS_SIZE        (1+MAX_WIFI_NETWORKS*(MAX_SSID_LENGTH+MAX_WPA2_PASSWORD_LENGTH))

class WifiSettings
{
  struct 
  {
    char ac_SSID[MAX_SSID_LENGTH+1];
    char ac_Password[MAX_WPA2_PASSWORD_LENGTH+1];    
  } ma_Networks[MAX_WIFI_NETWORKS];

  uint8_t mu8_WifiNetworkCount; 

public:
  WifiSettings(void);

  void init(void);
  void setNetwork(const char *pc_SSID, const char *pc_Password);
  uint8_t getNetworkCount(void);
  const char *getNetworkSSID(const uint8_t u8_Index);
  const char *getNetworkPassword(const uint8_t u8_Index);

  uint16_t serialize(uint8_t *pu8_Buffer, const uint16_t u16_BufferSize);
  void unserialize(const uint8_t *pu8_Buffer, const uint16_t u16_Size);
};

#endif
