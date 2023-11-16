#ifndef _VIEW_H_
#define _VIEW_H_

#include <shell.h>



class ViewFacade
{
  Shell m_Shell;

private:
  static void showPrompt(void);
  static void cmdNotFound(char *pc_Cmd);
  static void cmdError(char *pc_Cmd, int rc);

public:
  void setup(void);
  void loop(void);

  static void onMqttTopicReceived(const char *pc_Topic, const char *pc_Content);  
};


#endif