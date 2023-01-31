#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>

#include "rm-r.h"

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

    struct stat st;

    int ret = UPPM_OK;

    for (;;) {
        errno = 0;

        dir_entry = readdir(dir);

        if (dir_entry == NULL) {
            if (errno == 0) {
                closedir(dir);

                if (rmdir(dirPath) == 0) {
                    break;
                } else {
                    perror(dirPath);
                    return UPPM_ERROR;
                }
            } else {
                perror(dirPath);
                closedir(dir);
                return UPPM_ERROR;
            }
        }

        if ((strcmp(dir_entry->d_name, ".") == 0) || (strcmp(dir_entry->d_name, "..") == 0)) {
            continue;
        }

        size_t filePathLength = dirPathLength + strlen(dir_entry->d_name) + 2;
        char   filePath[filePathLength];
        snprintf(filePath, filePathLength, "%s/%s", dirPath, dir_entry->d_name);

        if (verbose) printf("rm %s\n", filePath);

        if (stat(filePath, &st) == 0) {
            if (S_ISDIR(st.st_mode)) {
                ret = rm_r(filePath, verbose);

                if (ret != UPPM_OK) {
                    closedir(dir);
                    return UPPM_ERROR;
                }
            } else {
                if (unlink(filePath) != 0) {
                    perror(filePath);
                    closedir(dir);
                    return UPPM_ERROR;
                }
            }
        } else {
            perror(filePath);
            closedir(dir);
            return UPPM_ERROR;
        }
    }

    return UPPM_OK;
}
