#include <regex.h>
#include "regex/regex.h"
#include "find-executables.h"
#include "util.h"
#include "../uppm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>

#if defined (__APPLE__)
#include <sys/syslimits.h>
#elif defined (__linux__) && defined (HAVE_LINUX_LIMITS_H)
#include <linux/limits.h>
#else
#include <limits.h>
#endif

#if defined (__FreeBSD__) || defined (__OpenBSD__)
#include <sys/types.h>
#include <sys/sysctl.h>
#endif

int get_file_extension_from_url(char * buf, size_t bufSize, const char * url) {
    if (url == NULL) {
        return UPPM_ERROR_ARG_IS_NULL;
    }

    if (buf == NULL) {
        return UPPM_ERROR_ARG_IS_NULL;
    }

    if (bufSize == 0) {
        return UPPM_ERROR_ARG_IS_INVALID;
    }

    size_t urlLength = 0;
    char c;

    for (;;) {
        c = url[urlLength];

        if (c == '?' || c == '\0') {
            break;
        } else {
            urlLength++;
        }
    }

    //printf("url=%s\nurlLength=%lu\n", url, urlLength);

    if (urlLength < 3) {
        return UPPM_ERROR_ARG_IS_INVALID;
    }

    size_t lastIndex = urlLength - 1;

    if (url[lastIndex] == '.') {
        return UPPM_ERROR_ARG_IS_INVALID;
    }

    size_t i = lastIndex;

    const char * p;

    for (; i >= 0; i--) {
        c = url[i];

        if (c == '.') {
            p = &url[i];

            if (urlLength - i == 3) {
                if (strcmp(p, ".gz") == 0) {
                    if (urlLength > 7) {
                        if (strncmp(&url[i - 4], ".tar", 4) == 0) {
                            strncpy(buf, ".tgz", bufSize > 4 ? 4 : bufSize);
                            return UPPM_OK;
                        }
                    }
                } else if (strcmp(p, ".xz") == 0) {
                    if (urlLength > 7) {
                        if (strncmp(&url[i - 4], ".tar", 4) == 0) {
                            strncpy(buf, ".txz", bufSize > 4 ? 4 : bufSize);
                            return UPPM_OK;
                        }
                    }
                } else if (strcmp(p, ".lz") == 0) {
                    if (urlLength > 7) {
                        if (strncmp(&url[i - 4], ".tar", 4) == 0) {
                            strncpy(buf, ".tlz", bufSize > 4 ? 4 : bufSize);
                            return UPPM_OK;
                        }
                    }
                }
            } else if (urlLength - i == 4) {
                if (strcmp(p, ".bz2") == 0) {
                    if (urlLength > 8) {
                        if (strncmp(&url[i - 4], ".tar", 4) == 0) {
                            strncpy(buf, ".tbz2", bufSize > 5 ? 5 : bufSize);
                            return UPPM_OK;
                        }
                    }
                }
            }

            size_t n = urlLength - i;
            strncpy(buf, p, bufSize > n ? n : bufSize);
            return UPPM_OK;
        }

        if (i == 0) {
            break;
        }
    }

    return UPPM_ERROR_ARG_IS_INVALID;
}

int get_current_executable_realpath(char * * out) {
#if defined (__APPLE__)
    char buf[PATH_MAX + 1] = {0};

    uint32_t bufSize = 0U;
    _NSGetExecutablePath(NULL, &bufSize);

    char path[bufSize];
    _NSGetExecutablePath(path, &bufSize);

    if (realpath(path, buf) == NULL) {
        (*out) = NULL;
        return UPPM_ERROR;
    }

    (*out) = strdup(buf);

    return (*out) == NULL ? -1 : 0;
#elif defined (__FreeBSD__)
    const int mib[] = { CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1 };

    size_t bufLength = 0;

    if (sysctl(mib, 4, NULL, &bufLength, NULL, 0) < 0) {
        return UPPM_ERROR;
    }

    char * buf = (char*)calloc(bufLength + 1, sizeof(char));

    if (buf == NULL) {
        return UPPM_ERROR_MEMORY_ALLOCATE;
    }

    if (sysctl(mib, 4, buf, &bufLength, NULL, 0) < 0) {
        return UPPM_ERROR;
    }

    (*out) = buf;
    return UPPM_OK;
#elif defined (__OpenBSD__)
    const int mib[4] = { CTL_KERN, KERN_PROC_ARGS, getpid(), KERN_PROC_ARGV };
    size_t size;

    if (sysctl(mib, 4, NULL, &size, NULL, 0) != 0) {
        return UPPM_ERROR;
    }

    char** argv = (char**)malloc(size);

    if (argv == NULL) {
        return UPPM_ERROR_MEMORY_ALLOCATE;
    }

    memset(argv, 0, size);

    if (sysctl(mib, 4, argv, &size, NULL, 0) != 0) {
        return UPPM_ERROR;
    }

    bool isPath = false;

    char c;

    char * p = argv[0];

    for (;;) {
        c = p[0];

        if (c == '\0') {
            break;
        }

        if (c == '/') {
            isPath = true;
            break;
        }

        p++;
    }

    if (isPath) {
        (*out) = realpath(argv[0], NULL);
        return (*out) == NULL ? UPPM_ERROR : UPPM_OK;
    } else {
        char * PATH = getenv("PATH");

        if ((PATH == NULL) || (strcmp(PATH, "") == 0)) {
            return UPPM_ERROR_ENV_PATH_NOT_SET;
        }

        size_t PATHLength = strlen(PATH);
        size_t PATH2Length = PATHLength + 1;
        char   PATH2[PATH2Length];
        memset(PATH2, 0, PATH2Length);
        strncpy(PATH2, PATH, PATHLength);

        size_t commandNameLength = strlen(argv[0]);

        char * PATHItem = strtok(PATH2, ":");

        while (PATHItem != NULL) {
            struct stat st;

            if ((stat(PATHItem, &st) == 0) && S_ISDIR(st.st_mode)) {
                size_t  fullPathLength = strlen(PATHItem) + commandNameLength + 2;
                char    fullPath[fullPathLength];
                memset( fullPath, 0, fullPathLength);
                snprintf(fullPath, fullPathLength, "%s/%s", PATHItem, argv[0]);

                if (access(fullPath, X_OK) == 0) {
                    (*out) = strdup(fullPath);
                    return (*out) == NULL ? UPPM_ERROR_MEMORY_ALLOCATE : UPPM_OK;
                }
            }

            PATHItem = strtok(NULL, ":");
        }

        return UPPM_ERROR;
    }
#else
    char buf[PATH_MAX + 1] = {0};

    int ret = readlink("/proc/self/exe", buf, PATH_MAX);

    if (ret == -1) {
        perror("/proc/self/exe");
        return UPPM_ERROR;
    }

    (*out) = strdup(buf);
    return (*out) == NULL ? UPPM_ERROR_MEMORY_ALLOCATE : UPPM_OK;
#endif
}
