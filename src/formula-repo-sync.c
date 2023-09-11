#include <time.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>

#include "core/log.h"

#include "uppm.h"

int uppm_formula_repo_sync_(const char * formulaRepoName) {
    UPPMFormulaRepo * formulaRepo = NULL;

    int ret = uppm_formula_repo_lookup(formulaRepoName, &formulaRepo);

    if (ret == UPPM_OK) {
        ret = uppm_formula_repo_sync(formulaRepo);
    }

    uppm_formula_repo_free(formulaRepo);

    return ret;
}

int uppm_formula_repo_sync(UPPMFormulaRepo * formulaRepo) {
    if (formulaRepo == NULL) {
        return UPPM_ERROR_ARG_IS_NULL;
    }

    if (formulaRepo->pinned) {
        fprintf(stderr, "'%s' formula repo was pinned, skipped.\n", formulaRepo->name);
        return UPPM_OK;
    }

    if (isatty(STDOUT_FILENO)) {
        printf("%s%s%s\n", COLOR_PURPLE, "==> Updating formula repo", COLOR_OFF);
    } else {
        printf("=== Updating formula repo\n");
    }

    uppm_formula_repo_info(formulaRepo);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const char * branchName = formulaRepo->branch;
    size_t       branchNameLength = strlen(branchName);

    size_t   remoteRefPathLength = branchNameLength + 12U;
    char     remoteRefPath[remoteRefPathLength];
    snprintf(remoteRefPath, remoteRefPathLength, "refs/heads/%s", branchName);

    size_t   remoteTrackingRefPathLength = branchNameLength + 21U;
    char     remoteTrackingRefPath[remoteTrackingRefPathLength];
    snprintf(remoteTrackingRefPath, remoteTrackingRefPathLength, "refs/remotes/origin/%s", branchName);

    int ret = uppm_git_sync(formulaRepo->path, formulaRepo->url, remoteRefPath, remoteTrackingRefPath, branchName);

    if (ret != UPPM_OK) {
        return ret;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    char ts[11];
    snprintf(ts, 11, "%ld", time(NULL));

    return uppm_formula_repo_config_write(formulaRepo->path, formulaRepo->url, formulaRepo->branch, formulaRepo->pinned, formulaRepo->enabled, formulaRepo->timestamp_created, ts);
}
