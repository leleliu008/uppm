#include <stdio.h>

#include "cp.h"

int copy_file(const char * fromFilePath, const char * toFilePath) {
    FILE * fromFile = fopen(fromFilePath, "rb");

    if (fromFile == NULL) {
        perror(fromFilePath);
        return UPPM_ERROR;
    }

    FILE * toFile = fopen(toFilePath, "wb");

    if (toFile == NULL) {
        perror(toFilePath);
        fclose(fromFile);
        return UPPM_ERROR;
    }

    unsigned char buff[1024];
    size_t size;

    for (;;) {
        size = fread(buff, 1, 1024, fromFile);

        if (ferror(fromFile)) {
            perror(fromFilePath);
            fclose(fromFile);
            fclose(toFile);
            return UPPM_ERROR;
        }

        if (size > 0) {
            if (fwrite(buff, 1, size, toFile) != size || ferror(toFile)) {
                perror(toFilePath);
                fclose(fromFile);
                fclose(toFile);
                return UPPM_ERROR;
            }
        }

        if (feof(fromFile)) {
            fclose(fromFile);
            fclose(toFile);
            return UPPM_OK;
        }
    }
}
