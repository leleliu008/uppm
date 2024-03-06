#include <stdio.h>

#include <unistd.h>
#include <limits.h>

#include "uppm.h"

int uppm_formula_edit(const char * packageName, const char * editor) {
    char formulaFilePath[PATH_MAX];

    int ret = uppm_formula_path(packageName, formulaFilePath, NULL);

    if (ret != UPPM_OK) {
        return ret;
    }

    int slashIndex = -1;

    if (editor == NULL || editor[0] == '\0') {
        editor = "vim";
    } else {
        for(int i = 0; ;i++) {
            if (editor[i] == '\0') {
                break;
            }

            if (editor[i] == '/') {
                slashIndex = i;
                break;
            }
        }
    }

    if (slashIndex == -1) {
        execlp(editor, editor, formulaFilePath, NULL);
    } else {
        execl (editor, editor, formulaFilePath, NULL);
    }

    perror(editor);

    return UPPM_ERROR;
}
