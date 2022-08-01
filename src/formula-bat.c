#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "fs.h"
#include "uppm.h"

int uppm_formula_bat(const char * pkgName) {
    char * formulaFilePath = NULL;

    int resultCode = UPPM_OK;

    resultCode = uppm_formula_path(pkgName, &formulaFilePath);

    if (resultCode != UPPM_OK) {
        fprintf(stderr, "package [%s] is not avaiable.\n", pkgName);
        return resultCode;
    }

    resultCode = execlp("bat", "bat", "--paging=never", formulaFilePath, NULL);

    if (resultCode == -1) {
        fprintf(stderr, "command not found: bat\n");
    }

    free(formulaFilePath);

    return resultCode;
}
