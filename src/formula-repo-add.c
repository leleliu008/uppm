#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include "core/rm-r.h"
#include "core/log.h"

#include "uppm.h"

int uppm_formula_repo_add(const char * formulaRepoName, const char * formulaRepoUrl, const char * branchName, int pinned, int enabled) {
    if (formulaRepoName == NULL) {
        return UPPM_ERROR_ARG_IS_NULL;
    }

    size_t formulaRepoNameLength = strlen(formulaRepoName);

    if (formulaRepoNameLength == 0U) {
        return UPPM_ERROR_ARG_IS_EMPTY;
    }

    if (formulaRepoUrl == NULL) {
        return UPPM_ERROR_ARG_IS_NULL;
    }

    size_t formulaRepoUrlLength = strlen(formulaRepoUrl);

    if (formulaRepoUrlLength == 0U) {
        return UPPM_ERROR_ARG_IS_EMPTY;
    }

    if (branchName == NULL || branchName[0] == '\0') {
        branchName = (char*)"master";
    }

    size_t branchNameLength = strlen(branchName);

    ///////////////////////////////////////////////////////////////////////////////////////

    char   uppmHomeDIR[256] = {0};
    size_t uppmHomeDIRLength;

    int ret = uppm_home_dir(uppmHomeDIR, 255, &uppmHomeDIRLength);

    if (ret != UPPM_OK) {
        return ret;
    }

    ////////////////////////////////////////////////////////////////////////////////////////

    struct stat st;

    size_t   formulaRepoDIRLength = uppmHomeDIRLength + formulaRepoNameLength + 10U;
    char     formulaRepoDIR[formulaRepoDIRLength];
    snprintf(formulaRepoDIR, formulaRepoDIRLength, "%s/repos.d/%s", uppmHomeDIR, formulaRepoName);

    if (stat(formulaRepoDIR, &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            fprintf(stderr, "formula repo '%s' already exist.", formulaRepoName);
            return UPPM_ERROR_FORMULA_REPO_HAS_EXIST;
        } else {
            fprintf(stderr, "'%s\n' was expected to be a directory, but it was not.\n", formulaRepoDIR);
            return UPPM_ERROR;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////

    size_t   uppmRunDIRLength = uppmHomeDIRLength + 5U;
    char     uppmRunDIR[uppmRunDIRLength];
    snprintf(uppmRunDIR, uppmRunDIRLength, "%s/run", uppmHomeDIR);

    if (stat(uppmRunDIR, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            fprintf(stderr, "'%s\n' was expected to be a directory, but it was not.\n", uppmRunDIR);
            return UPPM_ERROR;
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

    if (stat(sessionDIR, &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            if (rm_r(sessionDIR, false) != 0) {
                perror(sessionDIR);
                return UPPM_ERROR;
            }
        } else {
            fprintf(stderr, "'%s\n' was expected to be a directory, but it was not.\n", sessionDIR);
            return UPPM_ERROR;
        }
    } else {
        if (mkdir(sessionDIR, S_IRWXU) != 0) {
            perror(sessionDIR);
            return UPPM_ERROR;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////

    printf("Adding formula repo : %s => %s\n", formulaRepoName, formulaRepoUrl);

    size_t   remoteRefPathLength = branchNameLength + 12U;
    char     remoteRefPath[remoteRefPathLength];
    snprintf(remoteRefPath, remoteRefPathLength, "refs/heads/%s", branchName);

    size_t   remoteTrackingRefPathLength = branchNameLength + 21U;
    char     remoteTrackingRefPath[remoteTrackingRefPathLength];
    snprintf(remoteTrackingRefPath, remoteTrackingRefPathLength, "refs/remotes/origin/%s", branchName);

    ret = uppm_git_sync(sessionDIR, formulaRepoUrl, remoteRefPath, remoteTrackingRefPath, branchName);

    if (ret != UPPM_OK) {
        return ret;
    }

    ////////////////////////////////////////////////////////////////////////////////////////

    char ts[11];
    snprintf(ts, 11, "%ld", time(NULL));

    ret = uppm_formula_repo_config_write(sessionDIR, formulaRepoUrl, branchName, pinned, enabled, ts, NULL);

    if (ret != UPPM_OK) {
        return ret;
    }

    ////////////////////////////////////////////////////////////////////////////////////////

    size_t   formulaRepoRootDIRLength = uppmHomeDIRLength + 9U;
    char     formulaRepoRootDIR[formulaRepoRootDIRLength];
    snprintf(formulaRepoRootDIR, formulaRepoRootDIRLength, "%s/repos.d", uppmHomeDIR);

    if (stat(formulaRepoRootDIR, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            fprintf(stderr, "'%s\n' was expected to be a directory, but it was not.\n", formulaRepoRootDIR);
            return UPPM_ERROR;
        }
    } else {
        if (mkdir(formulaRepoRootDIR, S_IRWXU) != 0) {
            if (errno != EEXIST) {
                perror(formulaRepoRootDIR);
                return UPPM_ERROR;
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////

    if (rename(sessionDIR, formulaRepoDIR) == 0) {
        return UPPM_OK;
    } else {
        perror(sessionDIR);
        return UPPM_ERROR;
    }
}
