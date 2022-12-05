#ifndef UPPM_REGEX_H
#define UPPM_REGEX_H

#include<stdbool.h>

bool  regex_matched(const char * content, const char * pattern);
char* regex_extract(const char * content, const char * pattern);

#endif
