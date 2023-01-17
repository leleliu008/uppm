#ifndef FIND_EXECUTABLES_H
#define FIND_EXECUTABLES_H

#include <stdlib.h>
#include <stdbool.h>

int find_executables_in_PATH(char *** listP, size_t * listSize, const char * commandName, bool findAll);

#endif
