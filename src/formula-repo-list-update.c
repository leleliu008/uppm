#include <stdio.h>

#include "core/fs.h"
#include "core/git.h"
#include "uppm.h"

int uppm_formula_repo_list_update() {
    UPPMFormulaRepoList * formulaRepoList = NULL;

    int resultCode = uppm_formula_repo_list_new(&formulaRepoList);

    if (resultCode == 0) {
        for (size_t i = 0; i < formulaRepoList->size; i++) {
            UPPMFormulaRepo * formulaRepo = formulaRepoList->repos[i];
            printf("updating formula repo : %s => %s\n", formulaRepo->name, formulaRepo->url);

            if (exists_and_is_a_directory(formulaRepo->path)) {
                resultCode = do_git_pull(formulaRepo->path, NULL, NULL);
            } else {
                resultCode = do_git_clone(formulaRepo->url, formulaRepo->path);
            }
        }
    }

    uppm_formula_repo_list_free(formulaRepoList);

    return resultCode;
}
