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

    UPPMFormulaRepoList * formulaRepoList = NULL;

    resultCode = uppm_formula_repo_list_new(&formulaRepoList);

    if (resultCode != UPPM_OK) {
        uppm_formula_repo_list_free(formulaRepoList);
        return resultCode;
    }

    for (size_t i = 0; i < formulaRepoList->size; i++) {
        char *  formulaRepoPath = formulaRepoList->repos[i]->path;
        size_t  formulaFilePathLength = strlen(formulaRepoPath) + strlen(packageName) + 15;
        char    formulaFilePath[formulaFilePathLength];
        memset (formulaFilePath, 0, formulaFilePathLength);
        sprintf(formulaFilePath, "%s/formula/%s.yml", formulaRepoPath, packageName);

        if (exists_and_is_a_regular_file(formulaFilePath)) {
            uppm_formula_repo_list_free(formulaRepoList);
            return UPPM_OK;
        }
    }

    uppm_formula_repo_list_free(formulaRepoList);
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
