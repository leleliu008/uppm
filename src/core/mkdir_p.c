#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>

#include "mkdir_p.h"

int mkdir_p(const char * dirPath, bool verbose) {
    if (dirPath == NULL) {
        return UPPM_ERROR_ARG_IS_NULL;
    }

    size_t dirPathLength = strlen(dirPath);

    if (dirPathLength == 0) {
        return UPPM_ERROR_ARG_IS_EMPTY;
    }

    if (verbose) printf("rm %s\n", dirPath);

    struct stat st;

    if (stat(dirPath, &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            return UPPM_OK;
        } else {
            fprintf(stderr, "'%s\n' was expected to be a directory, but it was not.\n", dirPath);
        }
    } else {
        size_t i = dirPathLength - 1;

        if (dirPath[i] == '/') {
            i--;
        }

        for(;;) {
            if (dirPath[i] == '/') {
                if (i == 0) { // /a
                    if (mkdir(dirPath, S_IRWXU)) {
                        return UPPM_OK;
                    } else {
                        perror(dirPath);
                        return UPPM_ERROR;
                    }
                } else {
                    char p[i];
                    strncpy(p, dirPath, i - 1);

                    return mkdir_p(p, verbose);
                }
            }

            i--;

            if (i == 0) {
                // dirPath is a relative path

                if (mkdir(dirPath, S_IRWXU)) {
                    return UPPM_OK;
                } else {
                    perror(dirPath);
                    return UPPM_ERROR;
                }
            }
        }
    }

    return UPPM_OK;
}
