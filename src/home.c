#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "uppm.h"

int uppm_home_dir(char buf[], size_t bufSize, size_t * outSize) {
    if (buf == NULL) {
        return UPPM_ERROR_ARG_IS_NULL;
    }

    if (bufSize == 0U) {
        return UPPM_ERROR_ARG_IS_INVALID;
    }

    const char * const uppmHomeDir = getenv("UPPM_HOME");

    if (uppmHomeDir == NULL) {
        const char * const userHomeDir = getenv("HOME");

        if (userHomeDir == NULL) {
            return UPPM_ERROR_ENV_HOME_NOT_SET;
        }

        size_t userHomeDirLength = strlen(userHomeDir);

        if (userHomeDirLength == 0U) {
            return UPPM_ERROR_ENV_HOME_NOT_SET;
        }

        size_t   uppmHomeDirLength = userHomeDirLength + + 7U;
        char     uppmHomeDir[uppmHomeDirLength];
        snprintf(uppmHomeDir, uppmHomeDirLength, "%s/.uppm", userHomeDir);

        struct stat st;

        if (stat(uppmHomeDir, &st) == 0) {
            if (!S_ISDIR(st.st_mode)) {
                fprintf(stderr, "'%s\n' was expected to be a directory, but it was not.\n", uppmHomeDir);
                return UPPM_ERROR;
            }
        } else {
            if (mkdir(uppmHomeDir, S_IRWXU) != 0) {
                perror(uppmHomeDir);
                return UPPM_ERROR;
            }
        }

        size_t n = (bufSize > uppmHomeDirLength) ? uppmHomeDirLength : bufSize;

        strncpy(buf, uppmHomeDir, n);

        if (outSize != NULL) {
            (*outSize) = n;
        }
    } else {
        if (uppmHomeDir[0] == '\0') {
            fprintf(stderr, "'UPPM_HOME' environment variable's value was expected to be a non-empty string, but it was not.\n");
            return UPPM_ERROR;
        }

        struct stat st;

        if (stat(uppmHomeDir, &st) == 0) {
            if (!S_ISDIR(st.st_mode)) {
                fprintf(stderr, "'%s\n' was expected to be a directory, but it was not.\n", uppmHomeDir);
                return UPPM_ERROR;
            }
        } else {
            if (mkdir(uppmHomeDir, S_IRWXU) != 0) {
                perror(uppmHomeDir);
                return UPPM_ERROR;
            }
        }

        size_t uppmHomeDirLength = strlen(uppmHomeDir);

        size_t n = (bufSize > uppmHomeDirLength) ? uppmHomeDirLength : bufSize;

        strncpy(buf, uppmHomeDir, n);

        if (outSize != NULL) {
            (*outSize) = n;
        }
    }

    return UPPM_OK;
}
