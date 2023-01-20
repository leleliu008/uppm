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

    for(;;) {
        size = fread(buff, 1, 1024, file);

        if (ferror(file)) {
            fclose(file);
            return UPPM_ERROR;
        }

        if (fwrite(buff, 1, size, stdout) != size || ferror(stdout)) {
            fclose(file);
            return UPPM_ERROR;
        }

        if (feof(file)) {
            fclose(file);
            return UPPM_OK;
        }
    }
}
