#include <stdio.h>

#include <fcntl.h>
#include <unistd.h>
#include <limits.h>

#include "uppm.h"

int uppm_formula_cat(const char * packageName) {
    char formulaFilePath[PATH_MAX];

    int ret = uppm_formula_path(packageName, formulaFilePath, NULL);

    if (ret != UPPM_OK) {
        return ret;
    }

    int fd = open(formulaFilePath, O_RDONLY);

    if (fd == -1) {
        perror(formulaFilePath);
        return UPPM_ERROR;
    }

    printf("formula: %s\n", formulaFilePath);

    char buf[1024];

    for(;;) {
        ssize_t readSize = read(fd, buf, 1024);

        if (readSize == -1) {
            perror(formulaFilePath);
            close(fd);
            return UPPM_ERROR;
        }

        if (readSize == 0) {
            close(fd);
            return UPPM_OK;
        }

        ssize_t writeSize = write(STDOUT_FILENO, buf, readSize);

        if (writeSize == -1) {
            perror(NULL);
            close(fd);
            return UPPM_ERROR;
        }

        if (writeSize != readSize) {
            fprintf(stderr, "not fully written to stdout.");
            close(fd);
            return UPPM_ERROR;
        }
    }
}
