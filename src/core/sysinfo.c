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

static inline __attribute__((always_inline)) void copy2buf(char buf[], size_t bufSize, const char * s) {
    size_t m = strlen(s) + 1;
    size_t capacity = (bufSize > m) ? m : bufSize;
    size_t len = capacity - 1;

    for (size_t i = 0; i < len; i++) {
        buf[i] = s[i];
    }

    buf[len] = '\0';
}

int sysinfo_kind(char buf[], size_t bufSize) {
#if defined (_WIN32)
    copy2buf(buf, bufSize, "windows");
    return 0;
#elif defined (__APPLE__)
    copy2buf(buf, bufSize, "darwin");
    return 0;
#elif defined (__DragonFly__)
    copy2buf(buf, bufSize, "dragonflybsd");
    return 0;
#elif defined (__MidnightBSD__)
    copy2buf(buf, bufSize, "midnightbsd");
    return 0;
#elif defined (__FreeBSD__)
    copy2buf(buf, bufSize, "freebsd");
    return 0;
#elif defined (__OpenBSD__)
    copy2buf(buf, bufSize, "openbsd");
    return 0;
#elif defined (__NetBSD__)
    copy2buf(buf, bufSize, "netbsd");
    return 0;
#elif defined (__ANDROID__)
    copy2buf(buf, bufSize, "android");
    return 0;
#elif defined (__linux__)
    copy2buf(buf, bufSize, "linux");
    return 0;
#elif defined (__HAIKU__)
    copy2buf(buf, bufSize, "haiku");
    return 0;
#else
    struct utsname uts;

    if (uname(&uts) < 0) {
        return -1;
    }

    copy2buf(buf, bufSize, uts.sysname);

    for (size_t i = 0; buf[i] != '\0'; i++) {
        if ((buf[i] >= 'A') && (buf[i] <= 'Z')) {
             buf[i] += 32;
        }
    }

    return 0;
#endif
}

int sysinfo_type(char buf[], size_t bufSize) {
#if defined (_WIN32)
    copy2buf(buf, bufSize, "windows");
    return 0;
#elif defined (__APPLE__)
    copy2buf(buf, bufSize, "macos");
    return 0;
#elif defined (__DragonFly__)
    copy2buf(buf, bufSize, "dragonflybsd");
    return 0;
#elif defined (__MidnightBSD__)
    copy2buf(buf, bufSize, "midnightbsd");
    return 0;
#elif defined (__FreeBSD__)
    copy2buf(buf, bufSize, "freebsd");
    return 0;
#elif defined (__OpenBSD__)
    copy2buf(buf, bufSize, "openbsd");
    return 0;
#elif defined (__NetBSD__)
    copy2buf(buf, bufSize, "netbsd");
    return 0;
#elif defined (__ANDROID__)
    copy2buf(buf, bufSize, "android");
    return 0;
#elif defined (__linux__)
    copy2buf(buf, bufSize, "linux");
    return 0;
#elif defined (__HAIKU__)
    copy2buf(buf, bufSize, "haiku");
    return 0;
#else
    struct utsname uts;

    if (uname(&uts) < 0) {
        return -1;
    }

    copy2buf(buf, bufSize, uts.sysname);

    for (size_t i = 0; buf[i] != '\0'; i++) {
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

    copy2buf(buf, bufSize, uts.machine);

    return 0;
}

int sysinfo_code(char * buf, size_t bufSize) {
#if defined (_WIN32)
    copy2buf(buf, bufSize, "windows");
    return 0;
#elif defined (__APPLE__)
    copy2buf(buf, bufSize, "macos");
    return 0;
#elif defined (__DragonFly__)
    copy2buf(buf, bufSize, "dragonflybsd");
    return 0;
#elif defined (__MidnightBSD__)
    copy2buf(buf, bufSize, "midnightbsd");
    return 0;
#elif defined (__FreeBSD__)
    copy2buf(buf, bufSize, "freebsd");
    return 0;
#elif defined (__OpenBSD__)
    copy2buf(buf, bufSize, "openbsd");
    return 0;
#elif defined (__NetBSD__)
    copy2buf(buf, bufSize, "netbsd");
    return 0;
#elif defined (__ANDROID__)
    copy2buf(buf, bufSize, "android");
    return 0;
#elif defined (__HAIKU__)
    copy2buf(buf, bufSize, "haiku");
    return 0;
#else
    const char * const filepath = "/etc/os-release";
    struct stat sb;

    if ((stat(filepath, &sb) == 0) && (S_ISREG(sb.st_mode) || S_ISLNK(sb.st_mode))) {
        FILE * file = fopen(filepath, "r");

        if (file == NULL) {
            perror(filepath);
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

                copy2buf(buf, bufSize, p);

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
    copy2buf(buf, bufSize, "Windows");
    return 0;
#elif defined (__APPLE__)
    copy2buf(buf, bufSize, "macOS");
    return 0;
#elif defined (__DragonFly__)
    copy2buf(buf, bufSize, "DragonFlyBSD");
    return 0;
#elif defined (__MidnightBSD__)
    copy2buf(buf, bufSize, "MidnightBSD");
    return 0;
#elif defined (__FreeBSD__)
    copy2buf(buf, bufSize, "FreeBSD");
    return 0;
#elif defined (__OpenBSD__)
    copy2buf(buf, bufSize, "OpenBSD");
    return 0;
#elif defined (__NetBSD__)
    copy2buf(buf, bufSize, "NetBSD");
    return 0;
#elif defined (__ANDROID__)
    copy2buf(buf, bufSize, "Android");
    return 0;
#elif defined (__HAIKU__)
    copy2buf(buf, bufSize, "Haiku");
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

                copy2buf(buf, bufSize, p);

                fclose(file);

                return 0;
            }
        }
    }

    return -1;
#endif
}

