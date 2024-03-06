#include <stdio.h>

#include <unistd.h>
#include <limits.h>

#include "uppm.h"

int uppm_formula_bat(const char * packageName) {
    char formulaFilePath[PATH_MAX];

    int ret = uppm_formula_path(packageName, formulaFilePath, NULL);

    if (ret != UPPM_OK) {
        return ret;
    }

    execlp("bat", "bat", "--paging=never", formulaFilePath, NULL);

    perror("bat");

    return UPPM_ERROR;
}
