#include <Arduino.h>

#include <EEPROM.h>
#include <memory.h>

#include <CRC32.h>

#include "settings.h"



#define EEPROM_SIZE              512  // must be anything between 4 and 4096


const uint32_t EEPROM_SETTINGS_MAGIC   = 0x1acffc1d;
const uint16_t EEPROM_SETTINGS_VERSION = 2;


Settings::Settings(void)  
{
}



void Settings::begin(void)
{
}



Settings::ERc Settings::save(void)
{
  uint16_t u16_EepromPos = 0;
  uint8_t au8_WifiSettingBuffer[1+MAX_WIFI_NETWORKS*(MAX_SSID_LENGTH+MAX_WPA2_PASSWORD_LENGTH)];
  uint16_t u16_WifiSettingSize; 
  uint8_t au8_VersionBuffer[2];
  CRC32 crc;
  uint32_t u32_crc;

  EEPROM.begin(EEPROM_SIZE);

  EEPROM.write(u16_EepromPos++, static_cast<uint8_t>(EEPROM_SETTINGS_MAGIC>>24));
  EEPROM.write(u16_EepromPos++, static_cast<uint8_t>(EEPROM_SETTINGS_MAGIC>>16));
  EEPROM.write(u16_EepromPos++, static_cast<uint8_t>(EEPROM_SETTINGS_MAGIC>>8));
  EEPROM.write(u16_EepromPos++, static_cast<uint8_t>(EEPROM_SETTINGS_MAGIC));

  au8_VersionBuffer[0] = static_cast<uint8_t>(EEPROM_SETTINGS_VERSION>>8);
  au8_VersionBuffer[1] = static_cast<uint8_t>(EEPROM_SETTINGS_VERSION);
  for(auto i=0; i<2; i++)
    EEPROM.write(u16_EepromPos++, au8_VersionBuffer[i]);  
  crc.add(au8_VersionBuffer, 2);
  
  u16_WifiSettingSize = m_WifiSettings.serialize(au8_WifiSettingBuffer, sizeof(au8_WifiSettingBuffer));
  for(auto i=0; i<u16_WifiSettingSize; i++)
    EEPROM.write(u16_EepromPos++, au8_WifiSettingBuffer[i]);  
  crc.add(au8_WifiSettingBuffer, u16_WifiSettingSize);

  u32_crc = crc.calc();
  EEPROM.write(u16_EepromPos++, static_cast<uint8_t>(u32_crc>>24));
  EEPROM.write(u16_EepromPos++, static_cast<uint8_t>(u32_crc>>16));
  EEPROM.write(u16_EepromPos++, static_cast<uint8_t>(u32_crc>>8));
  EEPROM.write(u16_EepromPos++, static_cast<uint8_t>(u32_crc));  

  EEPROM.commit();
  EEPROM.end();

  return Ok;     
}



Settings::ERc Settings::load(void)
{
  uint32_t u32_EepromPos = 0;  
  uint32_t u32_Magic;
  uint16_t u16_Version;
  uint8_t au8_VersionBuffer[2];
  uint8_t au8_WifiSettingBuffer[WIFI_SETTINGS_SIZE];
  CRC32 crc;
  uint32_t u32_crc;

  EEPROM.begin(EEPROM_SIZE);

  u32_Magic  = static_cast<uint32_t>(EEPROM.read(u32_EepromPos++))<<24;
  u32_Magic |= static_cast<uint32_t>(EEPROM.read(u32_EepromPos++))<<16;
  u32_Magic |= static_cast<uint32_t>(EEPROM.read(u32_EepromPos++))<<8;
  u32_Magic |= static_cast<uint32_t>(EEPROM.read(u32_EepromPos++)); 

  for(uint16_t i=0; i<2; i++)
    au8_VersionBuffer[i] = EEPROM.read(u32_EepromPos++);
  crc.add(au8_VersionBuffer, 2);    

  u16_Version = (static_cast<uint16_t>(au8_VersionBuffer[0])<<8) | static_cast<uint16_t>(au8_VersionBuffer[1]);
  if((u32_Magic!=EEPROM_SETTINGS_MAGIC) || (u16_Version!=EEPROM_SETTINGS_VERSION))
  {
    EEPROM.end();
    return Error;     
  }

  for(uint16_t i=0; (i<WIFI_SETTINGS_SIZE) && (i<sizeof(au8_WifiSettingBuffer)); i++)
    au8_WifiSettingBuffer[i] = EEPROM.read(u32_EepromPos++);
  crc.add(au8_WifiSettingBuffer, WIFI_SETTINGS_SIZE);  

  u32_crc  = static_cast<uint32_t>(EEPROM.read(u32_EepromPos++))<<24;
  u32_crc |= static_cast<uint32_t>(EEPROM.read(u32_EepromPos++))<<16;
  u32_crc |= static_cast<uint32_t>(EEPROM.read(u32_EepromPos++))<<8;
  u32_crc |= static_cast<uint32_t>(EEPROM.read(u32_EepromPos++)); 

  EEPROM.end();

  if(u32_crc != crc.calc())
    return Error;

  m_WifiSettings.unserialize(au8_WifiSettingBuffer, sizeof(au8_WifiSettingBuffer));
  return Ok;
}



void Settings::clear(void)
{
  m_WifiSettings.init();
  save();
}



WifiSettings *Settings::getWifiSettings(void)
{
  return &m_WifiSettings;
}

