#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <limits.h>
#include <sys/stat.h>

#include "uppm.h"

int uppm_integrate_zsh_completion(const char * outputDIR, bool verbose) {
    char   uppmHomeDIR[PATH_MAX];
    size_t uppmHomeDIRLength;

    int ret = uppm_home_dir(uppmHomeDIR, PATH_MAX, &uppmHomeDIRLength);

    if (ret != UPPM_OK) {
        return ret;
    }

    ///////////////////////////////////////////////////////////////////////////////////////

    struct stat st;

    size_t   uppmRunDIRLength = uppmHomeDIRLength + 5U;
    char     uppmRunDIR[uppmRunDIRLength];
    snprintf(uppmRunDIR, uppmRunDIRLength, "%s/run", uppmHomeDIR);

    if (lstat(uppmRunDIR, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            if (unlink(uppmRunDIR) != 0) {
                perror(uppmRunDIR);
                return UPPM_ERROR;
            }

            if (mkdir(uppmRunDIR, S_IRWXU) != 0) {
                if (errno != EEXIST) {
                    perror(uppmRunDIR);
                    return UPPM_ERROR;
                }
            }
        }
    } else {
        if (mkdir(uppmRunDIR, S_IRWXU) != 0) {
            if (errno != EEXIST) {
                perror(uppmRunDIR);
                return UPPM_ERROR;
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    size_t   sessionDIRLength = uppmRunDIRLength + 20U;
    char     sessionDIR[sessionDIRLength];
    snprintf(sessionDIR, sessionDIRLength, "%s/%d", uppmRunDIR, getpid());

    if (lstat(sessionDIR, &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            ret = uppm_rm_r(sessionDIR, false);

            if (ret != UPPM_OK) {
                return ret;
            }

            if (mkdir(sessionDIR, S_IRWXU) != 0) {
                perror(sessionDIR);
                return UPPM_ERROR;
            }
        } else {
            if (unlink(sessionDIR) != 0) {
                perror(sessionDIR);
                return UPPM_ERROR;
            }

            if (mkdir(sessionDIR, S_IRWXU) != 0) {
                perror(sessionDIR);
                return UPPM_ERROR;
            }
        }
    } else {
        if (mkdir(sessionDIR, S_IRWXU) != 0) {
            perror(sessionDIR);
            return UPPM_ERROR;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////

    size_t   tmpFilePathLength = sessionDIRLength + 7U;
    char     tmpFilePath[tmpFilePathLength];
    snprintf(tmpFilePath, tmpFilePathLength, "%s/_uppm", sessionDIR);

    const char * const url = "https://raw.githubusercontent.com/leleliu008/uppm/master/uppm-zsh-completion";

    ret = uppm_http_fetch_to_file(url, tmpFilePath, verbose, verbose);

    if (ret != UPPM_OK) {
        return ret;
    }

    ////////////////////////////////////////////////////////////////////////////////////////

    size_t   defaultOutputDIRLength = uppmHomeDIRLength + 26U;
    char     defaultOutputDIR[defaultOutputDIRLength];
    snprintf(defaultOutputDIR, defaultOutputDIRLength, "%s/share/zsh/site-functions", uppmHomeDIR);

    size_t outputDIRLength;

    if (outputDIR == NULL) {
        outputDIR       = defaultOutputDIR;
        outputDIRLength = defaultOutputDIRLength;
    } else {
        outputDIRLength = strlen(outputDIR);
    }

    ////////////////////////////////////////////////////////////////////////////////////////

    ret = uppm_mkdir_p(outputDIR, verbose);

    if (ret != UPPM_OK) {
        return ret;
    }

    ////////////////////////////////////////////////////////////////////////////////////////

    size_t   outputFilePathLength = outputDIRLength + 7U;
    char     outputFilePath[outputFilePathLength];
    snprintf(outputFilePath, outputFilePathLength, "%s/_uppm", outputDIR);

    if (rename(tmpFilePath, outputFilePath) != 0) {
        if (errno == EXDEV) {
            ret = uppm_copy_file(tmpFilePath, outputFilePath);

            if (ret != UPPM_OK) {
                return ret;
            }
        } else {
            perror(outputFilePath);
            return UPPM_ERROR;
        }
    }

    printf("zsh completion script for uppm has been written to %s\n", outputFilePath);
    return UPPM_OK;
}

int uppm_integrate_bash_completion(const char * outputDIR, bool verbose) {
    (void)outputDIR;
    (void)verbose;
    return UPPM_OK;
}

int uppm_integrate_fish_completion(const char * outputDIR, bool verbose) {
    (void)outputDIR;
    (void)verbose;
    return UPPM_OK;
}
