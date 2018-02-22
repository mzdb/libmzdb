#ifndef LOG_H
#define LOG_H

//LOG LEVELS
typedef enum
{
  LOG_TRACE = 1,
  LOG_DEBUG,
  LOG_INFO,
  LOG_WARN,
  LOG_ERROR
} LogLevel;
extern enum LogLevel logLevel;

// Define private global log template
static const char* LOG_TEMPLATE = "LIBMZDB [%s] - %s - %s\n";

// Define private global list of log levels
static const char* LOG_LEVELS[] = {
  "OFF",  // 0
  "TRACE",// 1
  "DEBUG",// 2
  "INFO", // 3
  "WARN", // 4
  "ERROR",// 5
};

static const int MIN_LOGGING_LEVEL = LOG_DEBUG;

// Source: http://stackoverflow.com/questions/6508461/logging-library-for-c
int _log(LogLevel lvl, const char* message);
int _logf(LogLevel lvl, const char* msgfmt, ...);

#endif /* LOG_H */
