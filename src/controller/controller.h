#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_


#include "model/settings.h"

#include "controller_hw.h"

#include "controller_wifi.h"
#include "controller/controller_wifi_action.h"

#include "controller_mqtt.h"
#include "controller/controller_mqtt_action.h"

#include "view/view.h"

#include "debug.h"



class ControllerFacade
{
public: 
  typedef enum 
  {
    Ok = 0,
    Error = -1
  } ERc;


private:
  ViewFacade *mp_ViewFacade;

  Settings m_Settings;

  HwController    m_HwController;

  MyWifiAction    m_WifiAction;
  WifiController  m_WifiController; 

  MyMqttAction    m_MqttAction;
  MqttController  m_MqttController; 

  uint32_t mu32_NextValueScanCycle;

public:
  explicit ControllerFacade(ViewFacade *p_ViewFacade);

  ViewFacade *getViewFacade(void);

  void setup(void);
  void loop(void);

  ERc setDbgMinSevLevel(Debug::ESeverity e_MinSevLevel);

  ERc saveSettings(void);
  ERc clearSettings(void);
  void printSettings(void);

  void setWifiSettings(const char *pc_SSID, const char *pc_Passphrase); 

  void connectWifi(const char *pc_SSID, const char *pc_Passphrase);
  void connectWifi(void);
  void disconnectWifi(void);
  const char *getCurrentIP(void);
  void printWifiStatus(void);

  void setMqttSettings(const char *pc_BrokerAddr); 
  void setMqttSettings(const char *pc_BrokerAddr, const uint16_t u16_BrokerPort); 
  void connectMqtt(void);
  void disconnectMqtt(void);
  void printMqttStatus(void);  
  ERc registerMqttTopic(const char *pc_Topic);
  ERc publishMqttMessage(const char *pc_Topic, const char *pc_Content, const uint8_t u8_QoS = 0, const bool b_Retain = false);

  uint64_t getUid(void);
  void reset(void);
  void setLed(const uint8_t u8_Led, bool b_On);
};


#endif