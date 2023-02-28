#include <stdio.h>
#include "uppm.h"

int uppm_formula_repo_list_printf() {
    UPPMFormulaRepoList * formulaRepoList = NULL;

    int ret = uppm_formula_repo_list(&formulaRepoList);

    if (ret == UPPM_OK) {
        for (size_t i = 0; i < formulaRepoList->size; i++) {
            if (i > 0) {
                printf("---\n");
            }

            uppm_formula_repo_info(formulaRepoList->repos[i]);
        }

        uppm_formula_repo_list_free(formulaRepoList);
    }

    return ret;
}
