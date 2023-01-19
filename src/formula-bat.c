#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "uppm.h"

int uppm_formula_bat(const char * packageName) {
    char * formulaFilePath = NULL;

    int ret = uppm_formula_locate(packageName, &formulaFilePath);

    if (ret != UPPM_OK) {
        return ret;
    }

    execlp("bat", "bat", "--paging=never", formulaFilePath, NULL);

    perror("bat");

    free(formulaFilePath);

    return UPPM_ERROR;
}
