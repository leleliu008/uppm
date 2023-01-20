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

    ret = uppm_formula_repo_config_write(formulaRepo->path, url, branch, pinned, enabled, formulaRepo->timestamp_added, formulaRepo->timestamp_last_updated);

    uppm_formula_repo_free(formulaRepo);

    return ret;
}
