/*
 * MyBuiltinLedIndicatorAdapter.h
 *
 *  Created on: 04.11.2019
 *      Author: nid
 */

#ifndef LIB_ADAPTER_LEDINDICATORADAPTER_H_
#define LIB_ADAPTER_LEDINDICATORADAPTER_H_

#include <inttypes.h>

#include "Indicator.h"


class LedIndicatorAdapter: public AIndicatorAdapter
{
  uint16_t mu16_LedPin;
  bool mb_Inverted; 

public:
  explicit LedIndicatorAdapter(const uint16_t u16_LedPin, const bool b_Inverted = false);
  virtual ~LedIndicatorAdapter();
  void notifyStatusChange(bool status) override;

private:
  void setLed(bool isOn);

private:  // forbidden default functions
  LedIndicatorAdapter(const LedIndicatorAdapter &other) = delete;
  LedIndicatorAdapter(LedIndicatorAdapter &&other) = delete;
};

#endif /* LIB_ADAPTER_LEDINDICATORADAPTER_H_ */
