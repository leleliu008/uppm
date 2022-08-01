#include <stdio.h>
#include "uppm.h"

int uppm_formula_repo_list_printf() {
    UPPMFormulaRepoList * formulaRepoList = NULL;

    int resultCode = uppm_formula_repo_list_new(&formulaRepoList);

    if (resultCode == 0) {
        for (size_t i = 0; i < formulaRepoList->size; i++) {
            UPPMFormulaRepo * formulaRepo = formulaRepoList->repos[i];
            printf("%s=%s\n", formulaRepo->name, formulaRepo->url);
        }
    }

    uppm_formula_repo_list_free(formulaRepoList);

    return resultCode;
}
