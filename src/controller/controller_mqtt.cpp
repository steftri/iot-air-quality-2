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
#define MAX_CONNECTION_ATTEMPTS       0  // 0 means unlimited retries



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

  if(p_Controller->getStateAction())
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

      if((MAX_CONNECTION_ATTEMPTS>0) && (mu16_ConnectionAttempts>MAX_CONNECTION_ATTEMPTS))
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

  myMqttClient.onMessage(onMessage);

  if(p_Controller->getStateAction())
    p_Controller->getStateAction()->connected();
}


void MqttStateConnected::loop(MqttController *p_Controller)
{
  myMqttClient.poll();  // handling of keepalive messages

  if(!myMqttClient.connected())
  {
    debug.println(Debug::Info, "MQTT connection lost");

    if(p_Controller->getStateAction())
      p_Controller->getStateAction()->disconnected();
    p_Controller->setState(MqttState::Connecting);     
  }  
}



MqttState::EState MqttStateConnected::getState(void)
{
  return MqttState::Connected;
}


void MqttStateConnected::onMessage(int MsgSize)
{
  String Topic;
  char ac_Message[MAX_MQTT_MESSAGE+1];
  uint16_t i;

  Topic = myMqttClient.messageTopic();

  for(i=0; i<MsgSize && myMqttClient.available() && i<MAX_MQTT_MESSAGE; i++) 
    ac_Message[i]=(char)myMqttClient.read();
  ac_Message[i] = '\0';

  if(myMqttClient.available())   // clean up if message was longer than our buffer
    myMqttClient.flush();

  debug.println(Debug::Trace, "MQTT topic received");
  MqttController::onTopicReceived(Topic.c_str(), ac_Message);

  return;
}




MqttController::TTopicReceivedCallback *MqttController::mp_TopicReceivedCallback = nullptr;

MqttController::MqttController(MqttSettings *p_Settings, MqttStateAction *p_StateAction)
  : mp_Settings{p_Settings}
  , mp_CurrentState{&m_StateIdle}
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


void MqttController::setTopicReceivedCallback(TTopicReceivedCallback *p_TopicReceivedCallback)
{
  mp_TopicReceivedCallback = p_TopicReceivedCallback;
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



MqttController::ERc MqttController::publish(const char *pc_Topic, const char *pc_Content, const uint8_t u8_QoS, const bool b_Retain)
{
  if(mp_CurrentState->getState()!=MqttState::Connected)
  {
    debug.println(Debug::Warning, "MQTT broker is not connected");
    return Error;
  }

  char ac_Dbg[128];
  snprintf(ac_Dbg, sizeof(ac_Dbg), "Publishing MQTT topic '%s': '%s'", pc_Topic, pc_Content);
  debug.println(Debug::Info, ac_Dbg);

  myMqttClient.beginMessage(pc_Topic, b_Retain, u8_QoS);
  myMqttClient.print(pc_Content);
  myMqttClient.endMessage();

  return Ok;
}


MqttController::ERc MqttController::registerTopic(const char *pc_Topic)
{
  char ac_Dbg[64];
  snprintf(ac_Dbg, sizeof(ac_Dbg), "Registering MQTT topic '%s'", pc_Topic);
  debug.println(Debug::Info, ac_Dbg);

  return (MQTT_SUCCESS==myMqttClient.subscribe(pc_Topic))?Ok:Error;
}


void MqttController::onTopicReceived(const char *pc_Topic, const char *pc_Content)
{
  if(mp_TopicReceivedCallback)
    mp_TopicReceivedCallback(pc_Topic, pc_Content);  
}
