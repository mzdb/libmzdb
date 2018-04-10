#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "log.h"
#include "util.h"

// Source: http://stackoverflow.com/questions/6508461/logging-library-for-c
int _log(LogLevel lvl, const char* message)
{
  // Return if logging level is too low
  if (lvl <= MIN_LOGGING_LEVEL) return 0;
  /*time_t now;
  time(&now);
  printf(LOG_TEMPLATE, LOG_LEVELS[lvl], ctime(&now), message);*/

  char timestr[1000];
  time_t now = time(NULL);
  struct tm * p = localtime(&now);
  strftime(timestr, 1000, "%c", p);

  if (lvl >= LOG_WARN) {
    return fprintf(stderr,LOG_TEMPLATE, LOG_LEVELS[lvl], timestr, message,"\n");
  }
  else {
    int rc = fprintf(stdout,LOG_TEMPLATE, LOG_LEVELS[lvl], timestr, message,"\n");
    fflush(stdout); // we have to flush stdout because it is buffered (note that stderr is unbuffered)
    return rc;
  }
}

int _logf(LogLevel lvl, const char* msgfmt, ...)
{
  // Return if logging level is too low
  if (lvl <= MIN_LOGGING_LEVEL) return 0;

  int size = 0;
  va_list args;
  char* message;

  // init variadic arguments
  va_start(args, msgfmt);

  // format and get size
  vasprintf(&message, msgfmt, args);

  // toss args
  va_end(args);

  // log message
  size = _log(lvl, message);

  // free memory
  if (size >= 0) {
    free(message);
  }

  return size;
}

