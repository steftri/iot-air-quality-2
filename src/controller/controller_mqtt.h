#ifndef _CONTROLLER_MQTT_H_
#define _CONTROLLER_MQTT_H_

#include <inttypes.h>

#include "model/model_mqtt_settings.h"


#define MAX_MQTT_MESSAGE        256
#define MAX_MQTT_TOPIC           64


class MqttController;




class MqttState
{
public:
  typedef enum {Idle, Connecting, Connected, Error} EState;

  virtual void init(MqttController *p_Controller = nullptr) = 0;
  virtual void loop(MqttController *p_Controller = nullptr) = 0;
  virtual EState getState(void) = 0;
};


class MqttStateIdle : public MqttState
{
public:
  void init(MqttController *p_Controller = nullptr) override;
  void loop(MqttController *p_Controller = nullptr) override;
  EState getState(void) override;
};


class MqttStateConnecting : public MqttState
{
  uint32_t mu32_NextConnectionAttempt;
  uint16_t mu16_ConnectionAttempts;
public:
  void init(MqttController *p_Controller = nullptr) override;
  void loop(MqttController *p_Controller = nullptr) override;
  EState getState(void) override;
};


class MqttStateConnected : public MqttState
{
public:
  void init(MqttController *p_Controller = nullptr) override;
  void loop(MqttController *p_Controller = nullptr) override;
  EState getState(void) override;

private:
  static void onMessage(int MsgSize);   
};


class MqttStateError : public MqttState
{
public:
  void init(MqttController *p_Controller = nullptr) override;
  void loop(MqttController *p_Controller = nullptr) override;
  EState getState(void) override;
};




class MqttStateAction
{
  friend class MqttStateIdle;
  friend class MqttStateConnecting;
  friend class MqttStateConnected;
  friend class MqttStateError;

protected:
  virtual void idle(void) = 0;
  virtual void connecting(void) = 0;
  virtual void connected(void) = 0;
  virtual void disconnected(void) = 0;  
  virtual void error(void) = 0;
};



class MqttController
{
  friend class MqttStateIdle;
  friend class MqttStateConnecting;
  friend class MqttStateConnected;
  friend class MqttStateError;

public: 
  typedef enum 
  {
    Ok = 0,
    Error = -1
  } ERc;

  typedef void (TTopicReceivedCallback)(const char *pc_Topic, const char *pc_Content);

private:
  MqttSettings *mp_Settings;

  MqttStateIdle       m_StateIdle;
  MqttStateConnecting m_StateConnecting;
  MqttStateConnected  m_StateConnected;
  MqttStateError      m_StateError;

  MqttState *mp_CurrentState;
  MqttStateAction *mp_StateAction;

  static TTopicReceivedCallback *mp_TopicReceivedCallback;

public:
  explicit MqttController(MqttSettings *p_Settings = nullptr, MqttStateAction *p_StateAction = nullptr);
  void setSettings(MqttSettings *p_Settings);
  MqttSettings *getSettings(void);
  void setStateAction(MqttStateAction *p_StateAction);
  MqttStateAction *getStateAction(void);  
  void setTopicReceivedCallback(TTopicReceivedCallback *p_TopicReceivedCallback);

  void setup(void);
  void loop(void);

  void begin(void);
  void end(void);
  MqttState::EState getState(void);

  ERc publish(const char *pc_Topic, const char *pc_Content, const uint8_t u8_QoS = 0, const bool b_Retain = false);
  ERc registerTopic(const char *pc_Topic);

protected:  
  static void onTopicReceived(const char *pc_Topic, const char *pc_Content);

private:
  void setState(MqttState::EState e_NewState);
};


#endif

