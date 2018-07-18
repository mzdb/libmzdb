#ifndef UTIL_H
#define UTIL_H

#include "asprintf.h"

#define MAX_CONCAT_STR_COUNT 10


/**
 * @brief strs_concat concatenate n char*
 * @param count the number of char*
 * @return a char * or NULL if an error occured
 */
extern const char* concat_fast(int count, ...);

/**
 * @brief libmzdb_strdup
 * @param str_src
 * @return
 */
extern char * libmzdb_strdup(const char *str_src);

/**
 * @brief strrep replace each occurance of targer with by
 * @param str the str to edit
 * @param target the char to replace
 * @param by the new char
 * @return the replaced str
 */
extern char* strrep(char* str, char target, char by);

/**
 * @brief atob char* to c-boolean
 * @param str the string
 * @return return 0 if the str equals to "true" else false
 */
extern int atob(char* str);

/*
// Source: http://stackoverflow.com/questions/8465006/how-to-concatenate-2-strings-in-c
extern const char *concat(int count, ...);
extern char *chngChar(char *str, char oldChar, char newChar);
*/

#endif /* UTIL_H */
