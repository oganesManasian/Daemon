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

#include "notify.h"
#define CONFIG_FILE_NAME "/config.txt"
#define PID_FILE_PATH "/run/daemon.pid"

int work = true;

void daemonise()
{
  pid_t pid, sid;
  FILE *pid_fp;

  syslog(LOG_INFO, "Starting daemonisation.");

  //First fork
  pid = fork();
  if(pid < 0)
  {
    syslog(LOG_ERR, "Error occured in the first fork while daemonising. Error number is %d", errno);
    exit(1);
  }

  if(pid > 0)
  {
    syslog(LOG_INFO, "First fork successful (Parent)");
    exit(0);
  }
  syslog(LOG_INFO, "First fork successful (Child)");

  //Create a new session
  sid = setsid();
  if(sid < 0) 
  {
    syslog(LOG_ERR, "Error occured in making a new session while daemonising. Error number is %d", errno);
    exit(1);
  }
  syslog(LOG_INFO, "New session was created successfuly!");

  //Second fork
  pid = fork();
  if(pid < 0)
  {
    syslog(LOG_ERR, "Error occured in the second fork while daemonising. Error number is %d", errno);
    exit(1);
  }

  if(pid > 0)
  {
    syslog(LOG_INFO, "Second fork successful (Parent)");
    exit(0);
  }
  syslog(LOG_INFO, "Second fork successful (Child)");

  pid = getpid();

  //Change working directory to Home directory
  if(chdir(getenv("HOME")) == -1)
  {
    syslog(LOG_ERR, "Failed to change working directory while daemonising. Error number is %d", errno);
    exit(1);
  }

  //Grant all permisions for all files and directories created by the daemon
  umask(0);

  //Redirect std IO
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);
  if(open("/dev/null",O_RDONLY) == -1)
  {
    syslog(LOG_ERR, "Failed to reopen stdin while daemonising. Error number is %d", errno);
    exit(1);
  }
  if(open("/dev/null",O_WRONLY) == -1)
  {
    syslog(LOG_ERR, "Failed to reopen stdout while daemonising. Error number is %d", errno);
    exit(1);
  }
  if(open("/dev/null",O_RDWR) == -1)
  {
    syslog(LOG_ERR, "Failed to reopen stderr while daemonising. Error number is %d", errno);
    exit(1);
  }

  //Create a pid file
  mkdir("run/", 0777);
  pid_fp = fopen("run/daemon.pid", "w");
  if(pid_fp == NULL)
  {
    syslog(LOG_ERR, "Failed to create a pid file while daemonising. Error number is %d", errno);
    exit(1);
  }
  if(fprintf(pid_fp, "%d\n", pid) < 0)
  {
    syslog(LOG_ERR, "Failed to write pid to pid file while daemonising. Error number is %d, trying to remove file", errno);
    fclose(pid_fp);
    if(remove("run/daemon.pid") != 0)
    {
      syslog(LOG_ERR, "Failed to remove pid file. Error number is %d", errno);
    }
    exit(1);
  }
  fclose(pid_fp);
}

void sig_handler(int signo)
{
  syslog(LOG_DEBUG, "Handling signals");
  if(signo == SIGTERM)
  {
    syslog(LOG_DEBUG, "SIGTERM has been caught! Exiting...");
    if(remove("run/daemon.pid") != 0)
    {
      syslog(LOG_ERR, "Failed to remove the pid file. Error number is %d", errno);
      exit(1);
    }
    syslog(LOG_DEBUG, "Removed the pid file.");
    //work = false;
    exit(0);
  }
}

void handle_signals()
{
  // Set handlers
  if(signal(SIGTERM, sig_handler) == SIG_ERR)
  {
    syslog(LOG_ERR, "Error! Can't catch SIGTERM");
    exit(1);
  }
}
 
int main(int argc, char* argv[])
{
  pid_t pid;
  FILE *pid_fp;
  char cwd[PATH_MAX];
  char config_path[PATH_MAX];
  char home_dir[PATH_MAX];

  // Check number of command line arguments
  if (argc != 2)
  {
    syslog(LOG_ERR, "Wrong command line arguments.");
    exit(1);
  }

  // Save current directory
  if (getcwd(cwd, sizeof(cwd)) == NULL)
  { 
    syslog(LOG_ERR, "Can not save current dir.");
  }
  strcpy(config_path, cwd);
  strcat(config_path, CONFIG_FILE_NAME);
  syslog(LOG_ERR, "Saved current dir.");
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     

  // Start ot stop daemon depending command line arguments
  strcat(home_dir, getenv("HOME"));
  strcat(home_dir, PID_FILE_PATH);
  pid_fp = fopen(home_dir, "r");
  if (!strcmp(argv[1], "start"))
  {
    if(pid_fp == NULL)
    {
      syslog(LOG_DEBUG, "Creating daemon.");
      // Create daemon
      daemonise();
      handle_signals();
      syslog(LOG_DEBUG, "Created daemon.");
    }
    else
    {
      // Daemon is already created => Have to do nothing
      syslog(LOG_DEBUG, "Unable to start daemon, it is already started.");
      fclose(pid_fp);
      exit(0);
    }
  }
  else if (!strcmp(argv[1], "stop"))
  {
    if(pid_fp == NULL)
    {
      // Daemon is not created => Have to do nothing
      syslog(LOG_DEBUG, "Unable to stop daemon, it is not started.");
      exit(0);
    }
    else
    {
      syslog(LOG_DEBUG, "Stopping daemon");
      // Stop Daemon
      if (fscanf(pid_fp, "%d", &pid) < 0)
      {
        syslog(LOG_ERR, "Failed to read daemon pid. Error number is %d", errno);
        fclose(pid_fp);
        exit(1);
      }
      fclose(pid_fp);
      if (pid >= 0)
      {
        syslog(LOG_DEBUG, "Killing daemon");
        syslog(LOG_DEBUG, "pid is %d", pid);
        kill(pid, SIGTERM);
      }
      else 
      {
        syslog(LOG_ERR, "Read daemon pid is strange.");
        exit(1);
      }
      exit(0);
    }
  }
  else
  {
    syslog(LOG_ERR, "Wrong 2 command line argument");
    exit(1);
  }

  Notifier notifier;
  notifier.add_watch_from_config(config_path);
  closelog();

  openlog("Disk monitor", LOG_PID, LOG_LOCAL0);
  syslog(LOG_DEBUG, "Starting disk monitor");

  while(work)
  {	
    // Checking directories
    notifier.notify_changes();

    sleep(2);
  }
  closelog();
  notifier.del();
  syslog(LOG_DEBUG, "End of daemon's work");
  return 0;
}
