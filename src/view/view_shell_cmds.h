#ifndef _VIEW_SHELL_H_
#define _VIEW_SHELL_H_


class ViewShellCommands
{
public:
  static int cmdHelp(int argc, char *argv[]);
  static int cmdDebug(int argc, char *argv[]);
  static int cmdInfo(int argc, char *argv[]);
  static int cmdSettings(int argc, char *argv[]);
  static int cmdSet(int argc, char *argv[]);
  static int cmdWifi(int argc, char *argv[]);
  static int cmdMqtt(int argc, char *argv[]);
};


#endif