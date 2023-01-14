#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/fs.h"
#include "core/rm-r.h"
#include "uppm.h"

int uppm_formula_repo_del(const char * formulaRepoName) {
    if (formulaRepoName == NULL) {
        return UPPM_ARG_IS_NULL;
    }

    if (strcmp(formulaRepoName, "") == 0) {
        return UPPM_ARG_IS_EMPTY;
    }

    if (strcmp(formulaRepoName, "offical-core") == 0) {
        fprintf(stderr, "offical-core formula repo is not allowed to delete.\n");
        return UPPM_ERROR;
    }

    char * userHomeDir = getenv("HOME");

    if (userHomeDir == NULL) {
        return UPPM_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    if (strcmp(userHomeDir, "") == 0) {
        return UPPM_ENV_HOME_NOT_SET;
    }

    size_t  uppmFormulaRepoDirLength = userHomeDirLength + 15;
    char    uppmFormulaRepoDir[uppmFormulaRepoDirLength];
    memset (uppmFormulaRepoDir, 0, uppmFormulaRepoDirLength);
    snprintf(uppmFormulaRepoDir, uppmFormulaRepoDirLength, "%s/.uppm/repos.d", userHomeDir);

    size_t  formulaRepoDirLength = uppmFormulaRepoDirLength + strlen(formulaRepoName) + 2;
    char    formulaRepoDir[formulaRepoDirLength];
    memset (formulaRepoDir, 0, formulaRepoDirLength);
    snprintf(formulaRepoDir, formulaRepoDirLength, "%s/%s", uppmFormulaRepoDir, formulaRepoName);

    if (exists_and_is_a_directory(formulaRepoDir)) {
        return rm_r(formulaRepoDir, false);
    } else {
        fprintf(stderr, "%s named formula repo is not exist.", formulaRepoName);
        return UPPM_ERROR;
    }
}
