#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <limits.h>
#include <sys/stat.h>

#include "uppm.h"

int uppm_integrate_zsh_completion(const char * outputDIR, const bool verbose) {
    char   uppmHomeDIR[PATH_MAX] = {0};
    size_t uppmHomeDIRLength;

    int ret = uppm_home_dir(uppmHomeDIR, PATH_MAX, &uppmHomeDIRLength);

    if (ret != UPPM_OK) {
        return ret;
    }

    ///////////////////////////////////////////////////////////////////////////////////////

    struct stat st;

    size_t uppmRunDIRCapacity = uppmHomeDIRLength + 5U;
    char   uppmRunDIR[uppmRunDIRCapacity];

    ret = snprintf(uppmRunDIR, uppmRunDIRCapacity, "%s/run", uppmHomeDIR);

    if (ret < 0) {
        perror(NULL);
        return UPPM_ERROR;
    }

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

    size_t sessionDIRCapacity = uppmRunDIRCapacity + 20U;
    char   sessionDIR[sessionDIRCapacity];

    ret = snprintf(sessionDIR, sessionDIRCapacity, "%s/%d", uppmRunDIR, getpid());

    if (ret < 0) {
        perror(NULL);
        return UPPM_ERROR;
    }

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

    size_t tmpFilePathCapacity = sessionDIRCapacity + 7U;
    char   tmpFilePath[tmpFilePathCapacity];

    ret = snprintf(tmpFilePath, tmpFilePathCapacity, "%s/_uppm", sessionDIR);

    if (ret < 0) {
        perror(NULL);
        return UPPM_ERROR;
    }

    const char * const url = "https://raw.githubusercontent.com/leleliu008/uppm/master/uppm-zsh-completion";

    ret = uppm_http_fetch_to_file(url, tmpFilePath, verbose, verbose);

    if (ret != UPPM_OK) {
        return ret;
    }

    ////////////////////////////////////////////////////////////////////////////////////////

    size_t defaultOutputDIRCapacity = uppmHomeDIRLength + 26U;
    char   defaultOutputDIR[defaultOutputDIRCapacity];

    ret = snprintf(defaultOutputDIR, defaultOutputDIRCapacity, "%s/share/zsh/site-functions", uppmHomeDIR);

    if (ret < 0) {
        perror(NULL);
        return UPPM_ERROR;
    }

    size_t outputDIRLength;

    if (outputDIR == NULL) {
        outputDIR       = defaultOutputDIR;
        outputDIRLength = ret;
    } else {
        outputDIRLength = strlen(outputDIR);
    }

    ////////////////////////////////////////////////////////////////////////////////////////

    ret = uppm_mkdir_p(outputDIR, verbose);

    if (ret != UPPM_OK) {
        return ret;
    }

    ////////////////////////////////////////////////////////////////////////////////////////

    size_t outputFilePathCapacity = outputDIRLength + 7U;
    char   outputFilePath[outputFilePathCapacity];

    ret = snprintf(outputFilePath, outputFilePathCapacity, "%s/_uppm", outputDIR);

    if (ret < 0) {
        perror(NULL);
        return UPPM_ERROR;
    }

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

int uppm_integrate_bash_completion(const char * outputDIR, const bool verbose) {
    (void)outputDIR;
    (void)verbose;
    return UPPM_OK;
}

int uppm_integrate_fish_completion(const char * outputDIR, const bool verbose) {
    (void)outputDIR;
    (void)verbose;
    return UPPM_OK;
}
