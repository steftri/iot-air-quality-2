#ifndef _CONTROLLER_WIFI_ACTION_H_
#define _CONTROLLER_WIFI_ACTION_H_

#include "controller_wifi.h"

class MyWifiAction : public WifiAction
{
  void idle(void) override;
  void connecting(void) override;
  void connected(void) override;
  void disconnected(void) override;
  void error(void) override;
};

#endif