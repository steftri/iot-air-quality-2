#include <memory.h>

#include "model_wifi_settings.h"


WifiSettings::WifiSettings(void)
{
  init();
}


void WifiSettings::init(void)
{
  mu8_WifiNetworkCount = 0;
  for(uint8_t u8_Index = 0; u8_Index<MAX_WIFI_NETWORKS; u8_Index++)
  {
    ma_Networks[u8_Index].ac_SSID[0] = '\0';
    ma_Networks[u8_Index].ac_Password[0] = '\0';
  }
}



void WifiSettings::setNetwork(const char *pc_SSID, const char *pc_Password)
{
  // if the network is already known, move it to the front of the list (index 0) and replace its passwort with the new one 
  for(uint8_t u8_Index = 0; u8_Index<mu8_WifiNetworkCount; u8_Index++)
  {
    if(0 == strncmp(pc_SSID,  ma_Networks[u8_Index].ac_SSID, MAX_SSID_LENGTH))
    { 
      for(uint8_t i = u8_Index; i>0; i--)
      {  
        // i is the new position of the entry
        strncpy(ma_Networks[i].ac_SSID, ma_Networks[i-1].ac_SSID, MAX_SSID_LENGTH);
        ma_Networks[i].ac_SSID[MAX_SSID_LENGTH] = '\0';
        strncpy(ma_Networks[i].ac_Password, ma_Networks[i-1].ac_Password, MAX_WPA2_PASSWORD_LENGTH);
        ma_Networks[i].ac_Password[MAX_WPA2_PASSWORD_LENGTH] = '\0';
      }
      strncpy(ma_Networks[0].ac_SSID, pc_SSID, MAX_SSID_LENGTH);
      ma_Networks[0].ac_SSID[MAX_SSID_LENGTH] = '\0';
      strncpy(ma_Networks[0].ac_Password, pc_Password, MAX_WPA2_PASSWORD_LENGTH);
      ma_Networks[0].ac_Password[MAX_WPA2_PASSWORD_LENGTH] = '\0';
      return;
    }
  }

  // if there already are other known networks, move them down. The new network gets the highest priority.
  if(mu8_WifiNetworkCount>0)
  {
    for(uint8_t i = (mu8_WifiNetworkCount<MAX_WIFI_NETWORKS)?mu8_WifiNetworkCount:(MAX_WIFI_NETWORKS-1); i>0; i--)
    {  
      // i is the new position of the entry
      strncpy(ma_Networks[i].ac_SSID, ma_Networks[i-1].ac_SSID, MAX_SSID_LENGTH);
      ma_Networks[i].ac_SSID[MAX_SSID_LENGTH] = '\0';
      strncpy(ma_Networks[i].ac_Password, ma_Networks[i-1].ac_Password, MAX_WPA2_PASSWORD_LENGTH);
      ma_Networks[i].ac_Password[MAX_WPA2_PASSWORD_LENGTH] = '\0';
    }  
  }
  strncpy(ma_Networks[0].ac_SSID, pc_SSID, MAX_SSID_LENGTH);
  ma_Networks[0].ac_SSID[MAX_SSID_LENGTH] = '\0';
  strncpy(ma_Networks[0].ac_Password, pc_Password, MAX_WPA2_PASSWORD_LENGTH);
  ma_Networks[0].ac_Password[MAX_WPA2_PASSWORD_LENGTH] = '\0';

  if(mu8_WifiNetworkCount<MAX_WIFI_NETWORKS)
    mu8_WifiNetworkCount++;
}


uint8_t WifiSettings::getNetworkCount(void)
{
  return mu8_WifiNetworkCount;
}


const char *WifiSettings::getNetworkSSID(const uint8_t u8_Index)
{
  if((u8_Index>=MAX_WIFI_NETWORKS) || (u8_Index>=mu8_WifiNetworkCount))
    return nullptr;
  return ma_Networks[u8_Index].ac_SSID;
}



const char *WifiSettings::getNetworkPassword(const uint8_t u8_Index)
{
  if((u8_Index>=MAX_WIFI_NETWORKS) || (u8_Index>=mu8_WifiNetworkCount))
    return nullptr;
  return ma_Networks[u8_Index].ac_Password;
}


uint16_t WifiSettings::serialize(uint8_t *pu8_Buffer, const uint16_t u16_BufferSize)
{
  uint16_t u16_BufferPos = 0;

  if((!pu8_Buffer) || (u16_BufferSize<WIFI_SETTINGS_SIZE))
    return 0;

  pu8_Buffer[u16_BufferPos++] = mu8_WifiNetworkCount; 

  for(uint8_t u8_Index = 0; u8_Index<MAX_WIFI_NETWORKS; u8_Index++)
  {
    for(auto i=0; i<MAX_SSID_LENGTH; i++)
      pu8_Buffer[u16_BufferPos++] = ma_Networks[u8_Index].ac_SSID[i];
    for(auto i=0; i<MAX_WPA2_PASSWORD_LENGTH; i++)
      pu8_Buffer[u16_BufferPos++] = ma_Networks[u8_Index].ac_Password[i]; 
  }

  return u16_BufferPos;
}


void WifiSettings::unserialize(const uint8_t *pu8_Buffer, const uint16_t u16_Size)
{
  uint16_t u16_BufferPos = 0;

  if((!pu8_Buffer) || (u16_Size<WIFI_SETTINGS_SIZE))
    return;

  mu8_WifiNetworkCount = pu8_Buffer[u16_BufferPos++];

  for(uint8_t u8_Index=0; u8_Index<MAX_WIFI_NETWORKS; u8_Index++)
  {
    for(auto i=0; i<MAX_SSID_LENGTH; i++)
      ma_Networks[u8_Index].ac_SSID[i] = pu8_Buffer[u16_BufferPos++];
    ma_Networks[u8_Index].ac_SSID[MAX_SSID_LENGTH]=0;   

    for(auto i=0; i<MAX_WPA2_PASSWORD_LENGTH; i++)
      ma_Networks[u8_Index].ac_Password[i] = pu8_Buffer[u16_BufferPos++];
    ma_Networks[u8_Index].ac_Password[MAX_WPA2_PASSWORD_LENGTH]=0;
  }
}
