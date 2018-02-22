#include <stdarg.h>       // va_*
#include <stdio.h>
#include <stdlib.h>       // calloc
#include <string.h>       // strlen, strcpy

#include "util.h"

/**
 * `asprintf.c' - asprintf
 *
 * Source: https://github.com/littlstar/asprintf.c/blob/master/asprintf.c
 *
 * copyright (c) 2014 joseph werle <joseph.werle@gmail.com>
 */


// Source: http://stackoverflow.com/questions/8465006/how-to-concatenate-2-strings-in-c
const char* concat(int count, ...)
{
  va_list ap;
  int i;
  int len;
  const char *merged;
  int null_pos;

  if (count > MAX_CONCAT_STR_COUNT)
    {
      fputs("concat (util.c): too many strings to concat !", stderr);
      return NULL;
    }

  // Find required length to store merged string
  len = 1;   // room for NULL
  va_start(ap, count);
  for (i = 0; i < count; i++)
    {
      len += strlen(va_arg(ap, char*));
    }
  va_end(ap);

  // Allocate memory to concat strings
  merged = (const char*)calloc(sizeof(char), len);
  null_pos = 0;

  // Actually concatenate strings
  va_start(ap, count);
  for (i = 0; i < count; i++)
    {
      char *s = va_arg(ap, char*);
      strcpy(merged + null_pos, s);
      null_pos += strlen(s);
    }
  va_end(ap);

  return merged;
}

char *chngChar(char *str, char oldChar, char newChar)
{
  char *strPtr = str;

  while ((strPtr = strchr(strPtr, oldChar)) != NULL)
    *strPtr++ = newChar;

  return str;
}

//duplicates a string with memory allocation
/*void strmemdup(char** dest, const char *str) {
    size_t len;

    len = strlen(str) + 1;
    if (!(*dest = malloc(sizeof(char) * len)))
        return;

    memcpy(*dest, str, len);
   }*/


