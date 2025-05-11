#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/stat.h>
#include <sys/utsname.h>

#if defined (__APPLE__)
#include <fcntl.h>
#include <sys/mman.h>
#elif defined (__ANDROID__)
#include <sys/system_properties.h>
#endif

#include "sysinfo.h"

int sysinfo_kind(char * buf, size_t bufSize) {
#if defined (_WIN32)
    strncpy(buf, "windows", (bufSize > 7U) ? 7U : bufSize);
    return 0;
#elif defined (__APPLE__)
    strncpy(buf, "darwin",  (bufSize > 6U) ? 6U : bufSize);
    return 0;
#elif defined (__DragonFly__)
    strncpy(buf, "dragonflybsd", (bufSize > 12U) ? 12U : bufSize);
    return 0;
#elif defined (__FreeBSD__)
    strncpy(buf, "freebsd", (bufSize > 7U) ? 7U : bufSize);
    return 0;
#elif defined (__OpenBSD__)
    strncpy(buf, "openbsd", (bufSize > 7U) ? 7U : bufSize);
    return 0;
#elif defined (__NetBSD__)
    strncpy(buf, "netbsd",  (bufSize > 6U) ? 6U : bufSize);
    return 0;
#elif defined (__ANDROID__)
    strncpy(buf, "android", (bufSize > 7U) ? 7U : bufSize);
    return 0;
#elif defined (__linux__)
    strncpy(buf, "linux",   (bufSize > 5U) ? 5U : bufSize);
    return 0;
#else
    struct utsname uts;

    if (uname(&uts) < 0) {
        return -1;
    }

    size_t osKindLength = strlen(uts.sysname);
    size_t n = (bufSize > osKindLength) ? osKindLength : bufSize;

    strncpy(buf, uts.sysname, n);

    for (size_t i = 0; i < n; i++) {
        if ((buf[i] >= 'A') && (buf[i] <= 'Z')) {
             buf[i] += 32;
        }
    }

    return 0;
#endif
}

int sysinfo_type(char * buf, size_t bufSize) {
#if defined (_WIN32)
    strncpy(buf, "windows", (bufSize > 7U) ? 7U : bufSize);
    return 0;
#elif defined (__APPLE__)
    strncpy(buf, "macos",   (bufSize > 5U) ? 5U : bufSize);
    return 0;
#elif defined (__DragonFly__)
    strncpy(buf, "dragonflybsd", (bufSize > 12U) ? 12U : bufSize);
    return 0;
#elif defined (__FreeBSD__)
    strncpy(buf, "freebsd", (bufSize > 7U) ? 7U : bufSize);
    return 0;
#elif defined (__OpenBSD__)
    strncpy(buf, "openbsd", (bufSize > 7U) ? 7U : bufSize);
    return 0;
#elif defined (__NetBSD__)
    strncpy(buf, "netbsd",  (bufSize > 6U) ? 6U : bufSize);
    return 0;
#elif defined (__ANDROID__)
    strncpy(buf, "android", (bufSize > 7U) ? 7U : bufSize);
    return 0;
#elif defined (__linux__)
    strncpy(buf, "linux",   (bufSize > 5U) ? 5U : bufSize);
    return 0;
#else
    struct utsname uts;

    if (uname(&uts) < 0) {
        return -1;
    }

    size_t osKindLength = strlen(uts.sysname);
    size_t n = (bufSize > osKindLength) ? osKindLength : bufSize;

    strncpy(buf, uts.sysname, n);

    for (size_t i = 0; i < n; i++) {
        if ((buf[i] >= 'A') && (buf[i] <= 'Z')) {
             buf[i] += 32;
        }
    }

    return 0;
#endif
}

int sysinfo_arch(char * buf, size_t bufSize) {
    struct utsname uts;

    if (uname(&uts) < 0) {
        return -1;
    }

    size_t osArchLength = strlen(uts.machine);

    strncpy(buf, uts.machine, (bufSize > osArchLength) ? osArchLength : bufSize);

    return 0;
}

