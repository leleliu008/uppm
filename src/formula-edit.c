#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <unistd.h>

#include "uppm.h"

int uppm_formula_edit(const char * packageName, const char * editor) {
    char * formulaFilePath = NULL;

    int ret = uppm_formula_locate(packageName, &formulaFilePath);

    if (ret != UPPM_OK) {
        return ret;
    }

    bool ispath = false;

    if (editor == NULL || editor[0] == '\0') {
        editor = "vim";
    } else {
        const char * p = editor;

        for(;;) {
            if (p[0] == '\0') {
                break;
            }

            if (p[0] == '/') {
                ispath = true;
                break;
            }

            p++;
        }
    }

    if (ispath) {
        execl (editor, editor, formulaFilePath, NULL);
    } else {
        execlp(editor, editor, formulaFilePath, NULL);
    }

    perror(editor);

    free(formulaFilePath);

    return UPPM_ERROR;
}
