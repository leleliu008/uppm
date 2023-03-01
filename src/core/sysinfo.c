#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/utsname.h>

#include "regex/regex.h"

#include "sysinfo.h"

int sysinfo_kind(char * buf, size_t bufSize) {
#if defined (_WIN32)
    strncpy(buf, "windows", bufSize > 7 ? 7 : bufSize);
    return UPPM_OK;
#elif defined (__APPLE__)
    strncpy(buf, "darwin", bufSize > 6 ? 6 : bufSize);
    return UPPM_OK;
#elif defined (__FreeBSD__)
    strncpy(buf, "freebsd", bufSize > 7 ? 7 : bufSize);
    return UPPM_OK;
#elif defined (__OpenBSD__)
    strncpy(buf, "openbsd", bufSize > 7 ? 7 : bufSize);
    return UPPM_OK;
#elif defined (__NetBSD__)
    strncpy(buf, "netbsd", bufSize > 6 ? 6 : bufSize);
    return UPPM_OK;
#elif defined (__ANDROID__)
    strncpy(buf, "android", bufSize > 7 ? 7 : bufSize);
    return UPPM_OK;
#elif defined (__linux__)
    strncpy(buf, "linux", bufSize > 5 ? 5 : bufSize);
    return UPPM_OK;
#else
    struct utsname uts;

    if (uname(&uts) < 0) {
        perror("uname() error");
        return UPPM_ERROR;
    }

    size_t osKindLength = strlen(uts.sysname);
    size_t n = bufSize > osKindLength ? osKindLength : bufSize;

    strncpy(buf, uts.sysname, n);

    for (size_t i = 0; i < n; i++) {
        if ((buf[i] >= 'A') && (buf[i] <= 'Z')) {
             buf[i] += 32;
        }
    }

    return UPPM_OK;
#endif
}

int sysinfo_type(char * buf, size_t bufSize) {
#if defined (_WIN32)
    strncpy(buf, "windows", bufSize > 7 ? 7 : bufSize);
    return UPPM_OK;
#elif defined (__APPLE__)
    strncpy(buf, "macos", bufSize > 5 ? 5 : bufSize);
    return UPPM_OK;
#elif defined (__FreeBSD__)
    strncpy(buf, "freebsd", bufSize > 7 ? 7 : bufSize);
    return UPPM_OK;
#elif defined (__OpenBSD__)
    strncpy(buf, "openbsd", bufSize > 7 ? 7 : bufSize);
    return UPPM_OK;
#elif defined (__NetBSD__)
    strncpy(buf, "netbsd", bufSize > 6 ? 6 : bufSize);
    return UPPM_OK;
#elif defined (__ANDROID__)
    strncpy(buf, "android", bufSize > 7 ? 7 : bufSize);
    return UPPM_OK;
#elif defined (__linux__)
    strncpy(buf, "linux", bufSize > 5 ? 5 : bufSize);
    return UPPM_OK;
#else
    struct utsname uts;

    if (uname(&uts) < 0) {
        perror("uname() error");
        return UPPM_ERROR;
    }

    size_t osKindLength = strlen(uts.sysname);
    size_t n = bufSize > osKindLength ? osKindLength : bufSize;

    strncpy(buf, uts.sysname, n);

    for (size_t i = 0; i < n; i++) {
        if ((buf[i] >= 'A') && (buf[i] <= 'Z')) {
             buf[i] += 32;
        }
    }

    return UPPM_OK;
#endif
}

int sysinfo_arch(char * buf, size_t bufSize) {
    struct utsname uts;

    if (uname(&uts) < 0) {
        perror("uname() error");
        return UPPM_ERROR;
    }

    size_t osArchLength = strlen(uts.machine);

    strncpy(buf, uts.machine, bufSize > osArchLength ? osArchLength : bufSize);

    return UPPM_OK;
}

