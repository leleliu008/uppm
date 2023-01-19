#include <stdio.h>
#include <stdlib.h>

#include "uppm.h"

int uppm_formula_cat(const char * packageName) {
    char * formulaFilePath = NULL;

    int ret = uppm_formula_locate(packageName, &formulaFilePath);

    if (ret != UPPM_OK) {
        return ret;
    }

    FILE * file = fopen(formulaFilePath, "r");

    if (file == NULL) {
        perror(formulaFilePath);
        free(formulaFilePath);
        return UPPM_ERROR;
    }

    printf("formula: %s\n", formulaFilePath);

    free(formulaFilePath);
    formulaFilePath = NULL;

    char   buff[1024];
    size_t size;
    while((size = fread(buff, 1, 1024, file)) != 0) {
        fwrite(buff, 1, size, stdout);
    }

    fclose(file);

    return UPPM_OK;
}