int sysinfo_code(char * buf, size_t bufSize) {
#if defined (_WIN32)
    strncpy(buf, "windows", (bufSize > 7U) ? 7U : bufSize);
    return 0;
#elif defined (__APPLE__)
    strncpy(buf, "macos",   (bufSize > 5U) ? 5U : bufSize);
    return 0;
#elif defined (__DragonFly__)
    strncpy(buf, "dragonflybsd", (bufSize > 12U) ? 12U : bufSize);
    return 0;
#elif defined (__FreeBSD__)
    strncpy(buf, "freebsd", (bufSize > 7U) ? 7U : bufSize);
    return 0;
#elif defined (__OpenBSD__)
    strncpy(buf, "openbsd", (bufSize > 7U) ? 7U : bufSize);
    return 0;
#elif defined (__NetBSD__)
    strncpy(buf, "netbsd",  (bufSize > 6U) ? 6U : bufSize);
    return 0;
#elif defined (__ANDROID__)
    strncpy(buf, "android", (bufSize > 7U) ? 7U : bufSize);
    return 0;
#else
    const char * const filepath = "/etc/os-release";
    struct stat sb;

    if ((stat(filepath, &sb) == 0) && (S_ISREG(sb.st_mode) || S_ISLNK(sb.st_mode))) {
        FILE * file = fopen(filepath, "r");

        if (file == NULL) {
            return -1;
        }

        char line[50];

        for (;;) {
            if (fgets(line, 50, file) == NULL) {
                if (ferror(file)) {
                    perror(filepath);
                    fclose(file);
                    return -1;
                } else {
                    fclose(file);
                    return -1;
                }
            }

            if (strncmp(line, "ID=", 3) == 0) {
                char * p = &line[3];

                if ((p[0] == '"') || (p[0] == '\'')) {
                    p++;
                }

                size_t n = strlen(p);

                p[n - 1] = '\0';
                n--;

                if ((p[n - 1] == '"') || (p[n - 1] == '\'')) {
                    p[n - 1] = '\0';
                    n--;
                }

                strncpy(buf, p, (bufSize > n) ? n : bufSize);

                fclose(file);

                return 0;
            }
        }
    }

    return -1;
#endif
}

int sysinfo_name(char * buf, size_t bufSize) {
#if defined (_WIN32)
    strncpy(buf, "Windows", (bufSize > 7U) ? 7U : bufSize);
    return 0;
#elif defined (__APPLE__)
    strncpy(buf, "macOS",   (bufSize > 5U) ? 5U : bufSize);
    return 0;
#elif defined (__DragonFly__)
    strncpy(buf, "DragonFlyBSD", (bufSize > 12U) ? 12U : bufSize);
    return 0;
#elif defined (__FreeBSD__)
    strncpy(buf, "FreeBSD", (bufSize > 7U) ? 7U : bufSize);
    return 0;
#elif defined (__OpenBSD__)
    strncpy(buf, "OpenBSD", (bufSize > 7U) ? 7U : bufSize);
    return 0;
#elif defined (__NetBSD__)
    strncpy(buf, "NetBSD",  (bufSize > 6U) ? 6U : bufSize);
    return 0;
#elif defined (__ANDROID__)
    strncpy(buf, "Android", (bufSize > 7U) ? 7U : bufSize);
    return 0;
#else
    const char * const filepath = "/etc/os-release";
    struct stat sb;

    if ((stat(filepath, &sb) == 0) && (S_ISREG(sb.st_mode) || S_ISLNK(sb.st_mode))) {
        FILE * file = fopen(filepath, "r");

        if (file == NULL) {
            return -1;
        }

        char line[50];

        for (;;) {
            if (fgets(line, 50, file) == NULL) {
                if (ferror(file)) {
                    perror(filepath);
                    fclose(file);
                    return -1;
                } else {
                    fclose(file);
                    return -1;
                }
            }

            if (strncmp(line, "NAME=", 5) == 0) {
                char * p = &line[5];

                if ((p[0] == '"') || (p[0] == '\'')) {
                    p++;
                }

                size_t n = strlen(p);

                p[n - 1] = '\0';
                n--;

                if ((p[n - 1] == '"') || (p[n - 1] == '\'')) {
                    p[n - 1] = '\0';
                    n--;
                }

                strncpy(buf, p, (bufSize > n) ? n : bufSize);

                fclose(file);

                return 0;
            }
        }
    }

    return -1;
#endif
}

