#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "log.h"
#include "util.h"

log_level_enum get_log_level(void)
{
    return _LIBMZDB_LOGGING_DATA.min_log_level;
}

void set_log_level(log_level_enum lvl)
{
    _LIBMZDB_LOGGING_DATA.min_log_level = lvl;
}

FILE* get_log_file()
{
    return _LIBMZDB_LOGGING_DATA.file;
}

void set_log_file(FILE* file)
{
    _LIBMZDB_LOGGING_DATA.file = file;
}

char* time_to_str()
{
    /*const size_t str_time_size = 256;
    char* str_time = (char*) malloc(sizeof(char)*str_time_size);
    time_t now = time(NULL);
    strftime(str_time, str_time_size, "%c", localtime(&now));*/

    char str_time[1024];
    time_t now = time(NULL);
    strftime(str_time, 1024, "%c", localtime(&now));

    return str_time;
}

// Source: http://stackoverflow.com/questions/6508461/logging-library-for-c
// TODO: implement log_gile function?
// Source: https://github.com/rxi/log.c/blob/master/src/log.c
int flog(log_level_enum lvl, const char* message) {
    if(lvl <= _LIBMZDB_LOGGING_DATA.min_log_level || !_LIBMZDB_LOGGING_DATA.file) return 0;

    char* str_time = time_to_str();
    if(_LIBMZDB_LOGGING_DATA.file == stdout && lvl >= LOG_WARN)
    {
        return fprintf(stderr, LOG_TEMPLATE, LOG_LEVELS[lvl], str_time, message, "\n");
    }
    else
    {
        int rc = fprintf(_LIBMZDB_LOGGING_DATA.file, LOG_TEMPLATE, LOG_LEVELS[lvl], str_time, message, "\n");
        fflush(_LIBMZDB_LOGGING_DATA.file);

        return rc;
    }
}

int flogf(log_level_enum lvl, const char* msgfmt, ...)
{
  // Return if logging level is too low
  if(lvl <=  _LIBMZDB_LOGGING_DATA.min_log_level || !_LIBMZDB_LOGGING_DATA.file) return 0;

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
  size = flog(lvl, message);

  // free memory
  if (size >= 0)
  {
    free(message);
  }

  return size;
}


/* OLD IMPLEMENTATIONS
int flog(log_level_enum lvl, const char* message)
{
  // Return if logging level is too low
  if (lvl <= MIN_LOGGING_LEVEL) return 0;

  ///time_t now;
  //time(&now);
  //printf(LOG_TEMPLATE, LOG_LEVELS[lvl], ctime(&now), message);

  char timestr[1000];
  time_t now = time(NULL);
  struct tm * p = localtime(&now);
  strftime(timestr, 1000, "%c", p);

  if (lvl >= LOG_WARN)
  {
    return fprintf(stderr,LOG_TEMPLATE, LOG_LEVELS[lvl], timestr, message,"\n");
  }
  else
  {
    int rc = fprintf(stdout,LOG_TEMPLATE, LOG_LEVELS[lvl], timestr, message,"\n");
    fflush(stdout); // we have to flush stdout because it is buffered (note that stderr is unbuffered)
    return rc;
  }
}

*/





