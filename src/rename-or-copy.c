#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "uppm.h"

int uppm_rename_or_copy_file(const char * fromFilePath, const char * toFilePath) {
    if (fromFilePath == NULL) {
        return UPPM_ERROR_ARG_IS_NULL;
    }

    if (toFilePath == NULL) {
        return UPPM_ERROR_ARG_IS_NULL;
    }

    if (fromFilePath[0] == '\0') {
        return UPPM_ERROR_ARG_IS_NULL;
    }

    if (toFilePath[0] == '\0') {
        return UPPM_ERROR_ARG_IS_NULL;
    }

    size_t i = 0U;
    size_t j = 0U;

    for (;;) {
        char c = toFilePath[i];

        if (c == '\0') {
            break;
        }

        if (c == '/') {
            j = i;
        }

        i++;
    }

    if (j > 0U) {
        char outputDIR[j + 2U];

        strncpy(outputDIR, toFilePath, j);

        outputDIR[j] = '\0';

        int ret = uppm_mkdir_p(outputDIR, false);

        if (ret != UPPM_OK) {
            return ret;
        }
    }

    if (rename(fromFilePath, toFilePath) == 0) {
        return UPPM_OK;
    } else {
        if (errno == EXDEV) {
            return uppm_copy_file(fromFilePath, toFilePath);
        } else {
            perror(toFilePath);
            return UPPM_ERROR;
        }
    }
}
