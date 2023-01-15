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

    size_t formulaFilePathLength = strlen(formulaFilePath);
    char   formulaFilePath2[formulaFilePathLength + 1];
    formulaFilePath2[formulaFilePathLength] = '\0';
    strncpy(formulaFilePath2, formulaFilePath, formulaFilePathLength);

    free(formulaFilePath);

    resultCode = execlp("bat", "bat", "--paging=never", formulaFilePath2, NULL);

    if (resultCode == -1) {
        fprintf(stderr, "command not found: bat\n");
        return UPPM_ERROR;
    }
}
