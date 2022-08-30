#ifndef UPPM_UTIL_H
#define UPPM_UTIL_H

#include<stdbool.h>

bool regex_match(const char * contentStr, const char * regexStr);

int get_file_type_id_from_url(const char * url);

int get_linux_libc_type();

int get_linux_libc_type2(const char * osKind, const char * osArch);

#endif
