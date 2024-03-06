#include <stdio.h>
#include <string.h>

#include <sys/stat.h>

#include "uppm.h"

int uppm_formula_path(const char * packageName, char buf[], size_t * len) {
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

    for (size_t i = 0U; i < formulaRepoList->size; i++) {
        char * formulaRepoPath = formulaRepoList->repos[i]->path;

        size_t formulaFilePathLength = strlen(formulaRepoPath) + packageNameLength + 15U;
        char   formulaFilePath[formulaFilePathLength];

        int ret = snprintf(formulaFilePath, formulaFilePathLength, "%s/formula/%s.yml", formulaRepoPath, packageName);

        if (ret < 0) {
            perror(NULL);
            uppm_formula_repo_list_free(formulaRepoList);
            return UPPM_ERROR;
        }

        if (stat(formulaFilePath, &st) == 0 && S_ISREG(st.st_mode)) {
            uppm_formula_repo_list_free(formulaRepoList);

            strncpy(buf, formulaFilePath, ret);

            buf[ret] = '\0';

            if (len != NULL) {
                (*len) = ret;
            }

            return UPPM_OK;
        }
    }

    uppm_formula_repo_list_free(formulaRepoList);
    return UPPM_ERROR_PACKAGE_NOT_AVAILABLE;
}
