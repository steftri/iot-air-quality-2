#include <Arduino.h>
#include <ArduinoMqttClient.h>

#include "debug.h"

#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif

#include "controller_mqtt.h"



#define CONNECTION_RETRY_INTERVAL 10000  // in ms; MQTT reconnect interval 10s
#define MAX_CONNECTION_ATTEMPTS      10




WiFiClient myMqttTcpClient;
MqttClient myMqttClient(&myMqttTcpClient);  




/*****************************************************/
/*     implementation of behaviour in IDLE state     */
/*****************************************************/

void MqttStateIdle::init(MqttController *p_Controller)
{
  debug.println(Debug::Info, "MQTT state: idle");    

  if(p_Controller && p_Controller->getStateAction())
    p_Controller->getStateAction()->idle();
}


void MqttStateIdle::loop(MqttController *p_Controller)
{
}


MqttState::EState MqttStateIdle::getState(void)
{
  return MqttState::Idle;
}



/*****************************************************/
/*    implementation of behaviour in ERROR state     */
/*****************************************************/

void MqttStateError::init(MqttController *p_Controller)
{
  debug.println(Debug::Info, "MQTT state: error");    

  if(p_Controller && p_Controller->getStateAction())
    p_Controller->getStateAction()->error();
}


void MqttStateError::loop(MqttController *p_Controller)
{
}


MqttState::EState MqttStateError::getState(void)
{
  return MqttState::Error;
}



/*****************************************************/
/*  implementation of behaviour in CONNECTING state  */
/*****************************************************/

void MqttStateConnecting::init(MqttController *p_Controller)
{
  if(!p_Controller)
    return;

  if(!strcmp("", p_Controller->getSettings()->getBrokerAddr()))
  {
    debug.println(Debug::Warning, "No MQTT broker address");    
    p_Controller->setState(MqttState::Error);
    return;
  }

  debug.println(Debug::Info, "MQTT state: connecting");    

  if(p_Controller && p_Controller->getStateAction())
    p_Controller->getStateAction()->connecting();

  mu32_NextConnectionAttempt = millis();
  mu16_ConnectionAttempts = 0;
}


void MqttStateConnecting::loop(MqttController *p_Controller)
{
  if(!p_Controller)
    return;

  if(millis()>mu32_NextConnectionAttempt)
  {
    mu16_ConnectionAttempts++;
 
    const char *pc_BrokerAddr = p_Controller->getSettings()->getBrokerAddr();
    uint16_t u16_BrokerPort = p_Controller->getSettings()->getBrokerPort();

    char ac_DbgConnectMsg[80];
    snprintf(ac_DbgConnectMsg, 80, "Trying to connect to %s:%i", pc_BrokerAddr, u16_BrokerPort);
    debug.println(Debug::Info, ac_DbgConnectMsg);

    if(myMqttClient.connect(pc_BrokerAddr, u16_BrokerPort))
    {
      p_Controller->setState(MqttState::Connected);
      return;    
    }
    else
    {
      snprintf(ac_DbgConnectMsg, 80, "Connection to %s failed - error code: %i", pc_BrokerAddr, myMqttClient.connectError());
      debug.println(Debug::Info, ac_DbgConnectMsg);

      if(mu16_ConnectionAttempts>MAX_CONNECTION_ATTEMPTS)
      {
        p_Controller->setState(MqttState::Error);
      }
      mu32_NextConnectionAttempt = millis()+CONNECTION_RETRY_INTERVAL;
    }
  }
}


MqttState::EState MqttStateConnecting::getState(void)
{
  return MqttState::Connecting;
}


/*****************************************************/
/*  implementation of behaviour in CONNECTED state   */
/*****************************************************/

void MqttStateConnected::init(MqttController *p_Controller)
{
  if(!p_Controller)
    return;

  debug.println(Debug::Info, "MQTT state: connected");    

  if(p_Controller->getStateAction())
    p_Controller->getStateAction()->connected();
}


void MqttStateConnected::loop(MqttController *p_Controller)
{
  myMqttClient.poll();  // handling of keepalive messages
}



MqttState::EState MqttStateConnected::getState(void)
{
  return MqttState::Connected;
}









MqttController::MqttController(MqttSettings *p_Settings, MqttStateAction *p_StateAction)
  : mp_CurrentState{&m_StateIdle}
  , mp_Settings{p_Settings}
  , mp_StateAction{p_StateAction}  
{
}


void MqttController::setSettings(MqttSettings *p_Settings)
{
  mp_Settings = p_Settings;
}


MqttSettings *MqttController::getSettings(void)
{
  return mp_Settings;
}



void MqttController::setStateAction(MqttStateAction *p_StateAction)
{
  mp_StateAction = p_StateAction;
}



MqttStateAction *MqttController::getStateAction(void)
{
  return mp_StateAction;
}




void MqttController::setup(void)
{
  setState(MqttState::Idle);
}


void MqttController::loop(void)
{
  mp_CurrentState->loop(this);
}

  
void MqttController::begin(void)
{ 
  if(MqttState::Idle == getState())
    setState(MqttState::Connecting);
}




void MqttController::end(void)
{
  setState(MqttState::Idle);
}


MqttState::EState MqttController::getState(void)
{
  return mp_CurrentState->getState();
}


void MqttController::setState(MqttState::EState e_NewState)
{
  switch(e_NewState)
  {
    case MqttState::Idle: 
      mp_CurrentState=&m_StateIdle; 
      break;
    case MqttState::Connecting: 
      mp_CurrentState=&m_StateConnecting; 
      break;
    case MqttState::Connected: 
      mp_CurrentState=&m_StateConnected; 
      break;      
    default:
      mp_CurrentState=&m_StateError; 
      return;     
  }
  mp_CurrentState->init(this);
} 



int MqttController::publish(const char *pc_Topic, const char *pc_Content, const uint8_t u8_QoS, const bool b_Retain)
{
  if(mp_CurrentState->getState()!=MqttState::Connected)
  {
    debug.println(Debug::Warning, "MQTT broker is not connected");
    return -1;
  }

  char ac_Dbg[128];
  snprintf(ac_Dbg, sizeof(ac_Dbg), "Publishing MQTT topic '%s': '%s'", pc_Topic, pc_Content);
  debug.println(Debug::Info, ac_Dbg);

  myMqttClient.beginMessage(pc_Topic, b_Retain, u8_QoS);
  myMqttClient.print(pc_Content);
  myMqttClient.endMessage();

  return 0;
}



int MqttController::pickupTopic(String *p_Topic, String *p_Message, int MqttMsgSize)
{
  char ac_Message[MAX_MQTT_MESSAGE+1];
  uint16_t i;

  if(p_Topic)
    *p_Topic = myMqttClient.messageTopic();

  for(i=0; i<MqttMsgSize && myMqttClient.available() && i<MAX_MQTT_MESSAGE; i++) 
    ac_Message[i]=(char)myMqttClient.read();
  ac_Message[i] = '\0';

  if(myMqttClient.available())   // clean up if message was longer than our buffer
    myMqttClient.flush();

  if(p_Message)
    *p_Message = ac_Message;

  debug.println(Debug::Info, "MQTT topic received");

  return 0;
}
