#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fs.h"
#include "uppm.h"

int uppm_formula_cat(const char * pkgName) {
    char * formulaFilePath = NULL;

    int resultCode = uppm_formula_path(pkgName, &formulaFilePath);

    if (resultCode != UPPM_OK) {
        fprintf(stderr, "package [%s] is not avaiable.\n", pkgName);
        return resultCode;
    }

    FILE * file = fopen(formulaFilePath, "r");

    if (file == NULL) {
        perror(formulaFilePath);
        free(formulaFilePath);
        return UPPM_FORMULA_FILE_OPEN_ERROR;
    }

    free(formulaFilePath);
    formulaFilePath = NULL;

    char buff[1024];
    int  size = 0;
    while((size = fread(buff, 1, 1024, file)) != 0) {
        fwrite(buff, 1, size, stdout);
    }

    fclose(file);

    return UPPM_OK;
}
