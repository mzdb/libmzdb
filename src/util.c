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

char * libmzdb_strdup(const char * str_src)
{
    char * str_dest = (char *) malloc(sizeof(char) * strlen(str_src));
    strcpy(str_dest, str_src);
    return str_dest;
}

//TODO
/**
 * @brief strs_concat concatenate n char*
 * @param count the number of char*
 * @return a char * or NULL if an error occured
 */
const char* concat_fast(int count, ...)
{
    va_list va;
    if (count<1) return NULL; //no char* to concatenate

    int* arg_pos=(int *) malloc(sizeof(int)*(count+1)); //the array of each arg position in the new char *

    va_start(va, count);
    arg_pos[0]= 0;
    for(int i=1; i<count; ++i)arg_pos[i]= arg_pos[i-1]+strlen(va_arg(va, char*));
    va_end(va);

    //copy the arg in the variable out at the right position
    char* out =(char *) calloc(sizeof(char), arg_pos[count]+1);
    va_start(va, count);
    for(int i=0; i<count; ++i) strcpy(out+arg_pos[i], va_arg(va, const char*));
    va_end(va);
    free(arg_pos);
    return out;
}

/**
 * @brief strrep replace each occurance of targer with by
 * @param str the str to edit
 * @param target the char to replace
 * @param by the new char
 * @return the replaced str
 */
char* strrep(char* str, char target, char by)
{
    int len = strlen(str);
    for(int i=0; i<len; ++i) if (str[i]=target) str[i]=by;
    return str;
}

/**
 * @brief atob char* to c-boolean
 * @param str the string
 * @return return 0 if the str equals to "true" else false
 */
int atob(char* str)
{
    return strcmp(str, "true") == 0;
}

/*int _btoi(char* bytes)
{
    int out = bytes[0] | ( (int)bytes[1] << 8 ) | ( (int)bytes[2] << 16 ) | ( (int)bytes[3] << 24 );
    return out;
}*/

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
    char* s;
    for (i = 0; i < count; i++)
    {
        s = va_arg(ap, char*);
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


