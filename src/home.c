#include <errno.h>
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

    const char * const uppmHomeDIR = getenv("UPPM_HOME");

    if (uppmHomeDIR == NULL) {
        const char * const userHomeDIR = getenv("HOME");

        if (userHomeDIR == NULL) {
            return UPPM_ERROR_ENV_HOME_NOT_SET;
        }

        if (userHomeDIR[0] == '\0') {
            return UPPM_ERROR_ENV_HOME_NOT_SET;
        }

        size_t   defaultUppmHomeDIRLength = strlen(userHomeDIR) + 6U;
        char     defaultUppmHomeDIR[defaultUppmHomeDIRLength + 1U];
        snprintf(defaultUppmHomeDIR, defaultUppmHomeDIRLength + 1U, "%s/.uppm", userHomeDIR);

        struct stat st;

        if (stat(defaultUppmHomeDIR, &st) == 0) {
            if (!S_ISDIR(st.st_mode)) {
                fprintf(stderr, "%s was expected to be a directory, but it was not.\n", defaultUppmHomeDIR);
                return UPPM_ERROR;
            }
        } else {
            if (mkdir(defaultUppmHomeDIR, S_IRWXU) != 0) {
                if (errno != EEXIST) {
                    perror(defaultUppmHomeDIR);
                    return UPPM_ERROR;
                }
            }
        }

        size_t n = (bufSize > defaultUppmHomeDIRLength) ? defaultUppmHomeDIRLength : bufSize;

        strncpy(buf, defaultUppmHomeDIR, n);

        if (outSize != NULL) {
            (*outSize) = n;
        }
    } else {
        if (uppmHomeDIR[0] == '\0') {
            fprintf(stderr, "'UPPM_HOME' environment variable's value was expected to be a non-empty string, but it was not.\n");
            return UPPM_ERROR;
        }

        struct stat st;

        if (stat(uppmHomeDIR, &st) == 0) {
            if (!S_ISDIR(st.st_mode)) {
                fprintf(stderr, "%s was expected to be a directory, but it was not.\n", uppmHomeDIR);
                return UPPM_ERROR;
            }
        } else {
            if (mkdir(uppmHomeDIR, S_IRWXU) != 0) {
                if (errno != EEXIST) {
                    perror(uppmHomeDIR);
                    return UPPM_ERROR;
                }
            }
        }

        size_t uppmHomeDIRLength = strlen(uppmHomeDIR);

        size_t n = (bufSize > uppmHomeDIRLength) ? uppmHomeDIRLength : bufSize;

        strncpy(buf, uppmHomeDIR, n);

        if (outSize != NULL) {
            (*outSize) = n;
        }
    }

    return UPPM_OK;
}
