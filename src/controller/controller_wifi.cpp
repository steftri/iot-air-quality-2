#include <Arduino.h>

#include "debug.h"

#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif

#include "LedIndicatorAdapter.h"

#include "controller_wifi.h"



#define CONNECTION_RETRY_INTERVAL     500   // in ms
#define MAX_SAME_NETWORK_RETRY_COUNT    5





/*****************************************************/
/*     implementation of behaviour in IDLE state     */
/*****************************************************/


void WifiStateIdle::init(WifiController *p_Controller)
{
  debug.println(Debug::Info, "WIFI state: idle");    

  if(p_Controller && p_Controller->getStateAction())
    p_Controller->getStateAction()->idle();

  WiFi.disconnect();
}


void WifiStateIdle::loop(WifiController *p_Controller)
{
}


WifiState::EState WifiStateIdle::getState(void)
{
  return WifiState::Idle;
}



/*****************************************************/
/*    implementation of behaviour in ERROR state     */
/*****************************************************/

void WifiStateError::init(WifiController *p_Controller)
{
  debug.println(Debug::Info, "WIFI state: error");    

  if(p_Controller && p_Controller->getStateAction())
    p_Controller->getStateAction()->error();  
}


void WifiStateError::loop(WifiController *p_Controller)
{
}


WifiState::EState WifiStateError::getState(void)
{
  return WifiState::Error;
}



/*****************************************************/
/*  implementation of behaviour in CONNECTING state  */
/*****************************************************/


void WifiStateConnecting::init(WifiController *p_Controller)
{
  uint8_t u8_WifiNetworkCount = 0;

  if(!p_Controller)
    return;

  if(p_Controller->getSettings())
    u8_WifiNetworkCount = p_Controller->getSettings()->getNetworkCount();

  if(u8_WifiNetworkCount==0)
  {
    debug.println(Debug::Warning, "No WIFI SSID configured");    
    p_Controller->setState(WifiState::Error);
    return;
  }

  debug.println(Debug::Info, "WIFI state: Connecting");    

  if(p_Controller->getStateAction())
    p_Controller->getStateAction()->connecting();

  if(WL_CONNECTED==WiFi.status())
    WiFi.disconnect();

  WiFi.mode(WIFI_STA); // Optional
  mu32_NextConnectionAttempt = millis();
  mu8_SameNetworkConnectRetries = 0;
  mu8_CurrentNetworkIndex = 0; 
}


void WifiStateConnecting::loop(WifiController *p_Controller)
{
  if(!p_Controller)
    return;

  WifiSettings *p_WifiSettings = p_Controller->getSettings();
  if(WL_CONNECTED==WiFi.status())
  {
    p_Controller->setCurrentSSID(p_WifiSettings->getNetworkSSID(mu8_CurrentNetworkIndex));
    p_Controller->setState(WifiState::Connected);
    return;
  }
  else if(millis()>mu32_NextConnectionAttempt)
  {
    if(!p_WifiSettings)
    {
      p_Controller->setState(WifiState::Error);
      return;
    }

    mu8_SameNetworkConnectRetries++;
    if(mu8_SameNetworkConnectRetries>MAX_SAME_NETWORK_RETRY_COUNT)
    {
      mu8_CurrentNetworkIndex++;
      if(mu8_CurrentNetworkIndex>p_WifiSettings->getNetworkCount())
        mu8_CurrentNetworkIndex=0;  
      mu8_SameNetworkConnectRetries = 1;   
    }

    char ac_DbgConnectMsg[80];
    snprintf(ac_DbgConnectMsg, 80, "Trying to connect to \"%s\"", p_WifiSettings->getNetworkSSID(mu8_CurrentNetworkIndex));
    debug.println(Debug::Info, ac_DbgConnectMsg); 
    
    WiFi.begin(p_WifiSettings->getNetworkSSID(mu8_CurrentNetworkIndex), p_WifiSettings->getNetworkPassword(mu8_CurrentNetworkIndex));   // TODO: select other networks 
    mu32_NextConnectionAttempt = millis()+CONNECTION_RETRY_INTERVAL;
  }
}


WifiState::EState WifiStateConnecting::getState(void)
{
  return WifiState::Connecting;
}



/*****************************************************/
/*  implementation of behaviour in CONNECTED state   */
/*****************************************************/

void WifiStateConnected::init(WifiController *p_Controller)
{
  if(!p_Controller)
    return;

  debug.println(Debug::Info, "WIFI state: connected");    

  if(p_Controller->getStateAction())
    p_Controller->getStateAction()->connected();
}


void WifiStateConnected::loop(WifiController *p_Controller)
{
  if(!p_Controller)
    return;

  if(WL_CONNECTED!=WiFi.status())
  {
    debug.println(Debug::Info, "WLAN connection lost");

    if(p_Controller->getStateAction())
      p_Controller->getStateAction()->disconnected();
    p_Controller->setState(WifiState::Connecting);     
  }
}


WifiState::EState WifiStateConnected::getState(void)
{
  return WifiState::Connected;
}




/*****************************************************/
/*     implementation of INTERFACE                   */
/*****************************************************/

WifiController::WifiController(WifiSettings *p_Settings, WifiStateAction *p_StateAction)
  : mp_CurrentState{&m_StateIdle}
  , mp_Settings{p_Settings}
  , mp_StateAction{p_StateAction}
  , mac_CurrentSSID{""}
{
}


void WifiController::setSettings(WifiSettings *p_Settings)
{
  mp_Settings = p_Settings;
}


WifiSettings *WifiController::getSettings(void)
{
  return mp_Settings;
}


void WifiController::setStateAction(WifiStateAction *p_StateAction)
{
  mp_StateAction = p_StateAction;
}


WifiStateAction *WifiController::getStateAction(void)
{
  return mp_StateAction;
}


void WifiController::setup(void)
{
  setState(WifiState::Idle);
}


void WifiController::loop(void)
{
  mp_CurrentState->loop(this);
}


void WifiController::begin(void)
{
  if(WifiState::Idle == getState())
    setState(WifiState::Connecting);
}


void WifiController::end(void)
{
  setState(WifiState::Idle);
}


WifiState::EState WifiController::getState(void)
{
  return mp_CurrentState->getState();
}


void WifiController::setState(WifiState::EState e_NewState)
{
  switch(e_NewState)
  {
    case WifiState::Idle: 
      mp_CurrentState=&m_StateIdle; 
      break;
    case WifiState::Connecting: 
      mp_CurrentState=&m_StateConnecting; 
      break;
    case WifiState::Connected: 
      mp_CurrentState=&m_StateConnected; 
      break; 
    default:
      mp_CurrentState=&m_StateError;     
      return;     
  }
  mp_CurrentState->init(this);
} 


void WifiController::setCurrentSSID(const char *pc_SSID)
{
  strncpy(mac_CurrentSSID, pc_SSID, MAX_SSID_LENGTH);
  mac_CurrentSSID[MAX_SSID_LENGTH] = '\0';
}

const char *WifiController::getCurrentSSID(void)
{
  if(!mp_Settings)
    return nullptr;

  if(getState()==WifiState::EState::Connected)
    return mac_CurrentSSID;

  return ""; 
}


IPAddress WifiController::getLocalIp(void)
{
  return WiFi.localIP();
}
