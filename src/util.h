#ifndef UTIL_H
#define UTIL_H

#define MAX_CONCAT_STR_COUNT 10

// Source: http://stackoverflow.com/questions/8465006/how-to-concatenate-2-strings-in-c
extern char *concat(int count, ...);
extern char *chngChar(char *str, char oldChar, char newChar);

#endif /* UTIL_H */
