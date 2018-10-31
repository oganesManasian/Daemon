#include <stdlib.h>


class Notifier {
  private:
  int fd;
  public:
  Notifier();
  void del(void);
  void notify_changes(void);
  void add_watch_from_config(char config_path[]);
};
