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



class WifiStateIdle : public WifiState
{
public:
  void init(WifiController *p_Controller) override;
  void loop(WifiController *p_Controller) override;
  EState getState(void) override;
};


class WifiStateConnecting : public WifiState
{
  uint8_t mu8_CurrentNetworkIndex;
  uint8_t mu8_SameNetworkConnectRetries;
  uint32_t mu32_NextConnectionAttempt;
public:
  void init(WifiController *p_Controller) override;
  void loop(WifiController *p_Controller) override;
  EState getState(void) override;
};


class WifiStateConnected : public WifiState
{
public:
  void init(WifiController *p_Controller) override;
  void loop(WifiController *p_Controller) override;
  EState getState(void) override;
};


class WifiStateError : public WifiState
{
public:
  void init(WifiController *p_Controller) override;
  void loop(WifiController *p_Controller) override;
  EState getState(void) override;
};




class WifiAction
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

  WifiSettings *mp_Settings;

  WifiStateIdle       m_StateIdle;
  WifiStateConnecting m_StateConnecting;
  WifiStateConnected  m_StateConnected;
  WifiStateError      m_StateError;

  WifiState *mp_CurrentState;
  WifiAction *mp_Action;

  char mac_CurrentSSID[MAX_SSID_LENGTH+1];

public:
  explicit WifiController(WifiSettings *p_Settings = nullptr, WifiAction *p_Action = nullptr);
  void setSettings(WifiSettings *p_Settings);
  WifiSettings *getSettings(void);
  void setAction(WifiAction *p_Action);
  WifiAction *getAction(void);

  void setup(void);
  void loop(void);

  void begin(void);  
  void end(void);
  WifiState::EState getState(void);
  const char *getCurrentSSID(void);
  const char *getLocalIp(void);

private:
  void setState(WifiState::EState e_NewState);
  void setCurrentSSID(const char *pc_SSID);
};




#endif
