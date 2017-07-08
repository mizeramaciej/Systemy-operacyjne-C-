#ifndef HELPERS_H
#define HELPERS_H
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>

void errorExit(const char* err);
char* convertTime(const time_t* mtime);
long getTime();
#endif
