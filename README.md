# Introduction
Arduino Framework based IoT skeleton, using the [PlatformIO](http://platformio.org) cross-platform build system.

The project is able to run and has been tested on the [DoIt ESP32 DEVKIT V1](https://de.aliexpress.com/item/32807028338.html) controller board. It could be easily ported to many other controller boards, as long as the Arduino Framework is used with PlatformIO.

## Intended Use
With this project you have a good starting point to create your own internet-of-things application that shall be based on the Arduino Framework. 

## Features

 * *Configurable*: The WIFI network and the MQTT broker needed for the IoT functionality can be configured via the serial interface without the need of re-compiling and deploying the source code.
 * *Error tolerant*: The device is able to reconnect to a MIFI network or to the MQTT broker after a loss of connection occured without the need to reboot the device.
 * *CRC data protection*: The settings stored in the flash of the device are protected by a 32bit CRC.  
 * *MVC architecture*: The software is seperated in to the _model_ with the data used by the controller, the _view_ providing the text-based user interface (shell commands) and the _controller_, which holds the modules responsible for handling the functionality and to glue everything together.  

## Short IoT introduction

The Internet of things (IoT) describes devices with sensors or actuators which are able to connect to other devices over the internet. For that purpose, each IoT device connects to an _MQTT broker_ -- an internet server which is responsible to collect and forward messages to other devices. 

### Providing information
An IoT device can send messages to a specific _topic_ to the broker. The broker than distributes the information further. For example, a IoT thermometer can provide the messured temperature and humidity to the world.  

### Receiving information
An IoT device can register _topics_ which it is interested to receive. The MQTT broker then sends each message with this topic, which it receives from other devices, also to this device. 



# User interface

For using the IoT device in an own environment, at least the WIFI network and the MQTT broker which is to be used must be configured. 

TODO

## set wifi <SSID> <password>

## set mqtt <broker> [<port>]

## settings save

## wifi connect





# Software Architecture

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

![Class diagram](doc/class_diagram_controller_wifi.png)
![State diagram](doc/state_diagram_controller_wifi.png)

#### WIFI actions

### MQTT controller

![Class diagram](doc/class_diagram_controller_mqtt.png)
![State diagram](doc/state_diagram_controller_mqtt.png)

#### MQTT actions


# SOUP

## espressif32
6.3.1

## ArduinoMqttClient
0.1.7

## ArduinoJson
6.21.2

## spin-timer
3.0.0

## Shell
1.2.0


# Development tools

## Visual Studio Code

## VSCode plugin C/C++

## VSCode plugin PlatformIO IDE

## VSCode plugin PlantUML




# Additional information

## Create a new project based on this IoT skeleton application

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

