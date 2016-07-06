#include "util.h"
#include <stdio.h>
#include <stdlib.h>       // calloc
#include <stdarg.h>       // va_*
#include <string.h>       // strlen, strcpy
#include "lib/sqlite/sqlite3.h"

// Source: http://stackoverflow.com/questions/8465006/how-to-concatenate-2-strings-in-c
char* concat(int count, ...)
{
  va_list ap;
  int i;
  int len;
  char *merged;
  int null_pos;

  if (count > MAX_CONCAT_STR_COUNT)
    {
      printf("Too many strings to concat\n");
      exit(EXIT_FAILURE);
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
  merged = (char*)calloc(sizeof(char), len);
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


