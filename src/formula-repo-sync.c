#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

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

    size_t refspecLength = (strlen(formulaRepo->branch) << 1) + 33;
    char   refspec[refspecLength];
    snprintf(refspec, refspecLength, "refs/heads/%s:refs/remotes/origin/%s", formulaRepo->branch, formulaRepo->branch);

    if (uppm_fetch_via_git(formulaRepo->path, formulaRepo->url, refspec, formulaRepo->branch) != 0) {
        return UPPM_ERROR;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    char ts[11];
    snprintf(ts, 11, "%ld", time(NULL));

    return uppm_formula_repo_config_write(formulaRepo->path, formulaRepo->url, formulaRepo->branch, formulaRepo->pinned, formulaRepo->enabled, formulaRepo->timestamp_created, ts);
}
