#include <stdio.h>
#include <string.h>

#include "uppm.h"

int uppm_formula_repo_config(const char * formulaRepoName, const char * url, const char * branch, int pinned, int enabled) {
    UPPMFormulaRepo * formulaRepo = NULL;

    int ret = uppm_formula_repo_lookup(formulaRepoName, &formulaRepo);

    if (ret != UPPM_OK) {
        return ret;
    }

    if (url == NULL) {
        url = formulaRepo->url;
    }

    if (branch == NULL) {
        branch = formulaRepo->branch;
    }

    if (pinned == -1) {
        pinned = formulaRepo->pinned;
    }

    if (enabled == -1) {
        enabled = formulaRepo->enabled;
    }

    const char * timestamp_last_updated;

    if (formulaRepo->timestamp_last_updated == NULL) {
        timestamp_last_updated = "";
    } else {
        timestamp_last_updated = formulaRepo->timestamp_last_updated;
    }

    size_t strLength = strlen(url) + strlen(branch) + strlen(formulaRepo->timestamp_added) + strlen(timestamp_last_updated) + 79;
    char   str[strLength];
    snprintf(str, strLength, "url: %s\nbranch: %s\npinned: %1d\nenabled: %1d\ntimestamp-added: %s\ntimestamp-last-updated: %s\n", url, branch, pinned, enabled, formulaRepo->timestamp_added, timestamp_last_updated);

    uppm_formula_repo_free(formulaRepo);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    size_t formulaRepoConfigFilePathLength = strlen(formulaRepo->path) + 24;
    char   formulaRepoConfigFilePath[formulaRepoConfigFilePathLength];
    snprintf(formulaRepoConfigFilePath, formulaRepoConfigFilePathLength, "%s/.uppm-formula-repo.yml", formulaRepo->path);

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
