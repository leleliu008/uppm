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
    size_t size = 0;

    while((size = fread(buff, 1, 1024, fromFile)) != 0) {
        fwrite(buff, 1, size, toFile);
    }

    fclose(fromFile);
    fclose(toFile);

    return UPPM_OK;
}