int sysinfo_vers(char * buf, size_t bufSize) {
#if defined (__NetBSD__) || defined (__OpenBSD__)
    struct utsname uts;

    if (uname(&uts) < 0) {
        return -1;
    }

    size_t n = strlen(uts.release);

    strncpy(buf, uts.release, (bufSize > n) ? n : bufSize);

    return 0;
#elif defined (__ANDROID__)
    char buff[PROP_VALUE_MAX];

    int n = __system_property_get("ro.build.version.release", buff);

    if (n > 0) {
        strncpy(buf, buff, (bufSize > n) ? n : bufSize);
        return 0;
    } else {
        return -1;
    }
#elif defined (__APPLE__)
    const char * const fp = "/System/Library/CoreServices/SystemVersion.plist";

    struct stat st;

    if (stat(fp, &st) == -1) {
        perror(fp);
        return -1;
    }

    if (st.st_size == 0) {
        fprintf(stderr, "empty file: %s\n", fp);
        return -1;
    }

    int fd = open(fp, O_RDONLY);

    if (fd == -1) {
        perror(fp);
        return -1;
    }

    char * data = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0);

    if (data == MAP_FAILED) {
        perror(fp);
        close(fd);
        return -1;
    }

    char * p = strstr(data, "<key>ProductVersion</key>");

    if (p == NULL) {
        fprintf(stderr, "no <key>ProductVersion</key> in file : %s\n", fp);

        if (munmap(data, st.st_size) == -1) {
            perror("Failed to unmap file");
        }

        close(fd);

        return -1;
    }

    p = strstr(p, "<string>");

    if (p == NULL) {
        fprintf(stderr, "no <string> after <key>ProductVersion</key> in file : %s\n", fp);

        if (munmap(data, st.st_size) == -1) {
            perror("Failed to unmap file");
        }

        close(fd);

        return -1;
    }

    p += 8;

    char * q = strstr(p, "</string>");

    if (q == NULL) {
        fprintf(stderr, "no </string> after <key>ProductVersion</key> in file : %s\n", fp);

        if (munmap(data, st.st_size) == -1) {
            perror("Failed to unmap file");
        }

        close(fd);

        return -1;
    }

    ////////////////////////////////////////////

    size_t m = bufSize - 1U;
    size_t n = q - p;

    size_t len = (m > n) ? n : m;

    for (size_t i = 0; i < len; i++) {
        buf[i] = p[i];
    }

    buf[len] = '\0';

    if (munmap(data, st.st_size) == -1) {
        perror("Failed to unmap file");
    }

    close(fd);

    return 0;
#else
    const char * const filepath = "/etc/os-release";
    struct stat sb;

    if ((stat(filepath, &sb) == 0) && (S_ISREG(sb.st_mode) || S_ISLNK(sb.st_mode))) {
        FILE * file = fopen(filepath, "r");

        if (file == NULL) {
            return -1;
        }

        char line[50];

        for (;;) {
            if (fgets(line, 50, file) == NULL) {
                if (ferror(file)) {
                    perror(filepath);
                    fclose(file);
                    return -1;
                } else {
                    fclose(file);
                    strncpy(buf, "rolling", (bufSize > 7U) ? 7U : bufSize);
                    return 0;
                }
            }

            if (strncmp(line, "VERSION_ID=", 11) == 0) {
                char * p = &line[11];

                if ((p[0] == '"') || (p[0] == '\'')) {
                    p++;
                }

                size_t n = strlen(p);

                p[n - 1] = '\0';
                n--;

                if ((p[n - 1] == '"') || (p[n - 1] == '\'')) {
                    p[n - 1] = '\0';
                    n--;
                }

                strncpy(buf, p, (bufSize > n) ? n : bufSize);

                fclose(file);

                return 0;
            }
        }
    }

    return -1;
#endif
}

#if defined (__ANDROID__)
int sysinfo_libc() {
    return 0;
}
#elif defined (__linux__)
int determine_by_inspect_elf_files();
int sysinfo_libc() {
    return determine_by_inspect_elf_files();
}
#else
int sysinfo_libc() {
    return 0;
}
#endif

int sysinfo_ncpu() {
    long nprocs;

#if defined (_SC_NPROCESSORS_ONLN)
    nprocs = sysconf(_SC_NPROCESSORS_ONLN);

    if (nprocs > 0L) {
        return nprocs;
    }
#endif

#if defined (_SC_NPROCESSORS_CONF)
    nprocs = sysconf(_SC_NPROCESSORS_CONF);

    if (nprocs > 0L) {
        return nprocs;
    }
#endif

    nprocs = 1L;
    return nprocs;
}

