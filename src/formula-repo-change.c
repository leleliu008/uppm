#include <stdio.h>
#include <string.h>

#include "uppm.h"
#include "core/zlib-flate.h"

int uppm_formula_repo_change(const char * formulaRepoName, const char * url, const char * branch, const char * pinned, const char * enabled) {
    if (pinned != NULL) {
        if (!(strcmp(pinned, "yes") == 0 || strcmp(pinned, "no") == 0)) {
            fprintf(stderr, "pinned value should be either yes or no.\n");
            return UPPM_ERROR_ARG_IS_INVALID;
        }
    }

    if (enabled != NULL) {
        if (!(strcmp(enabled, "yes") == 0 || strcmp(enabled, "no") == 0)) {
            fprintf(stderr, "enabled value should be either yes or no.\n");
            return UPPM_ERROR_ARG_IS_INVALID;
        }
    }

    UPPMFormulaRepo * formulaRepo = NULL;

    int ret = uppm_formula_repo_lookup(formulaRepoName, &formulaRepo);

    if (ret != UPPM_OK) {
        return ret;
    }

    size_t formulaRepoConfigFilePathLength = strlen(formulaRepo->path) + 24;
    char   formulaRepoConfigFilePath[formulaRepoConfigFilePathLength];
    memset(formulaRepoConfigFilePath, 0, formulaRepoConfigFilePathLength);
    snprintf(formulaRepoConfigFilePath, formulaRepoConfigFilePathLength, "%s/.uppm-formula-repo.dat", formulaRepo->path);

    FILE * file = fopen(formulaRepoConfigFilePath, "wb");

    if (file == NULL) {
        perror(formulaRepoConfigFilePath);
        return UPPM_ERROR;
    }

    if (url == NULL) {
        url = formulaRepo->url;
    }

    if (branch == NULL) {
        branch = formulaRepo->branch;
    }

    if (pinned == NULL) {
        pinned = formulaRepo->pinned ? "yes" : "no";
    }

    if (enabled == NULL) {
        enabled = formulaRepo->enabled ? "yes" : "no";
    }

    const char * timestamp_last_updated;

    if (formulaRepo->timestamp_last_updated == NULL) {
        timestamp_last_updated = "";
    } else {
        timestamp_last_updated = formulaRepo->timestamp_last_updated;
    }

    size_t  strLength = strlen(url) + strlen(branch) + strlen(formulaRepo->timestamp_added) + strlen(timestamp_last_updated) + strlen(pinned) + strlen(enabled) + 78;
    char    str[strLength];
    memset (str, 0, strLength);
    snprintf(str, strLength, "url: %s\nbranch: %s\npinned: %s\nenabled: %s\ntimestamp-added: %s\ntimestamp-last-updated: %s\n", url, branch, pinned, enabled, formulaRepo->timestamp_added, timestamp_last_updated);

    uppm_formula_repo_free(formulaRepo);

    if (zlib_deflate_string_to_file(str, strLength - 1, file) != 0) {
        fclose(file);
        return UPPM_ERROR;
    } else {
        fclose(file);
        return UPPM_OK;
    }
}
