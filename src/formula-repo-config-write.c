#include <stdio.h>
#include <string.h>

#include "uppm.h"

int uppm_formula_repo_config_write(const char * formulaRepoDirPath, const char * formulaRepoUrl, const char * branchName, int pinned, int enabled, const char * timestamp_added, const char * timestamp_last_updated) {
    if (formulaRepoDirPath == NULL) {
        return UPPM_ERROR_ARG_IS_NULL;
    }

    if (branchName == NULL) {
        return UPPM_ERROR_ARG_IS_NULL;
    }

    if (timestamp_added == NULL) {
        return UPPM_ERROR_ARG_IS_NULL;
    }

    if (timestamp_last_updated == NULL) {
        timestamp_last_updated = "";
    }

    size_t strLength = strlen(formulaRepoUrl) + strlen(branchName) + strlen(timestamp_added) + strlen(timestamp_last_updated) + 78;
    char   str[strLength + 1];
    snprintf(str, strLength + 1, "url: %s\nbranch: %s\npinned: %1d\nenabled: %1d\ntimestamp-added: %s\ntimestamp-last-updated: %s\n", formulaRepoUrl, branchName, pinned, enabled, timestamp_added, timestamp_last_updated);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    size_t formulaRepoConfigFilePathLength = strlen(formulaRepoDirPath) + 24;
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
