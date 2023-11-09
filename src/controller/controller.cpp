#include <Arduino.h>

#include "controller.h"

#include "LedIndicatorAdapter.h"




#define WIFI_INDICATOR   13
#define TCP_INDICATOR   12
#define UDP_INDICATOR   14

LedIndicatorAdapter myWifiIndicatorAdapter(WIFI_INDICATOR);
Indicator myWifiIndicator(&myWifiIndicatorAdapter);

LedIndicatorAdapter myTcpIndicatorAdapter(TCP_INDICATOR);
Indicator myTcpIndicator(&myTcpIndicatorAdapter);

LedIndicatorAdapter myUdpIndicatorAdapter(UDP_INDICATOR);
Indicator myUdpIndicator(&myUdpIndicatorAdapter);



extern ControllerFacade myController;





void MyWifiStateAction::idle(void) 
{
  myWifiIndicator.clear();
}

void MyWifiStateAction::connecting(void) 
{
  myWifiIndicator.blink();
}

void MyWifiStateAction::connected(void) 
{
  myWifiIndicator.set();
}

void MyWifiStateAction::disconnected(void) 
{
  myWifiIndicator.clear();
}

void MyWifiStateAction::error(void) 
{
  myWifiIndicator.blink();
}  






ControllerFacade::ControllerFacade(ViewFacade *p_ViewFacade)
 : mp_ViewFacade{p_ViewFacade}
{
  m_WifiController.setSettings(m_Settings.getWifiSettings());
  m_WifiController.setStateAction(&m_WifiStateAction);
}


ViewFacade *ControllerFacade::getViewFacade(void)
{
  return mp_ViewFacade;
}


void ControllerFacade::setup(void)
{
  m_WifiController.setup();

  if(0==m_Settings.load())
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
}



ControllerFacade::ERc ControllerFacade::setDbgMinSevLevel(Debug::ESeverity e_MinSevLevel)
{
  debug.setMinSeverityLevel(e_MinSevLevel);
  return Ok;
}



ControllerFacade::ERc ControllerFacade::saveSettings(void)
{
  if(0!=m_Settings.save())
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
