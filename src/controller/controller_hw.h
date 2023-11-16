#ifndef _CONTROLLER_HW_H_
#define _CONTROLLER_HW_H_

#include <inttypes.h>

class HwController
{
public:  
  void setup(void);
  void loop(void);

  void reset(void);
  uint64_t getUid(void);

  void setLed(const uint8_t u8_Led, bool b_On);
  float readAnalogValue(void);
};

#endif