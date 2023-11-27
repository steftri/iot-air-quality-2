# Introduction
Arduino Framework based IoT skeleton, using the [PlatformIO](http://platformio.org) cross-platform build system.

The project is able to run and has been tested on the [ESP32 NODEMCU](https://www.az-delivery.de/en/products/esp32-developmentboard) controller board. It could be easily ported to many other controller boards, as long as the Arduino/PlatformIO Framework is used and the controller is able to access WIFI.

## Intended Use
With this project you have a good starting point to create your own Arduino basedd internet-of-things application. 

## Features

 * *Configurable*: The WIFI network and the MQTT broker needed for the IoT functionality can be configured via the serial interface without the need of re-compiling and deploying the source code.
 * *Error tolerant*: The device is able to reconnect to a WIFI network or to the MQTT broker after a loss of connection occured without the need to reboot the device.
 * *CRC data protection*: The settings stored in the flash of the device are protected by a 32bit CRC.  
 * *MVC architecture*: The software is seperated in to the _model_ with the data used by the controller, the _view_ providing the text-based user interface (shell commands) and the _controller_, which holds the modules responsible for handling the functionality and to glue everything together.
 * *Few module dependencies*: The software modules are highly independent from each other.    

## Short IoT introduction

The Internet of things (IoT) describes devices with sensors or actuators which are able to connect to other devices over the internet. For that purpose, each IoT device connects to an _MQTT broker_ -- an internet server which is responsible to collect and forward messages to other devices. 

![IoT overview](doc/iot_overview.png)

An example MQTT server which can be used for test purposes is [broker.hivemq.com], with the TCP port 1883 (this is also the default port). A web interface for test purposes is also available here: [MQTT browser client](https://www.hivemq.com/demos/websocket-client/).

### Providing information
An IoT device can send messages to a specific _topic_ to the broker. The broker than distributes the information further. For example, a IoT thermometer can provide the messured temperature and humidity to the world. 

### Receiving information
An IoT device can register _topics_ which it is interested to receive. The MQTT broker then sends each message with this topic, which it receives from other devices, also to this device. 


# User interface

For using the IoT device in an own environment, at least the WIFI network and the MQTT broker which is to be used must be configured. 



## set wifi <SSID> <password>

## set mqtt <broker> [<port>]

## settings save

## wifi connect


# MQTT message interface

UID = 30973702185336

Topic: iotdevice/30973702185336/command
Message: 
```
{"operation":"setled","number":0,"value":1}
```


Topic: iotdevice
```
{"uid":30973702185336,"analog":1.379633665}
```

# Electrical interfaces

* WIFI-LED: GPIO 2 
* MQTT-LED: GPIO 16
* USER-LED: GPIO 17
* Analog in: GPIO 32 (ADC1_CH4)


# Bootup process

![Bootup activity diagram](doc/activity_diagram_bootup.png)


# Software Architecture

The overal software architecture is the MVC architecture.

> Model–view–controller (MVC) is a software design pattern commonly used for 
> developing user interfaces that divides the related program logic into 
> three interconnected elements. These elements are the internal representations 
> of information (the Model), the interface (the View) that presents information 
> to and accepts it from the user, and the Controller software linking the two.
> --- [Model-view-controller. (2023, November 20). In Wikipedia, The Free Encyclopedia. Retrieved 12:56, November 20, 2023](https://en.wikipedia.org/w/index.php?title=Model-view-controller)

In this case, the _Model_ is responsible to hold and to store the configuration data in the internal flash memory of the ESP32 module. 

The _View_ is responsible for handling the 


![Component diagram](doc/component_diagram.png)

## Model

### settings

### WIFI settings

### MQTT settings

## View

### view facade

### Shell commands


## Controller

### controller facade

### WIFI controller

![WIFI class diagram](doc/class_diagram_controller_wifi.png)
![WIFI state diagram](doc/state_diagram_controller_wifi.png)

#### WIFI actions

### MQTT controller

![MQTT class diagram](doc/class_diagram_controller_mqtt.png)
![MQTT state diagram](doc/state_diagram_controller_mqtt.png)

#### MQTT actions





# SOUP

The following 3rd party software components are part of the excutable and are handeled as a SOUP (software of unknown provenance):

| Type | Software | Version | Manufacturer/Maintainer |
| ---- | -------- | ------- | ------------------------| 
| Platform | espressif32 | 6.3.1 | Espressif systems |
| Package | framework-arduinoespressif32 | 3.20009.0 (2.0.9) | Espressif systems |
| Library | CRC | 1.0.2 | Rob Tillaart |
| Library | ArduinoMqttClient | 0.1.7 | Alexander Entinger |
| Library | ArduinoJson | 6.21.2 | Benoit Blanchon |
| Library | spin-timer | 3.0.0 | Dieter Niklaus |
| Library | Shell | 1.2.0 | Stefan Trippler |

# Development tools

The following tools and drivers are used for development:

| Type | Software | Version | Manufacturer/Maintainer |
| ---- | -------- | ------- | ------------------------| 
| OS | Windows 11 | latest | Microsoft |
| Driver | CP210x Universal Windws Driver | 11.2.0 | Silicon Labs | 
| Interpreter | Python | 3.10 | Python Software Foundation |
| IDE  | Visual Studio Code | 1.77.3 | Microsoft |
| IDE Extension | C/C++ | 1.18.3 | Microsoft |
| IDE Extension | PlatformIO IDE | 3.1.1 | PlatformIO |
| IDE Extension | PlantUML | 2.17.5 | yebbs |
| Toolchain | toolchain-xtensa-esp32 | 8.4.0+2021r2-patch5 | Espressif systems |
| Toolchain | tool-esptoolpy | 1.40501.0 (4.5.1) | PlatformIO |
| RCS | git | 2.40.0 | Junio Hamano |
| RCS | Git Extensions | 4.0.2 | Henk Mesthuis |
| Merge Tool | P4Merge | 2023.1/2419860 | Perforce |


# Additional information

## Create a new project based on this IoT skeleton application

TODO

To use *wiring-skeleton* as a template for a new project, it has to be forked locally.

1. On **GitHub:** create new repository, i.e. *my-test*
2. Within a **Git Bash:**
   1. Clone the *wiring-skeleton* as a **bare repository**:
   ```bash
      git clone --bare git@github.com:ERNICommunity/wiring-skeleton.git
   ```
   2. Replace origin with the one for your new project (i.e. project *my-test*, with *your-name* as GitHub user name):
   ```bash
      cd ./wiring-skeleton.git
      git remote rm origin
      git remote add origin git@github.com:your-name/my-test.git
   ```
   3. Push the bare repo as a **mirror** to your new origin:
   ```bash
      git push --mirror
   ```
   4. Clone the new project (i.e. project *my-test*, with *your-name* as GitHub user name):
   ```bash
      cd ..
      git clone git@github.com:your-name/my-test.git
   ```
   5. Remove the bare *wiring-skeleton* template project:
   ```bash
      rm -rf ./wiring-skeleton.git
   ```