int sysinfo_vers(char * buf, size_t bufSize) {
#if defined (__NetBSD__) || defined (__OpenBSD__) || defined (__HAIKU__)
    struct utsname uts;

    if (uname(&uts) < 0) {
        return -1;
    }

    copy2buf(buf, bufSize, uts.release);

    return 0;
#elif defined (__ANDROID__)
    char buff[PROP_VALUE_MAX];

    int n = __system_property_get("ro.build.version.release", buff);

    if (n > 0) {
        copy2buf(buf, bufSize, buff);
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
            perror(filepath);
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

                copy2buf(buf, bufSize, p);

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
#include "linux/determine-linux-system-libc.c"
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

    char arch[SYSINFO_FIELD_BUFFER_SIZE]; arch[0] = '\0';

    ret = sysinfo_arch(arch, SYSINFO_FIELD_BUFFER_SIZE);

    if (ret != 0) {
        return ret;
    }

    ///////////////////////////////////////

    char kind[SYSINFO_FIELD_BUFFER_SIZE]; kind[0] = '\0';

    ret = sysinfo_kind(kind, SYSINFO_FIELD_BUFFER_SIZE);

    if (ret != 0) {
        return ret;
    }

    ///////////////////////////////////////

    char type[SYSINFO_FIELD_BUFFER_SIZE]; type[0] = '\0';

    ret = sysinfo_type(type, SYSINFO_FIELD_BUFFER_SIZE);

    if (ret != 0) {
        return ret;
    }

    ///////////////////////////////////////

    char code[SYSINFO_FIELD_BUFFER_SIZE]; code[0] = '\0';

    ret = sysinfo_code(code, SYSINFO_FIELD_BUFFER_SIZE);

    if (ret != 0) {
        return ret;
    }

    ///////////////////////////////////////

    char name[SYSINFO_FIELD_BUFFER_SIZE]; name[0] = '\0';

    ret = sysinfo_name(name, SYSINFO_FIELD_BUFFER_SIZE);

    if (ret != 0) {
        return ret;
    }

    ///////////////////////////////////////

    char vers[SYSINFO_FIELD_BUFFER_SIZE]; vers[0] = '\0';

    ret = sysinfo_vers(vers, SYSINFO_FIELD_BUFFER_SIZE);

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

    for (int i = 0; i < SYSINFO_FIELD_BUFFER_SIZE; i++) {
        sysinfo->arch[i] = arch[i];

        if (arch[i] == '\0') {
            break;
        }
    }

    for (int i = 0; i < SYSINFO_FIELD_BUFFER_SIZE; i++) {
        sysinfo->kind[i] = kind[i];

        if (kind[i] == '\0') {
            break;
        }
    }

    for (int i = 0; i < SYSINFO_FIELD_BUFFER_SIZE; i++) {
        sysinfo->type[i] = type[i];

        if (type[i] == '\0') {
            break;
        }
    }

    for (int i = 0; i < SYSINFO_FIELD_BUFFER_SIZE; i++) {
        sysinfo->code[i] = code[i];

        if (code[i] == '\0') {
            break;
        }
    }

    for (int i = 0; i < SYSINFO_FIELD_BUFFER_SIZE; i++) {
        sysinfo->name[i] = name[i];

        if (name[i] == '\0') {
            break;
        }
    }

    for (int i = 0; i < SYSINFO_FIELD_BUFFER_SIZE; i++) {
        sysinfo->vers[i] = vers[i];

        if (vers[i] == '\0') {
            break;
        }
    }

    sysinfo->libc = libc;
    sysinfo->ncpu = ncpu;

    sysinfo->euid = geteuid();
    sysinfo->egid = getegid();

    return 0;
}

void sysinfo_dump(SysInfo * sysinfo) {
    if (sysinfo == NULL) {
        return;
    }

    printf("sysinfo.ncpu: %u\n", sysinfo->ncpu);
    printf("sysinfo.arch: %s\n", sysinfo->arch);
    printf("sysinfo.kind: %s\n", sysinfo->kind);
    printf("sysinfo.type: %s\n", sysinfo->type);
    printf("sysinfo.code: %s\n", sysinfo->code);
    printf("sysinfo.name: %s\n", sysinfo->name);
    printf("sysinfo.vers: %s\n", sysinfo->vers);
    printf("sysinfo.euid: %u\n", sysinfo->euid);
    printf("sysinfo.egid: %u\n", sysinfo->egid);

    switch(sysinfo->libc) {
        case LIBC_GNU:  printf("sysinfo.libc: glibc\n"); break;
        case LIBC_MUSL:  printf("sysinfo.libc: musl\n");  break;
        default: printf("sysinfo.libc: \n");
    }
}


void sysinfo_dump_as_shell_script(SysInfo * sysinfo) {
    if (sysinfo == NULL) {
        return;
    }

    printf("NATIVE_PLATFORM_NCPU='%u'\n", sysinfo->ncpu);
    printf("NATIVE_PLATFORM_ARCH='%s'\n", sysinfo->arch);
    printf("NATIVE_PLATFORM_KIND='%s'\n", sysinfo->kind);
    printf("NATIVE_PLATFORM_TYPE='%s'\n", sysinfo->type);
    printf("NATIVE_PLATFORM_CODE='%s'\n", sysinfo->code);
    printf("NATIVE_PLATFORM_NAME='%s'\n", sysinfo->name);
    printf("NATIVE_PLATFORM_VERS='%s'\n", sysinfo->vers);
    printf("NATIVE_PLATFORM_EUID='%u'\n", sysinfo->euid);
    printf("NATIVE_PLATFORM_EGID='%u'\n", sysinfo->egid);

    switch(sysinfo->libc) {
        case LIBC_GNU:  printf("NATIVE_PLATFORM_LIBC='glibc'\n"); break;
        case LIBC_MUSL: printf("NATIVE_PLATFORM_LIBC='musl'\n");  break;
        default:        printf("NATIVE_PLATFORM_LIBC=\n");
    }
}
