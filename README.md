# Introduction

This IoT skeleton, based on the Arduino Framework, leverages the [PlatformIO](http://platformio.org) cross-platform build system. It facilitates IoT network connectivity and includes functionality to illuminate a custom user LED through MQTT commands and provide the measured voltage on an analog pin as a usage example.

The project is compatible with and has been tested on various ESP8266 and ESP32-based controller boards. It is easily adaptable to many other controller boards, provided they support the Arduino/PlatformIO Framework and have WiFi capabilities.

## Intended Use

This project serves as a robust foundation for creating Arduino-based Internet of Things (IoT) applications.

## Features

 * *Configurable:* Easily configure the WIFI network and MQTT broker required for IoT functionality via the serial interface without recompiling and deploying the source code.
 * *Error Tolerant:* The device autonomously reconnects to a WIFI network or MQTT broker after a loss of connection without requiring a device reboot.
 * *CRC Data Protection:* Settings stored in the device's flash memory are protected by a 32-bit CRC.
 * *MVC Architecture:* The software is structured into the _model_ containing data used by the controller, the _view_ providing the text-based user interface (shell commands), and the _controller_, which manages functionality and integrates modules.
 * *Few Module Dependencies:* Software modules exhibit high independence from each other.  

## Short IoT introduction

The Internet of Things (IoT) refers to devices equipped with sensors or actuators capable of connecting to other devices over the internet. Each IoT device connects to an _MQTT broker_, an internet server responsible for collecting and forwarding messages to other devices.

![IoT overview](doc/iot_overview.png)

An example MQTT server which can be used for test purposes is [broker.hivemq.com](broker.hivemq.com) with the default TCP port 1883. A web interface for testing is available here: [MQTT browser client](https://www.hivemq.com/demos/websocket-client/).

### Providing information

An IoT device can send messages to a specific _topic_ to the broker, which then distributes the information. For instance, an IoT thermometer can provide measured temperature and humidity to the world.

### Receiving information

An IoT device can register topics of interest to receive relevant messages. The MQTT broker forwards messages with these topics from other devices to the subscribed device.


# Hardware

A configuration exists for the following boards: 

## nodemcu
![ESP8266 NodeMCU Lua Amica Modul V2](doc/NodeMCU_Amica_250x125.png)

Az-Delivery: [ESP8266 NodeMCU Lua Amica Modul V2](https://www.az-delivery.de/products/nodemcu)

## d1_mini
![ESP8266 D1 mini NodeMCU](doc/D1_mini_168x125.png)

Az-Delivery: [ESP8266 D1 mini NodeMCU](https://www.az-delivery.de/products/d1-mini)

## esp32doit-devkit
![ESP32 NodeMCU Dev Kit C V2](doc/ESP32-WROOM-32_275x138.png)

Az-Delivery: [ESP32 NodeMCU Dev Kit C V2](https://www.az-delivery.de/products/esp32-developmentboard)

## lolin32
![LOLIN32 Lite V1.0.0](doc/Lolin32_250x125.png)

Az-Delivery: [LOLIN32 Lite V1.0.0](https://www.az-delivery.de/products/esp32-lolin-lolin32)

## Comparison and Pin-out

 |     | nodemcu | d1_mini | esp32doit-devkit | lolin32 |
 |-----|---------|---------|---------------------|---------|
 | size | 50 x 26 mm | 34 x 26 mm | 56 x 28 mm<br>(too large for<br>breadboard) | 50 x 26 mm |
 | CPU | ESP8266-12E | ESP8266-12F | ESP32 | ESP32 rev1 |
 | Cores | 1 | 1 | 2 | 2 |
 | max Speed | 80 MHz | 80 MHz | 240 MHz | 240 MHz |
 | LED WIFI | build-in,<br>D2 (inverted) | build-in,<br>D2 (inverted) | G22 | build-in,<br>22 (inverted) |
 | LED MQTT | build-in,<br>D16 (inverted) | D16 | G16 | 16 |
 | LED User1 | D5 | D5 | G5 | 5 |
 | Analog in | A0 | A0 | G32 | 32 |

The project could be easily ported to many other controller boards, as long as the Arduino/PlatformIO Framework is used and the controller is able to access WIFI.


# User Interface

To utilize the IoT device within a custom environment, essential configurations include setting up the WIFI network and specifying the MQTT broker need to be performed.

To initiate this setup, follow these steps:

1. *Driver Installation:*<br>
   Ensure the installation of the CP210x Universal Windows Driver on your computer. This driver is pivotal for establishing communication between the device and your computer via USB.

2. *Physical Connection:*<br>
   Connect the IoT device to your computer using a USB connection.

3. *Terminal Program Setup:*<br>
   Utilize a terminal program, such as HTerm, to facilitate configuration. Configure the terminal program with the following settings:
   * Baud Rate: 115200
   * Data Format: 8N1 (8 data bits, no parity bit, 1 stop bit)
   * Flow Control: None
   * Line Ending for Reception: \r\n
   * Command Termination: \r

## Setting up the WIFI connection 

To configure the WIFI network, use the follwing command:
```
set wifi <SSID> <password>
```
To connect to the configured network(s), use the following command:

```
wifi connect
```

Up to four networks can be configured. When a WIFI connection is requested, the device attempts to connect to the configured networks until a successful connection is established. 


## Setting up the MQTT connection

To configure the MQTT broker connection, use the follwing command:
```
set mqtt <broker>
```
To connect to the configured broker, use the following command:

```
mqtt connect
```

## Saving the configuration

The settings are not stored automatically. To persistently store the settings in flash, use the following command:

```
settings save
```


# MQTT message interface

MQTT operates on a publish/subscribe model, wherein messages are the central components, comprising a topic (for data categorization) and a payload (housing the actual information).

Upon establishing an MQTT connection to the broker, the initial message is sent, containing the device's UID under the topic _"iotdevice"._

Example: 
```
{"uid":30973702185336}
```

In routine operation, the device measures the voltage on the analog input pin and transmits it, along with the device's UID, under the same _"iotdevice"_ topic every 10 seconds. 

Example:
```
{"uid":30973702185336,"analog":1.379633665}
```

To receive commands, the device subscribes to the _"iotdevice/30973702185336/command"_ topic on the broker. For demonstration purposes, the device supports the _"setled"_ operation, allowing users to switch on the user LED with the following message:
```
{"operation":"setled","number":0,"value":1}
```



# Software Architecture

The overarching software architecture adheres to the Model–View–Controller (MVC) design pattern.

> Model–view–controller (MVC) is a software design pattern commonly used for 
> developing user interfaces that divides the related program logic into 
> three interconnected elements. These elements are the internal representations 
> of information (the Model), the interface (the View) that presents information 
> to and accepts it from the user, and the Controller software linking the two.
> --- [Model-view-controller. (2023, November 20). In Wikipedia, The Free Encyclopedia. Retrieved 12:56, November 20, 2023](https://en.wikipedia.org/w/index.php?title=Model-view-controller)

In this context, the _Model_ is responsible for holding and storing configuration data in the internal flash memory of the ESP8266/ESP32 module using the Arduino framework.

The _View_ manages the user interface, encompassing both the text-based command interface on the RS232/USB port and the MQTT command interface. The View features a viewFacade, the sole class accessed by the controller. The viewFacade, in turn, utilizes the Shell library, providing shell commands and MQTT commands.

The _Controller_ offers a controllerFacade, the exclusive class accessed from the main function or the view. The controllerFacade initializes and utilizes the WIFI controller, MQTT controller, and the hardware controller, abstracting access to the actual hardware via the Arduino framework.

![Component diagram](doc/component_diagram.png)


## Model

Within the _Model_, the pivotal component is the Settings class, serving as the facade for all storable settings in the device. The separation of WIFI and MQTT settings into distinct classes enhances modularity and clarity. These classes facilitate configuration adjustments by the WIFI and MQTT controllers, each offering serialization and deserialization methods. This design ensures the Settings class can manage settings without delving into the internal structure of each.

![Settings Class Diagram](doc/class_diagram_settings.png)

### Settings

The Settings class encompasses both WIFI and MQTT settings, providing methods for saving, loading, and clearing settings using the Arduino framework. Additionally, it supplies pointers to the WIFI and MQTT settings classes, directly utilized by the corresponding controllers.

### WIFI settings

The WIFI settings class serves as the storage unit for the WIFI controller, accommodating up to four WIFI networks with SSID and password. The priority of networks is determined by their index, with the most recently added having the highest priority. If more than four settings are added, the earliest networks are discarded.   

### MQTT settings

For the MQTT controller, the MQTT settings class serves as the storage entity, allowing configuration of the broker and port number (defaulting to 1883). 

### Data storage in flash

All settings are stored in flash, commencing with a magic number (0x1ACFFC1D) and a data version number. The WIFI settings, serialized by the WIFI settings class, precede the MQTT settings. The whole data settings section is protected by a 4-byte CRC. This approach ensures secure and organized storage of crucial device configurations.

![Settings in Flash](doc/map_flash_settings.png)


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


# Bootup process

![Bootup activity diagram](doc/activity_diagram_bootup.png)




# SOUP

The following 3rd party software components are part of the excutable and are handeled as a SOUP (software of unknown provenance):

| Type | Software | Version | Manufacturer/Maintainer |
| ---- | -------- | ------- | ------------------------| 
| Platform (ESP8266 only) | espressif8266 | 4.2.0 | Espressif systems |
| Platform (ESP32 only) | espressif32 | 6.3.1 | Espressif systems |
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

