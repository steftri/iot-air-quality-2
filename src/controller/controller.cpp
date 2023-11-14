#include <Arduino.h>
#include <ArduinoJson.h>

#include "controller.h"

#include "LedIndicatorAdapter.h"

#define LED_WIFI   13    // LED_BUILTIN
#define LED_MQTT   12

#define MQTT_IOT_DEVICE_NAME "iotdevice"


LedIndicatorAdapter myWifiIndicatorAdapter(LED_WIFI);
Indicator myWifiIndicator(&myWifiIndicatorAdapter);

LedIndicatorAdapter myMqttIndicatorAdapter(LED_MQTT);
Indicator myMqttIndicator(&myMqttIndicatorAdapter);


extern ControllerFacade myController;





void MyWifiStateAction::idle(void) 
{
  myWifiIndicator.clear();
  myController.disconnectMqtt(); 
}

void MyWifiStateAction::connecting(void) 
{
  myWifiIndicator.blink();
}

void MyWifiStateAction::connected(void) 
{
  myWifiIndicator.set();
  myController.connectMqtt();
}

void MyWifiStateAction::disconnected(void) 
{
  myWifiIndicator.clear();
  myController.disconnectMqtt();  
}

void MyWifiStateAction::error(void) 
{
  myWifiIndicator.blink();
  myController.disconnectMqtt(); 
}  



void MyMqttStateAction::idle(void) 
{
  myMqttIndicator.clear();
}

void MyMqttStateAction::connecting(void) 
{
  myMqttIndicator.blink();
}




void MyMqttStateAction::connected(void) 
{
  myMqttIndicator.set();

  uint64_t u64_uid; 
  StaticJsonDocument<32> jsonDoc;
  char ac_SerializedContent[32]; 
  char ac_Topic[48+1];

  u64_uid = myController.getUid();

  jsonDoc["uid"] = u64_uid;
  serializeJson(jsonDoc, ac_SerializedContent, sizeof(ac_SerializedContent));
  myController.publishMqttMessage(MQTT_IOT_DEVICE_NAME, ac_SerializedContent, 1, true);

  snprintf(ac_Topic, sizeof(ac_Topic), MQTT_IOT_DEVICE_NAME "/%llu/command", u64_uid);
  myController.registerMqttTopic(ac_Topic);
}

void MyMqttStateAction::disconnected(void) 
{
  myMqttIndicator.clear();
}

void MyMqttStateAction::error(void) 
{
  myMqttIndicator.blink();
}  








ControllerFacade::ControllerFacade(ViewFacade *p_ViewFacade)
 : mp_ViewFacade{p_ViewFacade}
{
  m_WifiController.setSettings(m_Settings.getWifiSettings());
  m_WifiController.setStateAction(&m_WifiStateAction);

  m_MqttController.setSettings(m_Settings.getMqttSettings());
  m_MqttController.setStateAction(&m_MqttStateAction);  
  m_MqttController.setTopicReceivedCallback(&ControllerFacade::onMqttTopicReceived);
}


ViewFacade *ControllerFacade::getViewFacade(void)
{
  return mp_ViewFacade;
}


void ControllerFacade::setup(void)
{
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
}



void ControllerFacade::loop(void)
{
  m_WifiController.loop();
  m_MqttController.loop();
}



uint64_t ControllerFacade::getUid(void)
{
  return ESP.getEfuseMac();
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


String ControllerFacade::getCurrentIP(void)
{
  return m_WifiController.getLocalIp().toString();
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
  else
  { /*
    Serial.println("Networks:");
    auto n=WiFi.scanNetworks();
    for (int i = 0; i < n; i++)
    {
      Serial.println(WiFi.SSID(i));
    }
    */
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
  return (MqttController::Ok==m_MqttController.publish(pc_Topic, pc_Content, u8_QoS, b_Retain)?Ok:Error);
}


void ControllerFacade::onMqttTopicReceived(const char *pc_Topic, const char *pc_Content)
{
  debug.println(Debug::Trace, "ControllerFacade::onTopicReceived()");

  Serial.print("MQTT Topic: ");
  Serial.print(pc_Topic);
  Serial.print(", Content: ");
  Serial.println(pc_Content);
}

