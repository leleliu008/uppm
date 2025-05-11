#ifndef SYSINFO_H
#define SYSINFO_H

#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>

#include <unistd.h>
#include <sys/types.h>

#define LIBC_GNU    1
#define LIBC_MUSL   2

#define SYSINFO_FIELD_BUFFER_SIZE 21

typedef struct {
   char kind[SYSINFO_FIELD_BUFFER_SIZE];
   char type[SYSINFO_FIELD_BUFFER_SIZE];
   char code[SYSINFO_FIELD_BUFFER_SIZE];
   char name[SYSINFO_FIELD_BUFFER_SIZE];
   char vers[SYSINFO_FIELD_BUFFER_SIZE];
   char arch[SYSINFO_FIELD_BUFFER_SIZE];
   unsigned int libc;
   unsigned int ncpu;
   uid_t  euid;
   gid_t  egid;
} SysInfo;

int  sysinfo_kind(char buf[], size_t bufSize);
int  sysinfo_type(char buf[], size_t bufSize);
int  sysinfo_code(char buf[], size_t bufSize);
int  sysinfo_name(char buf[], size_t bufSize);
int  sysinfo_vers(char buf[], size_t bufSize);
int  sysinfo_arch(char buf[], size_t bufSize);
int  sysinfo_libc();
int  sysinfo_ncpu();

int  sysinfo_make(SysInfo * sysinfo);
void sysinfo_dump(SysInfo * sysinfo);
void sysinfo_dump_as_shell_script(SysInfo * sysinfo);

#endif
