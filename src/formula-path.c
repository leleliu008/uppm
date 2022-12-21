#include <stdio.h>
#include <string.h>

#include "core/fs.h"
#include "uppm.h"

int uppm_formula_path(const char * packageName, char ** out) {
    int resultCode = uppm_check_if_the_given_argument_matches_package_name_pattern(packageName);

    if (resultCode != UPPM_OK) {
        return resultCode;
    }

    char * userHomeDir = getenv("HOME");

    if (userHomeDir == NULL) {
        return UPPM_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    if (userHomeDirLength == 0) {
        return UPPM_ENV_HOME_NOT_SET;
    }

    size_t  uppmHomeDirLength = userHomeDirLength + 7; 
    char    uppmHomeDir[uppmHomeDirLength];
    memset (uppmHomeDir, 0, uppmHomeDirLength);
    sprintf(uppmHomeDir, "%s/.uppm", userHomeDir);

    if (!exists_and_is_a_directory(uppmHomeDir)) {
        return UPPM_FORMULA_REPO_NOT_EXIST;
    }

    UPPMFormulaRepoList * formulaRepoList = NULL;

    resultCode = uppm_formula_repo_list_new(&formulaRepoList);

    if (resultCode != UPPM_OK) {
        uppm_formula_repo_list_free(formulaRepoList);
        return resultCode;
    }

    for (size_t i = 0; i < formulaRepoList->size; i++) {
        char *  formulaRepoPath = formulaRepoList->repos[i]->path;
        size_t  formulaFilePathLength =  strlen(formulaRepoPath) + strlen(packageName) + 15;
        char *  formulaFilePath = (char*)calloc(formulaFilePathLength, sizeof(char));
        sprintf(formulaFilePath, "%s/formula/%s.yml", formulaRepoPath, packageName);

        if (exists_and_is_a_regular_file(formulaFilePath)) {
            (*out) = formulaFilePath;
            uppm_formula_repo_list_free(formulaRepoList);
            return UPPM_OK;
        } else {
            free(formulaFilePath);
            formulaFilePath = NULL;
        }
    }

    uppm_formula_repo_list_free(formulaRepoList);
    return UPPM_PACKAGE_IS_NOT_AVAILABLE;
}
