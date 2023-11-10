#include <Arduino.h>

#include "controller/controller.h"

#include "view_shell_cmds.h"

#include "debug.h"


extern ControllerFacade myController;




int ViewShellCommands::cmdHelp(int argc, char *argv[])
{
  Serial.println("Commands:");
  Serial.println("info, settings, set, wifi, debuglevel");
  return 0;
}


int ViewShellCommands::cmdInfo(int argc, char *argv[])
{
  if(argc==2)
  {
    Serial.println("Usage:");
    Serial.println("  info");
  }
  else 
  {
    Serial.println("Settings:");
    myController.printSettings();
    Serial.println();
  }

  return 0;
}



int ViewShellCommands::cmdDebug(int argc, char *argv[])
{
  if(argc==2)
  {
    if(!strcmp("trace", argv[1]))
    {
      return myController.setDbgMinSevLevel(Debug::Trace);
    }
    else if(!strcmp("info", argv[1]))
    {
      return myController.setDbgMinSevLevel(Debug::Info);
    }
    else if(!strcmp("warning", argv[1]))
    {
      return myController.setDbgMinSevLevel(Debug::Warning);
    }
    else if(!strcmp("error", argv[1]))
    {
      return myController.setDbgMinSevLevel(Debug::Error);
    }
    else if(!strcmp("none", argv[1]))
    {
      return myController.setDbgMinSevLevel(Debug::None);
    }
  } 

  Serial.println("Usage:");
  Serial.println("  debuglevel trace");
  Serial.println("  debuglevel info");
  Serial.println("  debuglevel warning");
  Serial.println("  debuglevel error");
  Serial.println("  debuglevel none");
  return 0;  
}



int ViewShellCommands::cmdSettings(int argc, char *argv[])
{
  if(argc==2 && !strcmp("save", argv[1]))
  {
    myController.saveSettings();
    return 0;  
  } 
  else if(argc==2 && !strcmp("clear", argv[1]))
  {
    myController.clearSettings();
    return 0;
  } 
  else if(argc==2 && !strcmp("print", argv[1]))
  {
    myController.printSettings();
    return 0;
  }   

  Serial.println("Usage:");
  Serial.println("  settings save");
  Serial.println("  settings clear");
  Serial.println("  settings print");
  return 0;  
}


int ViewShellCommands::cmdSet(int argc, char *argv[])
{
  if(argc==4 && !strcmp("wifi", argv[1]))
  {
    myController.setWifiSettings(argv[2], argv[3]);
    return 0;
  } 
  else if((argc==3 || argc==4) && !strcmp("mqtt", argv[1]))
  {
    if(argc==4)
      myController.setMqttSettings(argv[2], atoi(argv[3]));
    else
      myController.setMqttSettings(argv[2]);
    return 0;
  } 

  Serial.println("Usage:");
  Serial.println("  set wifi <SSID> <key>");
  Serial.println("  set mqtt <addr> [<port>]");
  return 0;  
}




int ViewShellCommands::cmdWifi(int argc, char *argv[])
{
  if(argc==2 && !strcmp("status", argv[1]))
  {
    myController.printWifiStatus();
    return 0;
  } 
  else if((argc==2 || argc==4) && !strcmp("connect", argv[1]))
  {
    if(argc==4)
      myController.connectWifi(argv[2], argv[3]);
    else
      myController.connectWifi();
    return 0;
  } 
  else if(argc==2 && !strcmp("disconnect", argv[1]))
  {
    myController.disconnectWifi();
    return 0;
  } 
  
  Serial.println("Usage:");
  Serial.println("  wifi status");
  Serial.println("  wifi connect [<ssid> <key>]");
  Serial.println("  wifi disconnect");  
  return 0;
}


int ViewShellCommands::cmdMqtt(int argc, char *argv[])
{
  if(argc==2 && !strcmp("status", argv[1]))
  {
    myController.printMqttStatus();
    return 0;
  } 
  else if(argc==2 && !strcmp("connect", argv[1]))
  {
    myController.connectMqtt();
    return 0;
  } 
  else if(argc==2 && !strcmp("disconnect", argv[1]))
  {
    myController.disconnectMqtt();
    return 0;
  } 
  
  Serial.println("Usage:");
  Serial.println("  mqtt status");
  Serial.println("  mqtt connect");
  Serial.println("  mqtt disconnect");  
  return 0;
}
