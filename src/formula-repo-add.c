#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "uppm.h"
#include "core/zlib-flate.h"

int uppm_formula_repo_add(const char * formulaRepoName, const char * formulaRepoUrl, const char * branchName) {
    if (formulaRepoName == NULL) {
        return UPPM_ARG_IS_NULL;
    }

    size_t formulaRepoNameLength = strlen(formulaRepoName);

    if (formulaRepoNameLength == 0) {
        return UPPM_ARG_IS_EMPTY;
    }

    if (formulaRepoUrl == NULL) {
        return UPPM_ARG_IS_NULL;
    }

    size_t formulaRepoUrlLength = strlen(formulaRepoUrl);

    if (formulaRepoUrlLength == 0) {
        return UPPM_ARG_IS_EMPTY;
    }

    if (branchName == NULL || strcmp(branchName, "") == 0) {
        branchName = (char*)"master";
    }

    size_t branchNameLength = strlen(branchName);

    ///////////////////////////////////////////////////////////////////////////////////////

    char * userHomeDir = getenv("HOME");

    if (userHomeDir == NULL) {
        return UPPM_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    if (userHomeDirLength == 0) {
        return UPPM_ENV_HOME_NOT_SET;
    }

    ////////////////////////////////////////////////////////////////////////////////////////

    struct stat st;

    size_t  uppmHomeDirLength = userHomeDirLength + 7;
    char    uppmHomeDir[uppmHomeDirLength];
    memset (uppmHomeDir, 0, uppmHomeDirLength);
    snprintf(uppmHomeDir, uppmHomeDirLength, "%s/.uppm", userHomeDir);

    if (stat(uppmHomeDir, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            fprintf(stderr, "not a directory: %s\n", uppmHomeDir);
            return UPPM_ERROR;
        }
    } else {
        if (mkdir(uppmHomeDir, S_IRWXU) != 0) {
            perror(uppmHomeDir);
            return UPPM_ERROR;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////

    size_t  formulaRepoRootDirLength = uppmHomeDirLength + 9;
    char    formulaRepoRootDir[formulaRepoRootDirLength];
    memset (formulaRepoRootDir, 0, formulaRepoRootDirLength);
    snprintf(formulaRepoRootDir, formulaRepoRootDirLength, "%s/repos.d", uppmHomeDir);

    if (stat(formulaRepoRootDir, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            fprintf(stderr, "not a directory: %s\n", formulaRepoRootDir);
            return UPPM_ERROR;
        }
    } else {
        if (mkdir(formulaRepoRootDir, S_IRWXU) != 0) {
            perror(formulaRepoRootDir);
            return UPPM_ERROR;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////

    size_t  formulaRepoDirLength = formulaRepoRootDirLength + formulaRepoNameLength + 2;
    char    formulaRepoDir[formulaRepoDirLength];
    memset (formulaRepoDir, 0, formulaRepoDirLength);
    snprintf(formulaRepoDir, formulaRepoDirLength, "%s/%s", formulaRepoRootDir, formulaRepoName);

    if (stat(formulaRepoDir, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            fprintf(stderr, "not a directory: %s\n", formulaRepoDir);
            return UPPM_ERROR;
        }
    } else {
        if (mkdir(formulaRepoDir, S_IRWXU) != 0) {
            perror(formulaRepoDir);
            return UPPM_ERROR;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////

    printf("Adding formula repo : %s => %s\n", formulaRepoName, formulaRepoUrl);

    size_t  refspecLength = (branchNameLength << 1) + 33;
    char    refspec[refspecLength];
    memset (refspec, 0, refspecLength);
    snprintf(refspec, refspecLength, "refs/heads/%s:refs/remotes/origin/%s", branchName, branchName);

    if (uppm_fetch_via_git(formulaRepoDir, formulaRepoUrl, refspec, branchName) != 0) {
        return UPPM_ERROR;
    }

    size_t formulaRepoConfigFilePathLength = formulaRepoDirLength + 24;
    char   formulaRepoConfigFilePath[formulaRepoConfigFilePathLength];
    memset(formulaRepoConfigFilePath, 0, formulaRepoConfigFilePathLength);
    snprintf(formulaRepoConfigFilePath, formulaRepoConfigFilePathLength, "%s/.uppm-formula-repo.dat", formulaRepoDir);

    FILE * file = fopen(formulaRepoConfigFilePath, "wb");

    if (file == NULL) {
        perror(formulaRepoConfigFilePath);
        return UPPM_ERROR;
    }

    char ts[11];
    memset(ts, 0, 11);
    snprintf(ts, 11, "%ld", time(NULL));

    size_t  strLength = formulaRepoUrlLength + branchNameLength + strlen(ts) + 58;
    char    str[strLength];
    memset (str, 0, strLength);
    snprintf(str, strLength, "url: %s\nbranch: %s\npinned: no\nenabled: yes\ntimestamp-added: %s\n", formulaRepoUrl, branchName, ts);

    if (zlib_deflate_string_to_file(str, strLength - 1, file) != 0) {
        fclose(file);

        if (unlink(formulaRepoConfigFilePath) != 0) {
            perror(formulaRepoConfigFilePath);
        }

        return UPPM_ERROR;
    } else {
        fclose(file);
        return UPPM_OK;
    }
}
