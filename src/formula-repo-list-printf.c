#include <stdio.h>
#include "uppm.h"

int uppm_formula_repo_list_printf() {
    UPPMFormulaRepoList * formulaRepoList = NULL;

    int resultCode = uppm_formula_repo_list(&formulaRepoList);

    if (resultCode == UPPM_OK) {
        for (size_t i = 0; i < formulaRepoList->size; i++) {
            if (i > 0) {
                printf("---\n");
            }

            uppm_formula_repo_dump(formulaRepoList->repos[i]);
        }
    }

    uppm_formula_repo_list_free(formulaRepoList);

    return resultCode;
}
