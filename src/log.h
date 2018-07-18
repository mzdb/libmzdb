#ifndef LOG_H
#define LOG_H

//LOG LEVELS
typedef enum
{
  LOG_TRACE = 1,
  LOG_DEBUG,
  LOG_INFO,
  LOG_WARN,
  LOG_ERROR,
  NO_LOG
} log_level_enum;
extern enum log_level_enum log_level;

// Define private global log template
static const char* LOG_TEMPLATE = "LIBMZDB [%s] - %s - %s\n";

// Define private global list of log levels
static const char* LOG_LEVELS[] =
{
  "OFF",  // 0
  "TRACE",// 1
  "DEBUG",// 2
  "INFO", // 3
  "WARN", // 4
  "ERROR",// 5
};


// TODO: add a set_log_level and flogf functions
static const int MIN_LOGGING_LEVEL = LOG_DEBUG;

// Create a logging config struct with log level and log file (if no file => use stdout/stderr)
typedef struct logging_data
{
    int min_log_level;
    FILE* file;
}logging_data_t;
static logging_data_t _LIBMZDB_LOGGING_DATA = {LOG_DEBUG};

/**
 * @brief set_log_level set the min log level to a new min level
 * @param lvl the new minimum level
 */
void set_log_level(log_level_enum lvl);

/**
 * @brief get_log_level get the current minimum log level
 * @return The minimum log level
 */
log_level_enum get_log_level(void);

/**
 * @brief set_log_file Set the new log file (the output)
 * @param file The output
 */
void set_log_file(FILE* file);
FILE* get_log_file();

// Source: http://stackoverflow.com/questions/6508461/logging-library-for-c
/**
 * @brief flog Log a message
 * @param lvl The level of the message
 * @param message The message
 * @return 0 if no error occured
 */
int flog(log_level_enum lvl, const char* message);

/**
 * @brief flogf Log a message with attribute
 * @param lvl The level of the message
 * @param msgfmt The format of each attributes
 * @return 0 if no error occured
 */
int flogf(log_level_enum lvl, const char* msgfmt, ...);

#endif /* LOG_H */
