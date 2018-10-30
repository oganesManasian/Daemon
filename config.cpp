#include <syslog.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <limits.h>
#include <string.h>

#include "config.h"

void concateStrings(char* dir, char* folder)
{
  int i = 0;
  int len;
  len = strlen(dir);
  for (i = 0; folder[i] != 0; ++i)
  {
    dir[len + i] = folder[i];
  }  
  dir[len + i] = 0;
}

void readConfig(FILE *config_fp)
{
    return;
}
