#ifndef _CONTROLLER_WIFI_H_
#define _CONTROLLER_WIFI_H_

#include <inttypes.h>

#include "model/model_wifi_settings.h"


class WifiController;


class WifiState
{
public:
  typedef enum {Idle, Connecting, Connected, Error} EState;

  virtual void init(WifiController *p_Controller) = 0;
  virtual void loop(WifiController *p_Controller) = 0;
  virtual EState getState(void) = 0;
};


class WifiStateConnecting : public WifiState
{
  uint8_t mu8_CurrentNetworkIndex;
  uint8_t mu8_SameNetworkConnectRetries;
  uint32_t mu32_NextConnectionAttempt;
public:
  void init(WifiController *p_Controller);
  void loop(WifiController *p_Controller);
  EState getState(void);
};


class WifiStateConnected : public WifiState
{
public:
  void init(WifiController *p_Controller);
  void loop(WifiController *p_Controller);
  EState getState(void);
};


class WifiStateIdle : public WifiState
{
public:
  void init(WifiController *p_Controller);
  void loop(WifiController *p_Controller);
  EState getState(void);
};


class WifiStateError : public WifiState
{
public:
  void init(WifiController *p_Controller);
  void loop(WifiController *p_Controller);
  EState getState(void);
};




class WifiStateAction
{
  friend class WifiStateIdle;
  friend class WifiStateConnecting;
  friend class WifiStateConnected;
  friend class WifiStateError;

protected:
  virtual void idle(void) = 0;
  virtual void connecting(void) = 0;
  virtual void connected(void) = 0;
  virtual void disconnected(void) = 0;
  virtual void error(void) = 0;
};







class WifiController
{
  friend class WifiStateIdle;
  friend class WifiStateConnecting;
  friend class WifiStateConnected;
  friend class WifiStateError;

  WifiStateIdle       m_StateIdle;
  WifiStateConnecting m_StateConnecting;
  WifiStateConnected  m_StateConnected;
  WifiStateError      m_StateError;

  WifiState *mp_CurrentState;

  WifiSettings *mp_Settings;
  WifiStateAction *mp_StateAction;

  char mac_CurrentSSID[MAX_SSID_LENGTH+1];

public:
  WifiController(WifiSettings *p_Settings = nullptr, WifiStateAction *p_StateAction = nullptr);
  void setSettings(WifiSettings *p_Settings);
  WifiSettings *getSettings(void);
  void setStateAction(WifiStateAction *p_StateAction);
  WifiStateAction *getStateAction(void);

  void setup(void);
  void loop(void);
  void begin(void);  
  void end(void);

  WifiState::EState getState(void);
  const char *getCurrentSSID(void);
  IPAddress getLocalIp(void);

private:
  void setState(WifiState::EState e_NewState);
  void setCurrentSSID(const char *pc_SSID);
};




#endif
