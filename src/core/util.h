#ifndef UPPM_UTIL_H
#define UPPM_UTIL_H

int get_file_type_id_from_url(const char * url);

int get_linux_libc_type();

int get_linux_libc_type2(const char * osKind, const char * osArch);

#endif
