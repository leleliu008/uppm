#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "uppm.h"

int uppm_formula_repo_lookup(const char * formulaRepoName, UPPMFormulaRepo * * formulaRepoPP) {
    char   uppmHomeDir[256];
    size_t uppmHomeDirLength;

    int ret = uppm_home_dir(uppmHomeDir, 256, &uppmHomeDirLength);

    if (ret != UPPM_OK) {
        return ret;
    }

    size_t formulaRepoNameLength = strlen(formulaRepoName);

    size_t formulaRepoDirPathLength = uppmHomeDirLength + formulaRepoNameLength + 10U;
    char   formulaRepoDirPath[formulaRepoDirPathLength];
    snprintf(formulaRepoDirPath, formulaRepoDirPathLength, "%s/repos.d/%s", uppmHomeDir, formulaRepoName);

    struct stat st;

    if (stat(formulaRepoDirPath, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            fprintf(stderr, "'%s\n' was expected to be a directory, but it was not.\n", formulaRepoDirPath);
            return UPPM_ERROR;
        }
    } else {
        return UPPM_ERROR_FORMULA_REPO_NOT_FOUND;
    }

    size_t formulaRepoConfigFilePathLength = formulaRepoDirPathLength + 24U;
    char   formulaRepoConfigFilePath[formulaRepoConfigFilePathLength];
    snprintf(formulaRepoConfigFilePath, formulaRepoConfigFilePathLength, "%s/.uppm-formula-repo.yml", formulaRepoDirPath);

    if (!((stat(formulaRepoConfigFilePath, &st) == 0) && S_ISREG(st.st_mode))) {
        return UPPM_ERROR_FORMULA_REPO_NOT_FOUND;
    }

    UPPMFormulaRepo * formulaRepo = NULL;

    ret = uppm_formula_repo_parse(formulaRepoConfigFilePath, &formulaRepo);

    if (ret != UPPM_OK) {
        return ret;
    }

    formulaRepo->name = strdup(formulaRepoName);

    if (formulaRepo->name == NULL) {
        uppm_formula_repo_free(formulaRepo);
        return UPPM_ERROR_MEMORY_ALLOCATE;
    }

    formulaRepo->path = strdup(formulaRepoDirPath);

    if (formulaRepo->path == NULL) {
        uppm_formula_repo_free(formulaRepo);
        return UPPM_ERROR_MEMORY_ALLOCATE;
    }

    (*formulaRepoPP) = formulaRepo;
    return UPPM_OK;
}
