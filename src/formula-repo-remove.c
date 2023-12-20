#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <limits.h>
#include <sys/stat.h>

#include "uppm.h"

int uppm_formula_repo_remove(const char * formulaRepoName) {
    if (formulaRepoName == NULL) {
        return UPPM_ERROR_ARG_IS_NULL;
    }

    if (formulaRepoName[0] == '\0') {
        return UPPM_ERROR_ARG_IS_EMPTY;
    }

    if (strcmp(formulaRepoName, "offical-core") == 0) {
        fprintf(stderr, "offical-core formula repo is not allowed to delete.\n");
        return UPPM_ERROR;
    }

    char   uppmHomeDIR[PATH_MAX];
    size_t uppmHomeDIRLength;

    int ret = uppm_home_dir(uppmHomeDIR, PATH_MAX, &uppmHomeDIRLength);

    if (ret != UPPM_OK) {
        return ret;
    }

    size_t   formulaRepoPathLength = uppmHomeDIRLength + strlen(formulaRepoName) + 10U;
    char     formulaRepoPath[formulaRepoPathLength];
    snprintf(formulaRepoPath, formulaRepoPathLength, "%s/repos.d/%s", uppmHomeDIR, formulaRepoName);

    struct stat st;

    if (stat(formulaRepoPath, &st) != 0) {
        fprintf(stderr, "formula repo not found: %s\n", formulaRepoName);
        return UPPM_ERROR;
    }

    size_t   formulaRepoConfigFilePathLength = formulaRepoPathLength + 24U;
    char     formulaRepoConfigFilePath[formulaRepoConfigFilePathLength];
    snprintf(formulaRepoConfigFilePath, formulaRepoConfigFilePathLength, "%s/.uppm-formula-repo.yml", formulaRepoPath);

    if (stat(formulaRepoConfigFilePath, &st) == 0 && S_ISREG(st.st_mode)) {
        return uppm_rm_r(formulaRepoPath, false);
    } else {
        fprintf(stderr, "formula repo is broken: %s\n", formulaRepoName);
        return UPPM_ERROR;
    }
}
