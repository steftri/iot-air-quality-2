#include <Arduino.h>

#include "controller.h"
#include "controller_hw.h"
#include "controller_wifi_action.h"
#include "controller_mqtt_action.h"

#include "debug.h"


#define VALUE_PUBLISH_INTERVAL_MS  5000  // every 5 seconds


ControllerFacade::ControllerFacade(ViewFacade *p_ViewFacade)
 : mp_ViewFacade{p_ViewFacade}
 , mu32_NextValueScanCycle{0}
{
  m_WifiController.setSettings(m_Settings.getWifiSettings());
  m_WifiController.setAction(&m_WifiAction);

  m_MqttController.setSettings(m_Settings.getMqttSettings());
  m_MqttController.setAction(&m_MqttAction);  

  m_MqttController.setTopicReceivedCallback(&ViewFacade::onMqttTopicReceived);  // if the MQTT is not seen as an user interface, 
                                                                                // the callback function could also be located 
                                                                                // in the ControllerFacade class 
}


ViewFacade *ControllerFacade::getViewFacade(void)
{
  return mp_ViewFacade;
}


void ControllerFacade::setup(void)
{
  m_HwController.setup();
  m_WifiController.setup();
  m_MqttController.setup();

  if(m_Settings.load()==Settings::Ok)
  {
    debug.println(Debug::Info, "Settings loaded");
    if(m_Settings.getWifiSettings()->getNetworkCount()>0)
      m_WifiController.begin();
  }
  else
  {
    debug.println(Debug::Warning, "No valid settings found");      
  }
  mu32_NextValueScanCycle = millis();
}



void ControllerFacade::loop(void)
{
  m_HwController.loop();
  m_WifiController.loop();
  m_MqttController.loop();

  // trigger for publishing messurement data - if this gets more 
  // complicated, it should be moved to an own controller
  if(mu32_NextValueScanCycle<=millis())
  {
    float f32_Value = m_HwController.readAnalogValue();
    mu32_NextValueScanCycle+=VALUE_PUBLISH_INTERVAL_MS;  // wraparound error after 49.7 days 
    m_MqttAction.publishMessurementData(f32_Value);
  }
}





ControllerFacade::ERc ControllerFacade::setDbgMinSevLevel(Debug::ESeverity e_MinSevLevel)
{
  debug.setMinSeverityLevel(e_MinSevLevel);
  return Ok;
}



ControllerFacade::ERc ControllerFacade::saveSettings(void)
{
  if(m_Settings.save()!=Settings::Ok)
  {
    Serial.println("Error! Settings not saved.");
    return Error;
  }
  return Ok;
}



ControllerFacade::ERc ControllerFacade::clearSettings(void)
{
  m_Settings.clear();  
  return Ok;
}



void ControllerFacade::printSettings(void)
{
  Serial.println("WIFI SSIDs:");
  for(auto i=0; i<m_Settings.getWifiSettings()->getNetworkCount(); i++)
  {
    Serial.print("  ");
    Serial.println(m_Settings.getWifiSettings()->getNetworkSSID(i));
  }

  Serial.print("MQTT Broker: ");
  Serial.print(m_Settings.getMqttSettings()->getBrokerAddr());
  Serial.print(", Port: ");  
  Serial.println(m_Settings.getMqttSettings()->getBrokerPort());
}



void ControllerFacade::setWifiSettings(const char *pc_SSID, const char *pc_Passphrase)
{
  m_Settings.getWifiSettings()->setNetwork(pc_SSID, pc_Passphrase);
}




void ControllerFacade::connectWifi(const char *pc_SSID, const char *pc_Passphrase)
{
  Serial.print("Connecting to WiFi... SSID: ");
  Serial.println(pc_SSID);   
  Serial.flush();

  m_Settings.getWifiSettings()->setNetwork(pc_SSID, pc_Passphrase);
  m_WifiController.begin();
}

void ControllerFacade::connectWifi(void)
{
  m_WifiController.begin();
}


void ControllerFacade::disconnectWifi(void)
{
  m_WifiController.end();  
}


const char *ControllerFacade::getCurrentIP(void)
{
  return m_WifiController.getLocalIp();
}


void ControllerFacade::printWifiStatus(void)
{
  Serial.print("Status: ");
  auto WifiState = m_WifiController.getState(); 
  switch(WifiState)
  {
    case WifiState::Idle:       Serial.println("idle"); break;
    case WifiState::Connecting: Serial.println("connecting"); break;
    case WifiState::Connected:  Serial.print("connected to "); Serial.println(m_WifiController.getCurrentSSID()); break;
    case WifiState::Error:      Serial.println("ERROR"); break;
    default: Serial.println("unknown"); break;
  }

  // additional information, depending on the state
  if(WifiState == WifiState::Connected)
  {
    Serial.print("IP: ");
    Serial.println(m_WifiController.getLocalIp());
  }
}




void ControllerFacade::setMqttSettings(const char *pc_BrokerAddr, const uint16_t u16_BrokerPort)
{
  m_Settings.getMqttSettings()->setBroker(pc_BrokerAddr, u16_BrokerPort);
} 

void ControllerFacade::setMqttSettings(const char *pc_BrokerAddr)
{
  m_Settings.getMqttSettings()->setBroker(pc_BrokerAddr);
} 


void ControllerFacade::connectMqtt(void)
{
  m_MqttController.begin();
}


void ControllerFacade::disconnectMqtt(void)
{
  m_MqttController.end();  
}


void ControllerFacade::printMqttStatus(void)
{
  Serial.print("Status: ");
  auto MqttState = m_MqttController.getState(); 
  switch(MqttState)
  {
    case MqttState::Idle:       Serial.println("idle"); break;
    case MqttState::Connecting: Serial.println("connecting"); break;
    case MqttState::Connected:  Serial.println("connected"); break;
    case MqttState::Error:      Serial.println("ERROR"); break;
    default: Serial.println("unknown"); break;
  }
}


ControllerFacade::ERc ControllerFacade::registerMqttTopic(const char *pc_Topic)
{
  return (MqttController::Ok==m_MqttController.registerTopic(pc_Topic)?Ok:Error);
}


ControllerFacade::ERc ControllerFacade::publishMqttMessage(const char *pc_Topic, const char *pc_Content, const uint8_t u8_QoS, const bool b_Retain)
{
  if(MqttState::Connected==m_MqttController.getState()) 
    return (MqttController::Ok==m_MqttController.publish(pc_Topic, pc_Content, u8_QoS, b_Retain)?Ok:Error);
  return Error; 
}



uint64_t ControllerFacade::getUid(void)
{
  return m_HwController.getUid();
}


void ControllerFacade::reset(void)
{
  Serial.println("resetting device...");
  Serial.flush();
  m_HwController.reset();
}


void ControllerFacade::setLed(const uint8_t u8_Led, bool b_On)
{
  m_HwController.setLed(u8_Led, b_On);
}
