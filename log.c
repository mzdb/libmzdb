#include "log.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Source: http://stackoverflow.com/questions/6508461/logging-library-for-c
void log(const char* tag, const char* message)
{
  time_t now;

  time(&now);
  printf("%s [%s]: %s\n", ctime(&now), tag, message);
}
