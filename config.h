#include <stdio.h>
#include <stdlib.h>
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

void concateStrings(char* dir, char* folder);
void readConfig(FILE *config_fp);
