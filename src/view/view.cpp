#include <Arduino.h>
#include <shell.h>   // see https://github.com/steftri/shell


#include "view.h"
#include "view_shell_cmds.h"



void ViewFacade::showPrompt(void)
{
  Serial.print("> ");
  return;
}



void ViewFacade::cmdNotFound(char *pc_Cmd)
{
  Serial.print(pc_Cmd); 
  Serial.println(": command not found (try 'help' for command list)");
  return;
}


void ViewFacade::cmdError(char *pc_Cmd, int rc)
{
  Serial.print(pc_Cmd);
  Serial.print(" failed returncode ");
  Serial.print(rc);
  Serial.println();  
}



void ViewFacade::setup(void)
{
  m_Shell.setPromptCallback(&showPrompt);
  m_Shell.setCommandNotFoundCallback(&cmdNotFound);
  m_Shell.setCommandErrorCallback(&cmdError);

  m_Shell.addCommandCallback("help", &ViewShellCommands::cmdHelp);
  m_Shell.addCommandCallback("info", &ViewShellCommands::cmdInfo);
  m_Shell.addCommandCallback("settings", &ViewShellCommands::cmdSettings);
  m_Shell.addCommandCallback("set", &ViewShellCommands::cmdSet);
  m_Shell.addCommandCallback("wifi", &ViewShellCommands::cmdWifi);
  m_Shell.addCommandCallback("debuglevel", &ViewShellCommands::cmdDebug);

  m_Shell.begin(); 
}



void ViewFacade::loop(void)
{
  char c_Char;

  if(Serial.available())
  {
    c_Char = Serial.read();
    if(c_Char=='\r')
      Serial.println();
    else
      Serial.write(c_Char);
    
    m_Shell.putChar(c_Char);
  }  
}

