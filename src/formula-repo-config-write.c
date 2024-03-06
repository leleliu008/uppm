#include <stdio.h>
#include <string.h>

#include "uppm.h"

int uppm_formula_repo_config_write(const char * formulaRepoDirPath, const char * formulaRepoUrl, const char * branchName, int pinned, int enabled, const char * timestamp_created, const char * timestamp_updated) {
    if (formulaRepoDirPath == NULL) {
        return UPPM_ERROR_ARG_IS_NULL;
    }

    if (branchName == NULL) {
        return UPPM_ERROR_ARG_IS_NULL;
    }

    if (timestamp_created == NULL) {
        return UPPM_ERROR_ARG_IS_NULL;
    }

    if (timestamp_updated == NULL) {
        timestamp_updated = "";
    }

    size_t strCapacity = strlen(formulaRepoUrl) + strlen(branchName) + strlen(timestamp_created) + strlen(timestamp_updated) + 67U;
    char   str[strCapacity];

    int ret = snprintf(str, strCapacity, "url: %s\nbranch: %s\npinned: %1d\nenabled: %1d\ncreated: %s\nupdated: %s\n", formulaRepoUrl, branchName, pinned, enabled, timestamp_created, timestamp_updated);

    if (ret < 0) {
        perror(NULL);
        return UPPM_ERROR;
    }

    size_t strLength = ret;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    size_t formulaRepoConfigFilePathLength = strlen(formulaRepoDirPath) + 24U;
    char   formulaRepoConfigFilePath[formulaRepoConfigFilePathLength];
    snprintf(formulaRepoConfigFilePath, formulaRepoConfigFilePathLength, "%s/.uppm-formula-repo.yml", formulaRepoDirPath);

    FILE * file = fopen(formulaRepoConfigFilePath, "w");

    if (file == NULL) {
        perror(formulaRepoConfigFilePath);
        return UPPM_ERROR;
    }

    if (fwrite(str, 1, strLength, file) != strLength || ferror(file)) {
        perror(formulaRepoConfigFilePath);
        fclose(file);
        return UPPM_ERROR;
    }

    fclose(file);
    return UPPM_OK;
}
