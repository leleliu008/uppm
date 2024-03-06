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

    if (strcmp(formulaRepoName, "official-core") == 0) {
        fprintf(stderr, "official-core formula repo is not allowed to delete.\n");
        return UPPM_ERROR;
    }

    char   uppmHomeDIR[PATH_MAX];
    size_t uppmHomeDIRLength;

    int ret = uppm_home_dir(uppmHomeDIR, PATH_MAX, &uppmHomeDIRLength);

    if (ret != UPPM_OK) {
        return ret;
    }

    size_t formulaRepoPathCapacity = uppmHomeDIRLength + strlen(formulaRepoName) + 10U;
    char   formulaRepoPath[formulaRepoPathCapacity];

    ret = snprintf(formulaRepoPath, formulaRepoPathCapacity, "%s/repos.d/%s", uppmHomeDIR, formulaRepoName);

    if (ret < 0) {
        perror(NULL);
        return UPPM_ERROR;
    }

    struct stat st;

    if (stat(formulaRepoPath, &st) != 0) {
        fprintf(stderr, "formula repo not found: %s\n", formulaRepoName);
        return UPPM_ERROR;
    }

    size_t formulaRepoConfigFilePathCapacity = formulaRepoPathCapacity + 24U;
    char   formulaRepoConfigFilePath[formulaRepoConfigFilePathCapacity];

    ret = snprintf(formulaRepoConfigFilePath, formulaRepoConfigFilePathCapacity, "%s/.uppm-formula-repo.yml", formulaRepoPath);

    if (ret < 0) {
        perror(NULL);
        return UPPM_ERROR;
    }

    if (stat(formulaRepoConfigFilePath, &st) == 0 && S_ISREG(st.st_mode)) {
        return uppm_rm_r(formulaRepoPath, false);
    } else {
        fprintf(stderr, "formula repo is broken: %s\n", formulaRepoName);
        return UPPM_ERROR;
    }
}
