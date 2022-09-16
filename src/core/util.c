#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <regex.h>
#include <sys/utsname.h>
#include "regex/regex.h"
#include "util.h"
#include "fs.h"

int get_file_type_id_from_url(const char * url) {
    if (regex_match(url, ".*\\.zip$")) {
        return 1;
    } else if (regex_match(url, ".*\\.tar\\.gz$")) {
        return 2;
    } else if (regex_match(url, ".*\\.tar\\.xz$")) {
        return 3;
    } else if (regex_match(url, ".*\\.tar\\.lz$")) {
        return 4;
    } else if (regex_match(url, ".*\\.tar\\.bz2$")) {
        return 5;
    } else if (regex_match(url, ".*\\.tgz$")) {
        return 2;
    } else if (regex_match(url, ".*\\.txz$")) {
        return 3;
    } else if (regex_match(url, ".*\\.tlz$")) {
        return 4;
    } else {
        return 0;
    }
}

#define LINUX_LIBC_TYPE_ERROR        -1
#define LINUX_LIBC_TYPE_NOT_LINUX    -2

#define LINUX_LIBC_TYPE_UNKNOWN 0
#define LINUX_LIBC_TYPE_GLIBC   1
#define LINUX_LIBC_TYPE_MUSL    2

int get_linux_libc_type() {
    return get_linux_libc_type2(NULL, NULL);
}

int get_linux_libc_type2(const char * osKind, const char * osArch) {
    if ((osKind == NULL) || (osArch == NULL)) {
        struct utsname uts;

        if (uname(&uts) < 0) {
            perror("uname() error");
            return LINUX_LIBC_TYPE_ERROR;
        }

        osKind = uts.sysname;
        osArch = uts.machine;
    }

    if (strcmp(osKind, "Linux") == 0) {
        char dynamicLoaderPath[40] = {0};
        sprintf(dynamicLoaderPath, "/lib/ld-musl-%s.so.1", osArch);

        if (exists_and_is_a_regular_file(dynamicLoaderPath) || exists_and_is_a_symbolic_link(dynamicLoaderPath)) {
            return LINUX_LIBC_TYPE_MUSL;
        } else {
            memset(dynamicLoaderPath, 0, 40);

            if (strcmp(osArch, "x86_64") == 0) {
                strcpy(dynamicLoaderPath, "/lib64/ld-linux-x86-64.so.2");
            } else {
                sprintf(dynamicLoaderPath, "/lib64/ld-linux-%s.so.2", osArch);
            }

            if (exists_and_is_a_regular_file(dynamicLoaderPath) || exists_and_is_a_symbolic_link(dynamicLoaderPath)) {
                return LINUX_LIBC_TYPE_GLIBC;
            } else {
                return LINUX_LIBC_TYPE_UNKNOWN;
            }
        }
    } else {
        return LINUX_LIBC_TYPE_NOT_LINUX;
    }
}

