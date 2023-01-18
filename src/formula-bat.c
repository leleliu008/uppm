#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "uppm.h"

int uppm_formula_bat(const char * packageName) {
    char * formulaFilePath = NULL;

    int resultCode = uppm_formula_find(packageName, &formulaFilePath);

    if (resultCode != UPPM_OK) {
        return resultCode;
    }

    free(formulaFilePath);

    execlp("bat", "bat", "--paging=never", formulaFilePath, NULL);

    perror("bat");

    free(formulaFilePath);

    return UPPM_ERROR;
}
