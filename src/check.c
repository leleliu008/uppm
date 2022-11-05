#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "core/fs.h"
#include "core/util.h"
#include "core/regex/regex.h"
#include "uppm.h"

int uppm_is_package_name(const char * packageName) {
    if (packageName == NULL) {
        return UPPM_PACKAGE_NAME_IS_NULL;
    } else if (strcmp(packageName, "") == 0) {
        return UPPM_PACKAGE_NAME_IS_EMPTY;
    } else if (regex_match(packageName, "^[A-Za-z0-9+-._]{1,50}$")) {
        return UPPM_OK;
    } else {
        return UPPM_PACKAGE_NAME_IS_INVALID;
    }
}

int uppm_is_package_available(const char * packageName) {
    int resultCode = uppm_is_package_name(packageName);

    if (resultCode != UPPM_OK) {
        return resultCode;
    }

    char * userHomeDir = getenv("HOME");

    if (userHomeDir == NULL || strcmp(userHomeDir, "") == 0) {
        return UPPM_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    size_t  uppmHomeDirLength = userHomeDirLength + 7; 
    char    uppmHomeDir[uppmHomeDirLength];
    memset (uppmHomeDir, 0, uppmHomeDirLength);
    sprintf(uppmHomeDir, "%s/.uppm", userHomeDir);

    if (!exists_and_is_a_directory(uppmHomeDir)) {
        return UPPM_FORMULA_REPO_NOT_EXIST;
    }

    size_t  reposConfigFilePathLength = userHomeDirLength + 13;
    char    reposConfigFilePath[reposConfigFilePathLength];
    memset (reposConfigFilePath, 0, reposConfigFilePathLength);
    sprintf(reposConfigFilePath, "%s/.uppm/repos", userHomeDir);

    if (exists_and_is_a_regular_file(reposConfigFilePath)) {
        FILE * reposConfigFile = fopen(reposConfigFilePath, "r");

        if (reposConfigFile == NULL) {
            perror(reposConfigFilePath);
            return UPPM_REPOS_CONFIG_READ_ERROR;
        }

        char line[300];
        while(fgets(line, 300, reposConfigFile)) {
            char * formulaRepoName = strtok(line, "=");

            if (formulaRepoName == NULL) {
                fprintf(stderr, "~/.uppm/repos config file syntax error.\n");
                fclose(reposConfigFile);
                return UPPM_REPOS_CONFIG_READ_ERROR;
            }

            size_t  formulaRepoPathLength = userHomeDirLength + 30 + strlen(formulaRepoName) + strlen(packageName);
            char    formulaRepoPath[formulaRepoPathLength];
            memset (formulaRepoPath, 0, formulaRepoPathLength);
            sprintf(formulaRepoPath, "%s/.uppm/repos.d/%s/formula/%s.yml", userHomeDir, formulaRepoName, packageName);

            if (exists_and_is_a_regular_file(formulaRepoPath)) {
                return UPPM_OK;
            }
        }
    } else {
        size_t  formulaRepoPathLength = userHomeDirLength + 36 + strlen(packageName);
        char    formulaRepoPath[formulaRepoPathLength];
        memset (formulaRepoPath, 0, formulaRepoPathLength);
        sprintf(formulaRepoPath, "%s/.uppm/repos.d/offical/formula/%s.yml", userHomeDir, packageName);

        if (exists_and_is_a_regular_file(formulaRepoPath)) {
            return UPPM_OK;
        }
    }

    return UPPM_PACKAGE_IS_NOT_AVAILABLE;
}

int uppm_is_package_installed(const char * packageName) {
    int resultCode = uppm_is_package_name(packageName);

    if (resultCode != UPPM_OK) {
        return resultCode;
    }

    char * userHomeDir = getenv("HOME");

    if (userHomeDir == NULL || strcmp(userHomeDir, "") == 0) {
        return UPPM_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    size_t  uppmHomeDirLength = userHomeDirLength + 7; 
    char    uppmHomeDir[uppmHomeDirLength];
    memset (uppmHomeDir, 0, uppmHomeDirLength);
    sprintf(uppmHomeDir, "%s/.uppm", userHomeDir);

    if (!exists_and_is_a_directory(uppmHomeDir)) {
        return UPPM_PACKAGE_IS_NOT_INSTALLED;
    }

    size_t  receiptFilePathLength = uppmHomeDirLength + strlen(packageName) + 30;
    char    receiptFilePath[receiptFilePathLength];
    memset (receiptFilePath, 0, receiptFilePathLength);
    sprintf(receiptFilePath, "%s/installed/%s/.uppm/receipt.yml", uppmHomeDir, packageName);

    if (exists_and_is_a_regular_file(receiptFilePath)) {
        return UPPM_OK;
    } else {
        return UPPM_PACKAGE_IS_NOT_INSTALLED;
    }
}

int uppm_is_package_outdated(const char * packageName) {
    int resultCode = uppm_is_package_name(packageName);

    if (resultCode != UPPM_OK) {
        return resultCode;
    }

    return 0;
}
