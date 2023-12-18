#define PROJECT_NAME "iot-arduino integration tests"

#include <unity.h>
#include <Arduino.h>
#include <SpinTimer.h>      // see https://github.com/dniklaus/spin-timer





#include "controller_wifi.h"
#include "model_wifi_settings.h"

#include "controller_mqtt.h"
#include "model_mqtt_settings.h"

#define TEST_NETWORK_SSID     // WIFI network
#define TEST_NETWORK_PASSWORD // WIFI password 

#define TEST_MQTT_BROKER      "broker.hivemq.com"
#define TEST_MQTT_TOPIC       "iotdevice/integration"
#define TEST_MQTT_MESSAGE     "Hello World!"



void setUp(void) 
{
  // set stuff up here
}

void tearDown(void) 
{
  // clean stuff up here
}


bool gb_WifiConnectedCallback = false;

class MyWifiAction : public WifiAction
{
  void idle(void) override {};
  void connecting(void) override {};
  void connected(void) override { gb_WifiConnectedCallback = true; };
  void disconnected(void) override {};
  void error(void) override {};
};



void test_wifi_integration(void) 
{
  WifiController  myWifiController;
  WifiSettings    myWifiSettings;
  MyWifiAction    myWifiAction;

  // test step 1: check initialization of settings class
  TEST_ASSERT_EQUAL_UINT8(0, myWifiSettings.getNetworkCount());

  myWifiController.setSettings(&myWifiSettings);
  myWifiController.setAction(&myWifiAction);

  // test step 2: check set wifi settings 
  myWifiSettings.setNetwork(TEST_NETWORK_SSID, TEST_NETWORK_PASSWORD);
  TEST_ASSERT_EQUAL_UINT8(1, myWifiSettings.getNetworkCount());

  // test step 3: check wifi init state (idle)
  TEST_ASSERT_EQUAL(WifiState::Idle, myWifiController.getState());

  // test step 4: check connect
  myWifiController.begin();
  TEST_ASSERT_EQUAL(WifiState::Connecting, myWifiController.getState());
  for(uint32_t i=0; myWifiController.getState()==WifiState::Connecting && i<10; i++)
  {
    delay(1000);
    myWifiController.loop();
  }

  // test step 5: check connection state and call of connected action method
  TEST_ASSERT_EQUAL(WifiState::Connected, myWifiController.getState());
  TEST_ASSERT_TRUE(gb_WifiConnectedCallback);

  // test step 6: check termination of connection
  myWifiController.end();
  TEST_ASSERT_EQUAL(WifiState::Idle, myWifiController.getState());
}



bool gb_MqttConnectedCallback = false;

class MyMqttAction : public MqttAction
{
  void idle(void) override {};
  void connecting(void) override {};
  void connected(void) override { gb_MqttConnectedCallback = true; };
  void disconnected(void) override {};
  void error(void) override {};
};


bool gb_MqttTopicReceivedCallback = false;

void TopicReceivedCallback(const char *pc_Topic, const char *pc_Content)
{
  gb_MqttTopicReceivedCallback = true;
}

void test_mqtt_integration(void) 
{
  WifiController  myWifiController;
  WifiSettings    myWifiSettings;

  // wifi is not focus of this test, but it is required for the MQTT broker connection 
  myWifiSettings.setNetwork(TEST_NETWORK_SSID, TEST_NETWORK_PASSWORD);
  myWifiController.setSettings(&myWifiSettings);
  myWifiController.begin();
  for(uint32_t i=0; myWifiController.getState()==WifiState::Connecting && i<100; i++)
  {
    delay(100);
    myWifiController.loop();
  }
  TEST_ASSERT_EQUAL(WifiState::Connected, myWifiController.getState());

  // here the real test starts...
  MqttController  myMqttController;
  MqttSettings    myMqttSettings;
  MyMqttAction    myMqttAction;

  myMqttController.setSettings(&myMqttSettings);
  myMqttController.setAction(&myMqttAction);
  myMqttController.setTopicReceivedCallback(&TopicReceivedCallback);

  // test step 1: check set and get wifi settings 
  myMqttSettings.setBroker(TEST_MQTT_BROKER);
  TEST_ASSERT_EQUAL_STRING(TEST_MQTT_BROKER, myMqttSettings.getBrokerAddr());

  // test step 2: check wifi init state (idle)
  TEST_ASSERT_EQUAL(MqttState::Idle, myMqttController.getState());

  // test step 4: check connect
  myMqttController.begin();
  TEST_ASSERT_EQUAL(MqttState::Connecting, myMqttController.getState());
  for(uint32_t i=0; myMqttController.getState()==MqttState::Connecting && i<10; i++)
  {
    delay(1000);
    myMqttController.loop();
  }

  // test step 5: check connection state and call of connected action method
  TEST_ASSERT_EQUAL(MqttState::Connected, myMqttController.getState());
  TEST_ASSERT_TRUE(gb_MqttConnectedCallback);

  // test step 6: check registering topic
  TEST_ASSERT_EQUAL(MqttController::Ok, myMqttController.registerTopic(TEST_MQTT_TOPIC));

  // test step 7: check sending and receiving topics
  TEST_ASSERT_EQUAL(MqttController::Ok, myMqttController.publish(TEST_MQTT_TOPIC, TEST_MQTT_MESSAGE, 1));
  delay(1000);
  TEST_ASSERT_TRUE(gb_MqttTopicReceivedCallback);

  // test step 8: check termination of connection
  myMqttController.end();
  TEST_ASSERT_EQUAL(MqttState::Idle, myMqttController.getState());

  // clean up wifi
  myWifiController.end();
}




void setup()
{
  delay(2000); // service delay  
  UNITY_BEGIN();

  RUN_TEST(test_wifi_integration);
  RUN_TEST(test_mqtt_integration);

  UNITY_END(); // stop unit testing
}



void loop()
{
}
