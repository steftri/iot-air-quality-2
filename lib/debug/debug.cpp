#include <Arduino.h>

#include "debug.h"
 
 
Debug::Debug(ESeverity e_MinSeverityLevel)
  : me_MinSeverityLevel{e_MinSeverityLevel}
{
}



void Debug::setMinSeverityLevel(ESeverity e_MinSeverityLevel)
{
  me_MinSeverityLevel = e_MinSeverityLevel;
}



void Debug::_printSevPrefix(ESeverity e_Severity)
{
  switch(e_Severity)
  {
    case Trace: 
      Serial.print("TRC: ");
      break;
    case Info: 
      Serial.print("INF: ");
      break;
    case Warning: 
      Serial.print("WRN: ");
      break;
    case Error: 
      Serial.print("ERR: ");
      break;
    case FatalError: 
      Serial.print("FATAL ERROR: ");
      break;
    default:
      break;
  }  
}


void Debug::println(ESeverity e_Severity, const char *pc_String)
{
  if(e_Severity>=me_MinSeverityLevel)
  {
    _printSevPrefix(e_Severity);
    Serial.println(pc_String);
  }
}



void Debug::println(ESeverity e_Severity, const uint32_t u32_Value)
{
  if(e_Severity>=me_MinSeverityLevel)
  {
    _printSevPrefix(e_Severity);
    Serial.println(u32_Value);
  }
}


Debug debug;
