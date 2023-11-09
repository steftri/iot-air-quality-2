#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_



#include "debug.h"

#include "model/settings.h"

#include "controller/controller_wifi.h"

#include "view/view.h"



class MyWifiStateAction : public WifiStateAction
{
  void idle(void) override;
  void connecting(void) override;
  void connected(void) override;
  void disconnected(void) override;
  void error(void) override;
};





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

  MyWifiStateAction    m_WifiStateAction;
  WifiController       m_WifiController; 


public:
  ControllerFacade(ViewFacade *p_ViewFacade);

  ViewFacade *getViewFacade(void);

  void setup(void);
  void loop(void);

  ERc setDbgMinSevLevel(Debug::ESeverity e_MinSevLevel);

  ERc saveSettings(void);
  ERc clearSettings(void);
  void printSettings(void);

  void setWifiSettings(const char *pc_SSID, const char *pc_Passphrase); 
  void setTcpSettings(const uint16_t u16_Port);
  uint16_t getTcpPort(void);

  void connectWifi(const char *pc_SSID, const char *pc_Passphrase);
  void connectWifi(void);
  void disconnectWifi(void);
  String getCurrentIP(void);
  void printWifiStatus(void);
};


#endif