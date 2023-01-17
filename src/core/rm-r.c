#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

#include "../uppm.h"

int rm_r(const char * dirPath, bool verbose) {
    if (dirPath == NULL) {
        return UPPM_ERROR_ARG_IS_NULL;
    }

    size_t dirPathLength = strlen(dirPath);

    if (dirPathLength == 0) {
        return UPPM_ERROR_ARG_IS_EMPTY;
    }

    DIR * dir;
    struct dirent * dir_entry;

    dir = opendir(dirPath);

    if (dir == NULL) {
        perror(dirPath);
        return UPPM_ERROR;
    }

    int r = 0;

    while ((dir_entry = readdir(dir))) {
        if ((strcmp(dir_entry->d_name, ".") == 0) || (strcmp(dir_entry->d_name, "..") == 0)) {
            continue;
        }

        size_t filePathLength = dirPathLength + strlen(dir_entry->d_name) + 2;
        char   filePath[filePathLength];
        memset(filePath, 0, filePathLength);
        snprintf(filePath, filePathLength, "%s/%s", dirPath, dir_entry->d_name);

        if (verbose) printf("rm %s\n", filePath);

        struct stat st;

        r = stat(filePath, &st);

        if (r == 0) {
            if (S_ISDIR(st.st_mode)) {
                r = rm_r(filePath, verbose);

                if (r != 0) {
                    break;
                }
            } else {
                r = unlink(filePath);

                if (r != 0) {
                    perror(filePath);
                    break;
                }
            }
        } else {
            r = unlink(filePath);

            if (r != 0) {
                perror(filePath);
                break;
            }
        }
    }

    closedir(dir);

    if (r == 0) {
        r = rmdir(dirPath);

        if (r != 0) {
            perror(dirPath);
        }
    }

    return r == 0 ? UPPM_OK : UPPM_ERROR;
}