int sysinfo_name(char * buf, size_t bufSize) {
#if defined (_WIN32)
    strncpy(buf, "windows", bufSize > 7 ? 7 : bufSize);
    return UPPM_OK;
#elif defined (__APPLE__)
    strncpy(buf, "macos", bufSize > 5 ? 5 : bufSize);
    return UPPM_OK;
#elif defined (__FreeBSD__)
    strncpy(buf, "freebsd", bufSize > 7 ? 7 : bufSize);
    return UPPM_OK;
#elif defined (__OpenBSD__)
    strncpy(buf, "openbsd", bufSize > 7 ? 7 : bufSize);
    return UPPM_OK;
#elif defined (__NetBSD__)
    strncpy(buf, "netbsd", bufSize > 6 ? 6 : bufSize);
    return UPPM_OK;
#elif defined (__ANDROID__)
    strncpy(buf, "android", bufSize > 7 ? 7 : bufSize);
    return UPPM_OK;
#else
    const char * filepath = "/etc/os-release";
    struct stat sb;

    if ((stat(filepath, &sb) == 0) && (S_ISREG(sb.st_mode) || S_ISLNK(sb.st_mode))) {
        FILE * file = fopen(filepath, "r");

        if (file == NULL) {
            perror(filepath);
            return UPPM_ERROR;
        }

        char line[50];

        for (;;) {
            if (fgets(line, 50, file) == NULL) {
                if (ferror(file)) {
                    perror(filepath);
                    fclose(file);
                    return UPPM_ERROR;
                } else {
                    fclose(file);
                    return UPPM_ERROR;
                }
            }

            if (strncmp(line, "ID=", 3) == 0) {
                char * p = &line[3];

                if (p[0] == '"' || p[0] == '\'') {
                    p++;
                }

                size_t n = strlen(p);

                p[n - 1] = '\0';
                n--;

                if (p[n - 1] == '"' || p[n - 1] == '\'') {
                    p[n - 1] = '\0';
                    n--;
                }

                strncpy(buf, p, bufSize > n ? n : bufSize);

                fclose(file);
                return UPPM_OK;
            }
        }
    }
#endif
}

int sysinfo_vers(char * buf, size_t bufSize) {
#if defined (__NetBSD__) || defined (__OpenBSD__)
    struct utsname uts;

    if (uname(&uts) < 0) {
        perror("uname() error");
        return UPPM_ERROR;
    }

    size_t n = strlen(uts.release);

    strncpy(buf, uts.release, bufSize > n ? n : bufSize);

    return UPPM_OK;
#elif defined (__APPLE__)
    const char * filepath = "/System/Library/CoreServices/SystemVersion.plist";
    struct stat sb;

    if ((stat(filepath, &sb) == 0) && (S_ISREG(sb.st_mode) || S_ISLNK(sb.st_mode))) {
        FILE * file = fopen(filepath, "r");

        if (file == NULL) {
            perror(filepath);
            return UPPM_ERROR;
        }

        char line[512];

        for (;;) {
            if (fgets(line, 512, file) == NULL) {
                if (ferror(file)) {
                    perror(filepath);
                    fclose(file);
                    return UPPM_ERROR;
                } else {
                    fclose(file);
                    return UPPM_ERROR;
                }
            }

            if (regex_matched(line, "ProductVersion")) {
                if (fgets(line, 512, file) == NULL) {
                    if (ferror(file)) {
                        perror(filepath);
                        fclose(file);
                        return UPPM_ERROR;
                    } else {
                        fclose(file);
                        return UPPM_ERROR;
                    }
                }

                char * p = regex_extract(line, "[1-9][0-9.]+[0-9]");

                if (p == NULL) {
                    return UPPM_ERROR;
                } else {
                    size_t n = strlen(p);
                    strncpy(buf, p, bufSize > n ? n : bufSize);
                    free(p);
                    return UPPM_OK;
                }
            }
        }
    }
#else
    const char * filepath = "/etc/os-release";
    struct stat sb;

    if ((stat(filepath, &sb) == 0) && (S_ISREG(sb.st_mode) || S_ISLNK(sb.st_mode))) {
        FILE * file = fopen(filepath, "r");

        if (file == NULL) {
            perror(filepath);
            return UPPM_ERROR;
        }

        char line[50];

        for (;;) {
            if (fgets(line, 50, file) == NULL) {
                if (ferror(file)) {
                    perror(filepath);
                    fclose(file);
                    return UPPM_ERROR;
                } else {
                    fclose(file);
                    strncpy(buf, "rolling", bufSize > 7 ? 7 : bufSize);
                    return UPPM_OK;
                }
            }

            if (strncmp(line, "VERSION_ID=", 11) == 0) {
                char * p = &line[11];

                if (p[0] == '"' || p[0] == '\'') {
                    p++;
                }

                size_t n = strlen(p);

                p[n - 1] = '\0';
                n--;

                if (p[n - 1] == '"' || p[n - 1] == '\'') {
                    p[n - 1] = '\0';
                    n--;
                }

                strncpy(buf, p, bufSize > n ? n : bufSize);
                return UPPM_OK;
            }
        }
    }
#endif
}

