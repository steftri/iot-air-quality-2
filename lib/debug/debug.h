#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <Arduino.h>

class Debug
{
public:
  typedef enum {Trace, Info, Warning, Error, FatalError, None} ESeverity;
  
private:
  ESeverity me_MinSeverityLevel;

public:
  Debug(ESeverity e_MinSeverityLevel = Info);

  void setMinSeverityLevel(ESeverity e_MinSeverityLevel);

  void println(ESeverity e_Severity, const char *pc_String);
  void println(ESeverity e_Severity, const uint32_t u32_Value);

private:
  void _printSevPrefix(ESeverity e_Severity);
};


extern Debug debug;

/*
Experimental:
#define DEBUG_TRACE(x)   debug.println(__FILENAME__, __LINE__, Debug::Trace, x)
#define DEBUG_INFO(x)    debug.println(__FILENAME__, __LINE__, Debug::Info, x)
#define DEBUG_WARNING(x) debug.println(__FILENAME__, __LINE__, Debug::Warning, x)
#define DEBUG_ERROR(x)   debug.println(__FILENAME__, __LINE__, Debug::Error, x)
*/

#endif