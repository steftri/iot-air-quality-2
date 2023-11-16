#include <Arduino.h>
#include <ArduinoJson.h>

#include "controller.h"

#include "LedIndicatorAdapter.h"


#define LED_MQTT    16    
   

#define MQTT_IOT_DEVICE_NAME "iotdevice"



LedIndicatorAdapter myMqttIndicatorAdapter(LED_MQTT);
Indicator myMqttIndicator(&myMqttIndicatorAdapter);


extern ControllerFacade myController;



void MyMqttAction::idle(void) 
{
  myMqttIndicator.clear();
}



void MyMqttAction::connecting(void) 
{
  myMqttIndicator.blink();
}



void MyMqttAction::connected(void) 
{
  uint64_t u64_uid; 
  char ac_Topic[48+1];

  StaticJsonDocument<32> jsonDoc;
  char ac_SerializedContent[32]; 

  u64_uid = myController.getUid();

  // register topic
  snprintf(ac_Topic, sizeof(ac_Topic), MQTT_IOT_DEVICE_NAME "/%llu/command", u64_uid);
  myController.registerMqttTopic(ac_Topic);
   
  // publish own UID
  jsonDoc["uid"] = u64_uid;
  serializeJson(jsonDoc, ac_SerializedContent, sizeof(ac_SerializedContent));
  myController.publishMqttMessage(MQTT_IOT_DEVICE_NAME, ac_SerializedContent, 1, true);

  myMqttIndicator.set();
}



void MyMqttAction::disconnected(void) 
{
  myMqttIndicator.clear();
}



void MyMqttAction::error(void) 
{
  myMqttIndicator.blink();
}  



void  MyMqttAction::publishMessurementData(float f32_AnalogueValue)
{
  StaticJsonDocument<64> jsonDoc;
  char ac_SerializedContent[64];   

  // publish analog value together with the device UID
  jsonDoc["uid"] = myController.getUid();
  jsonDoc["analog"] = f32_AnalogueValue;
  serializeJson(jsonDoc, ac_SerializedContent, sizeof(ac_SerializedContent));
  myController.publishMqttMessage(MQTT_IOT_DEVICE_NAME, ac_SerializedContent);
}