int sysinfo_libc(LIBC * out) {
    struct utsname uts;

    if (uname(&uts) < 0) {
        perror("uname() error");
        return UPPM_ERROR;
    }

    if (strcmp(uts.sysname, "Linux") == 0) {
        size_t osArchLength = strlen(uts.machine);

        size_t dynamicLoaderPathLength = osArchLength + 19;
        char   dynamicLoaderPath[dynamicLoaderPathLength];
        snprintf(dynamicLoaderPath, dynamicLoaderPathLength, "/lib/ld-musl-%s.so.1", uts.machine);

        struct stat sb;

        if ((stat(dynamicLoaderPath, &sb) == 0) && (S_ISREG(sb.st_mode) || S_ISLNK(sb.st_mode))) {
            (*out) = LIBC_MUSL;
        } else {
            if (strcmp(uts.machine, "x86_64") == 0) {
                const char * dynamicLoaderPath = "/lib64/ld-linux-x86-64.so.2";

                struct stat sb;

                if ((stat(dynamicLoaderPath, &sb) == 0) && (S_ISREG(sb.st_mode) || S_ISLNK(sb.st_mode))) {
                    (*out) = LIBC_GLIBC;
                } else {
                    (*out) = LIBC_UNKNOWN;
                }
            } else {
                size_t dynamicLoaderPathLength = osArchLength + 22;
                char   dynamicLoaderPath[dynamicLoaderPathLength];
                snprintf(dynamicLoaderPath, dynamicLoaderPathLength, "/lib64/ld-linux-%s.so.2", uts.machine);

                if ((stat(dynamicLoaderPath, &sb) == 0) && (S_ISREG(sb.st_mode) || S_ISLNK(sb.st_mode))) {
                    (*out) = LIBC_GLIBC;
                } else {
                    (*out) = LIBC_UNKNOWN;
                }
            }
        }
    }

    return UPPM_OK;
}

int sysinfo_ncpu(size_t * out) {
    long nprocs = 1;

#if defined (_SC_NPROCESSORS_ONLN)
    nprocs = sysconf(_SC_NPROCESSORS_ONLN);

    if (nprocs > 0) {
        (*out) = nprocs;
        return UPPM_OK;
    }
#endif

#if defined (_SC_NPROCESSORS_CONF)
    nprocs = sysconf(_SC_NPROCESSORS_CONF);

    if (nprocs > 0) {
        (*out) = nprocs;
        return UPPM_OK;
    }
#endif

    (*out) = 1;
    return UPPM_OK;
}

