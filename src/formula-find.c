#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "uppm.h"

int uppm_formula_find(const char * packageName, char ** out) {
    int resultCode = uppm_check_if_the_given_argument_matches_package_name_pattern(packageName);

    if (resultCode != UPPM_OK) {
        return resultCode;
    }

    char * userHomeDir = getenv("HOME");

    if (userHomeDir == NULL) {
        return UPPM_ERROR_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    if (userHomeDirLength == 0) {
        return UPPM_ERROR_ENV_HOME_NOT_SET;
    }

    UPPMFormulaRepoList * formulaRepoList = NULL;

    resultCode = uppm_formula_repo_list_new(&formulaRepoList);

    if (resultCode != UPPM_OK) {
        uppm_formula_repo_list_free(formulaRepoList);
        return resultCode;
    }

    struct stat st;

    for (size_t i = 0; i < formulaRepoList->size; i++) {
        if (!(formulaRepoList->repos[i]->enabled)) {
            continue;
        }

        char *  formulaRepoPath = formulaRepoList->repos[i]->path;

        size_t formulaFilePathLength =  strlen(formulaRepoPath) + strlen(packageName) + 15;
        char   formulaFilePath[formulaFilePathLength];
        memset(formulaFilePath, 0, formulaFilePathLength);
        snprintf(formulaFilePath, formulaFilePathLength, "%s/formula/%s.yml", formulaRepoPath, packageName);

        if (stat(formulaFilePath, &st) == 0 && S_ISREG(st.st_mode)) {
            uppm_formula_repo_list_free(formulaRepoList);

            (*out) = strdup(formulaFilePath);

            if (*out == NULL) {
                return UPPM_ERROR_MEMORY_ALLOCATE;
            } else {
                return UPPM_OK;
            }
        }
    }

    uppm_formula_repo_list_free(formulaRepoList);
    return UPPM_ERROR_PACKAGE_NOT_AVAILABLE;
}
