#include <stdio.h>
#include <string.h>

#include <limits.h>
#include <sys/stat.h>

#include "uppm.h"

int uppm_formula_repo_lookup(const char * formulaRepoName, UPPMFormulaRepo * * formulaRepoPP) {
    char   uppmHomeDIR[PATH_MAX];
    size_t uppmHomeDIRLength;

    int ret = uppm_home_dir(uppmHomeDIR, PATH_MAX, &uppmHomeDIRLength);

    if (ret != UPPM_OK) {
        return ret;
    }

    size_t formulaRepoNameLength = strlen(formulaRepoName);

    size_t formulaRepoDIRPathCapacity = uppmHomeDIRLength + formulaRepoNameLength + 10U;
    char   formulaRepoDIRPath[formulaRepoDIRPathCapacity];

    ret = snprintf(formulaRepoDIRPath, formulaRepoDIRPathCapacity, "%s/repos.d/%s", uppmHomeDIR, formulaRepoName);

    if (ret < 0) {
        perror(NULL);
        return UPPM_ERROR;
    }

    struct stat st;

    if (stat(formulaRepoDIRPath, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            fprintf(stderr, "%s was expected to be a directory, but it was not.\n", formulaRepoDIRPath);
            return UPPM_ERROR;
        }
    } else {
        return UPPM_ERROR_FORMULA_REPO_NOT_FOUND;
    }

    size_t formulaRepoConfigFilePathCapacity = formulaRepoDIRPathCapacity + 24U;
    char   formulaRepoConfigFilePath[formulaRepoConfigFilePathCapacity];

    ret = snprintf(formulaRepoConfigFilePath, formulaRepoConfigFilePathCapacity, "%s/.uppm-formula-repo.yml", formulaRepoDIRPath);

    if (ret < 0) {
        perror(NULL);
        return UPPM_ERROR;
    }

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

    formulaRepo->path = strdup(formulaRepoDIRPath);

    if (formulaRepo->path == NULL) {
        uppm_formula_repo_free(formulaRepo);
        return UPPM_ERROR_MEMORY_ALLOCATE;
    }

    (*formulaRepoPP) = formulaRepo;
    return UPPM_OK;
}