int sysinfo_make(SysInfo * sysinfo) {
    if (sysinfo == NULL) {
        return UPPM_ERROR_ARG_IS_NULL;
    }

    int ret;

    ///////////////////////////////////////

    char osArch[31] = {0};

    ret = sysinfo_arch(osArch, 30);

    if (ret != UPPM_OK) {
        return ret;
    }

    ///////////////////////////////////////

    char osKind[31] = {0};

    ret = sysinfo_kind(osKind, 30);

    if (ret != UPPM_OK) {
        return ret;
    }

    ///////////////////////////////////////

    char osType[31] = {0};

    ret = sysinfo_type(osType, 30);

    if (ret != UPPM_OK) {
        return ret;
    }

    ///////////////////////////////////////

    char osName[31] = {0};

    ret = sysinfo_name(osName, 30);

    if (ret != UPPM_OK) {
        return ret;
    }

    ///////////////////////////////////////

    char osVers[31] = {0};

    ret = sysinfo_vers(osVers, 30);

    if (ret != UPPM_OK) {
        return ret;
    }

    ///////////////////////////////////////

    LIBC libc = LIBC_UNKNOWN;

    ret = sysinfo_libc(&libc);

    if (ret != UPPM_OK) {
        return ret;
    }

    ///////////////////////////////////////

    size_t ncpu = 0;
    
    ret = sysinfo_ncpu(&ncpu);

    if (ret != UPPM_OK) {
        return ret;
    }

    ///////////////////////////////////////

    sysinfo->arch = strdup(osArch);

    if (sysinfo->arch == NULL) {
        return UPPM_ERROR_MEMORY_ALLOCATE;
    }

    sysinfo->kind = strdup(osKind);

    if (sysinfo->kind == NULL) {
        return UPPM_ERROR_MEMORY_ALLOCATE;
    }

    sysinfo->type = strdup(osType);

    if (sysinfo->type == NULL) {
        return UPPM_ERROR_MEMORY_ALLOCATE;
    }

    sysinfo->name = strdup(osName);

    if (sysinfo->name == NULL) {
        return UPPM_ERROR_MEMORY_ALLOCATE;
    }

    sysinfo->vers = strdup(osVers);

    if (sysinfo->vers == NULL) {
        return UPPM_ERROR_MEMORY_ALLOCATE;
    }

    sysinfo->libc = libc;
    sysinfo->ncpu = ncpu;

    sysinfo->euid = geteuid();
    sysinfo->egid = getegid();

    return UPPM_OK;
}

void sysinfo_dump(SysInfo sysinfo) {
    printf("sysinfo.ncpu: %lu\n", sysinfo.ncpu);
    printf("sysinfo.arch: %s\n",  sysinfo.arch == NULL ? "" : sysinfo.arch);
    printf("sysinfo.kind: %s\n",  sysinfo.kind == NULL ? "" : sysinfo.kind);
    printf("sysinfo.type: %s\n",  sysinfo.type == NULL ? "" : sysinfo.type);
    printf("sysinfo.name: %s\n",  sysinfo.name == NULL ? "" : sysinfo.name);
    printf("sysinfo.vers: %s\n",  sysinfo.vers == NULL ? "" : sysinfo.vers);
    printf("sysinfo.euid: %u\n",  sysinfo.euid);
    printf("sysinfo.egid: %u\n",  sysinfo.egid);

    switch(sysinfo.libc) {
        case LIBC_GLIBC: printf("sysinfo.libc: glibc\n"); break;
        case LIBC_MUSL:  printf("sysinfo.libc: musl\n");  break;
        default:         printf("sysinfo.libc: \n");
    }
}

void sysinfo_free(SysInfo sysinfo) {
    if (sysinfo.arch != NULL) {
        free(sysinfo.arch);
        sysinfo.arch = NULL;
    }

    if (sysinfo.kind != NULL) {
        free(sysinfo.kind);
        sysinfo.kind = NULL;
    }

    if (sysinfo.type != NULL) {
        free(sysinfo.type);
        sysinfo.type = NULL;
    }

    if (sysinfo.name != NULL) {
        free(sysinfo.name);
        sysinfo.name = NULL;
    }

    if (sysinfo.vers != NULL) {
        free(sysinfo.vers);
        sysinfo.vers = NULL;
    }
}
