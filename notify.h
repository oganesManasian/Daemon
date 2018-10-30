#include <stdlib.h>

//const int static MAX_FOLDERS_TO_WATCH = 10;

class Notifier {
  private:
  int fd;
  //int watchList[10];
  public:
  Notifier();
  void del(void);
  void notify_changes(void);
  void add_watch_from_config(char config_path[]);
};