int sysinfo_make(SysInfo * sysinfo) {
    if (sysinfo == NULL) {
        errno = EINVAL;
        return -1;
    }

    int ret;

    ///////////////////////////////////////

    char osArch[31] = {0};

    ret = sysinfo_arch(osArch, 30);

    if (ret != 0) {
        return ret;
    }

    ///////////////////////////////////////

    char osKind[31] = {0};

    ret = sysinfo_kind(osKind, 30);

    if (ret != 0) {
        return ret;
    }

    ///////////////////////////////////////

    char osType[31] = {0};

    ret = sysinfo_type(osType, 30);

    if (ret != 0) {
        return ret;
    }

    ///////////////////////////////////////

    char osCode[31] = {0};

    ret = sysinfo_code(osCode, 30);

    if (ret != 0) {
        return ret;
    }

    ///////////////////////////////////////

    char osName[31] = {0};

    ret = sysinfo_name(osName, 30);

    if (ret != 0) {
        return ret;
    }

    ///////////////////////////////////////

    char osVers[31] = {0};

    ret = sysinfo_vers(osVers, 30);

    if (ret != 0) {
        return ret;
    }

    ///////////////////////////////////////

    int libc = sysinfo_libc();

    if (libc < 0) {
        return libc;
    }

    ///////////////////////////////////////

    int ncpu = sysinfo_ncpu();

    if (ncpu < 0) {
        return ncpu;
    }

    ///////////////////////////////////////

    sysinfo->arch = strdup(osArch);

    if (sysinfo->arch == NULL) {
        sysinfo_free(*sysinfo);
        errno = ENOMEM;
        return -1;
    }

    sysinfo->kind = strdup(osKind);

    if (sysinfo->kind == NULL) {
        sysinfo_free(*sysinfo);
        errno = ENOMEM;
        return -1;
    }

    sysinfo->type = strdup(osType);

    if (sysinfo->type == NULL) {
        sysinfo_free(*sysinfo);
        errno = ENOMEM;
        return -1;
    }

    sysinfo->code = strdup(osCode);

    if (sysinfo->code == NULL) {
        sysinfo_free(*sysinfo);
        errno = ENOMEM;
        return -1;
    }

    sysinfo->name = strdup(osName);

    if (sysinfo->name == NULL) {
        sysinfo_free(*sysinfo);
        errno = ENOMEM;
        return -1;
    }

    sysinfo->vers = strdup(osVers);

    if (sysinfo->vers == NULL) {
        sysinfo_free(*sysinfo);
        errno = ENOMEM;
        return -1;
    }

    sysinfo->libc = libc;
    sysinfo->ncpu = ncpu;

    sysinfo->euid = geteuid();
    sysinfo->egid = getegid();

    return 0;
}

void sysinfo_dump(SysInfo sysinfo) {
    printf("sysinfo.ncpu: %u\n",  sysinfo.ncpu);
    printf("sysinfo.arch: %s\n", (sysinfo.arch == NULL) ? "" : sysinfo.arch);
    printf("sysinfo.kind: %s\n", (sysinfo.kind == NULL) ? "" : sysinfo.kind);
    printf("sysinfo.type: %s\n", (sysinfo.type == NULL) ? "" : sysinfo.type);
    printf("sysinfo.code: %s\n", (sysinfo.code == NULL) ? "" : sysinfo.code);
    printf("sysinfo.name: %s\n", (sysinfo.name == NULL) ? "" : sysinfo.name);
    printf("sysinfo.vers: %s\n", (sysinfo.vers == NULL) ? "" : sysinfo.vers);
    printf("sysinfo.euid: %u\n",  sysinfo.euid);
    printf("sysinfo.egid: %u\n",  sysinfo.egid);

    switch(sysinfo.libc) {
        case 1:  printf("sysinfo.libc: glibc\n"); break;
        case 2:  printf("sysinfo.libc: musl\n");  break;
        case 3:  printf("sysinfo.libc: bonic\n"); break;
        default: printf("sysinfo.libc: \n");
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

    if (sysinfo.code != NULL) {
        free(sysinfo.code);
        sysinfo.code = NULL;
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
