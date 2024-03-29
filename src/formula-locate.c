#include <stdio.h>
#include <string.h>

#include <sys/stat.h>

#include "uppm.h"

int uppm_formula_locate(const char * packageName, char ** out) {
    int ret = uppm_check_if_the_given_argument_matches_package_name_pattern(packageName);

    if (ret != UPPM_OK) {
        return ret;
    }

    UPPMFormulaRepoList * formulaRepoList = NULL;

    ret = uppm_formula_repo_list(&formulaRepoList);

    if (ret != UPPM_OK) {
        uppm_formula_repo_list_free(formulaRepoList);
        return ret;
    }

    struct stat st;

    size_t packageNameLength = strlen(packageName);

    for (size_t i = 0; i < formulaRepoList->size; i++) {
        char * formulaRepoPath = formulaRepoList->repos[i]->path;

        size_t formulaFilePathLength =  strlen(formulaRepoPath) + packageNameLength + 15U;
        char   formulaFilePath[formulaFilePathLength];

        ret = snprintf(formulaFilePath, formulaFilePathLength, "%s/formula/%s.yml", formulaRepoPath, packageName);

        if (ret < 0) {
            perror(NULL);
            uppm_formula_repo_list_free(formulaRepoList);
            return UPPM_ERROR;
        }

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
