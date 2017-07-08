#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <ctype.h>
#include <time.h>
#include "helpers.h"

void errorExit(const char* err){
  printf("Error! %s Errno: %d, %s\n", err, errno, strerror(errno));
  exit(3);
}

char* convertTime(const time_t* mtime){
  char* buff = malloc(sizeof(char) * 30);
  struct tm * timeinfo;
  timeinfo = localtime (mtime);
  strftime(buff, 20, "%b %d %H:%M", timeinfo);
  return buff;
}

long getTime(){
  struct timespec marker;
  if(clock_gettime(CLOCK_MONOTONIC, &marker) == -1) errorExit("Getting time error!");
  return marker.tv_nsec / 1000;
}
