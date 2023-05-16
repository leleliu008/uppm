#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "core/rm-r.h"

#include "uppm.h"

int uppm_formula_repo_remove(const char * formulaRepoName) {
    if (formulaRepoName == NULL) {
        return UPPM_ERROR_ARG_IS_NULL;
    }

    size_t formulaRepoNameLength = strlen(formulaRepoName);

    if (formulaRepoNameLength == 0U) {
        return UPPM_ERROR_ARG_IS_EMPTY;
    }

    if (strcmp(formulaRepoName, "offical-core") == 0) {
        fprintf(stderr, "offical-core formula repo is not allowed to delete.\n");
        return UPPM_ERROR;
    }

    char   uppmHomeDir[256];
    size_t uppmHomeDirLength;

    int ret = uppm_home_dir(uppmHomeDir, 256, &uppmHomeDirLength);

    if (ret != UPPM_OK) {
        return ret;
    }

    size_t   formulaRepoPathLength = uppmHomeDirLength + formulaRepoNameLength + 10U;
    char     formulaRepoPath[formulaRepoPathLength];
    snprintf(formulaRepoPath, formulaRepoPathLength, "%s/repos.d/%s", uppmHomeDir, formulaRepoName);

    struct stat st;

    if (stat(formulaRepoPath, &st) != 0) {
        fprintf(stderr, "formula repo not found: %s\n", formulaRepoName);
        return UPPM_ERROR;
    }

    size_t   formulaRepoConfigFilePathLength = formulaRepoPathLength + 24U;
    char     formulaRepoConfigFilePath[formulaRepoConfigFilePathLength];
    snprintf(formulaRepoConfigFilePath, formulaRepoConfigFilePathLength, "%s/.uppm-formula-repo.yml", formulaRepoPath);

    if (stat(formulaRepoConfigFilePath, &st) == 0 && S_ISREG(st.st_mode)) {
        if (rm_r(formulaRepoPath, false) == 0) {
            return UPPM_OK;
        } else {
            perror(formulaRepoPath);
            return UPPM_ERROR;
        }
    } else {
        fprintf(stderr, "formula repo is broken: %s\n", formulaRepoName);
        return UPPM_ERROR;
    }
}
