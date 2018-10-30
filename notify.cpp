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
#include <sys/inotify.h>
#include <vector>

#include "notify.h"

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define EVENT_BUF_LEN     ( 10 * ( EVENT_SIZE + 16 ) )

void Notifier::notify_changes()
{
  syslog(LOG_DEBUG, "Notifing changes");
  char buffer[EVENT_BUF_LEN];
  int i = 0;

  int length = read( this->fd, buffer, EVENT_BUF_LEN ); 

  if ( length < 0 ) {
    syslog(LOG_ERR, "Error while reading events");
    exit(1);
  }  

  struct inotify_event *event;
  for (int i = 0; i < length; i += EVENT_SIZE + event->len) {     
    event = ( struct inotify_event * ) &buffer[ i ];     
    if ( event->len ) {
      if ( event->mask & IN_ISDIR ) { // For directories
        if ( event->mask & IN_CREATE ) {
          syslog(LOG_INFO, "New directory %s was created.\n", event->name );
        } else if ( event->mask & IN_ATTRIB ) {
          syslog(LOG_INFO, "Directory %s metadata changed.\n", event->name );
        } else if ( event->mask & IN_OPEN) {
          syslog(LOG_INFO, "Directory %s was opened.\n", event->name );
        } else if ( event->mask & IN_DELETE) {
          syslog(LOG_INFO, "File %s was deleted.\n", event->name );
        }
      } else { // For files
        if ( event->mask & IN_CREATE ) {
          syslog(LOG_INFO, "New file %s was created.\n", event->name );
        } else if ( event->mask & IN_ACCESS ) {
          syslog(LOG_INFO, "File %s was accessed.\n", event->name );
        } else if ( event->mask & IN_ATTRIB ) {
          syslog(LOG_INFO, "File %s metadata changed.\n", event->name );
        } else if ( event->mask & IN_MODIFY ) {
          syslog(LOG_INFO, "File %s was modified.\n", event->name );
        } else if ( event->mask & IN_OPEN) {
          syslog(LOG_INFO, "File %s was opened.\n", event->name );
        } else if ( event->mask & IN_DELETE) {
          syslog(LOG_INFO, "File %s was deleted.\n", event->name );
        } 
      } // if file
    } // if ( event->len )
  } // for
  return;
}

Notifier::Notifier(void)
{
  this->fd = inotify_init();
  if (this->fd <= 0)
  {
    syslog(LOG_ERR, "Can not create notifier");
    exit(1);
  }
  syslog(LOG_DEBUG, "Created notifier");
}

void Notifier::add_watch_from_config(char config_path[])
{
  // Open config file
  syslog(LOG_DEBUG, "Config path is %s", config_path);
  FILE *config_fp = fopen(config_path, "r");
  if (config_fp == NULL) {
    syslog(LOG_ERR, "Can not open config. Error number is %d", errno);
    remove("run/daemon.pid");
    exit(1);
  }
  syslog(LOG_DEBUG, "Opened config.");

  // Read config file and add folders to watch
  char line[100];
  while (fscanf(config_fp, "%s", line) > 0) {
    syslog(LOG_INFO, "Scanned line: %s", line);
    int wd = inotify_add_watch( this->fd, line, IN_ALL_EVENTS );
    if (wd < 0) {
      syslog(LOG_WARNING, "Can not add %s to watch list. Error number: %d", line, errno);
    } else {
      syslog(LOG_DEBUG, "Successfully added %s to watch list", line);
    }
  }

  fclose(config_fp);
  syslog(LOG_DEBUG, "Filled watch list");
}

void Notifier::del(void)
{
  syslog(LOG_DEBUG, "Closing disk monitor");
  if (this->fd)
    close(this->fd);
}


