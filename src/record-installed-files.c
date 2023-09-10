#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

#include "uppm.h"

static int record_installed_files_r(const char * dirPath, size_t offset, int outputFD) {
    if (dirPath == NULL) {
        return UPPM_ERROR_ARG_IS_NULL;
    }

    if (dirPath[0] == '\0') {
        return UPPM_ERROR_ARG_IS_EMPTY;
    }

    size_t dirPathLength = strlen(dirPath);

    DIR * dir = opendir(dirPath);

    if (dir == NULL) {
        perror(dirPath);
        return UPPM_ERROR;
    }

    int ret = UPPM_OK;

    struct stat st;

    for (;;) {
        errno = 0;

        struct dirent * dir_entry = readdir(dir);

        if (dir_entry == NULL) {
            if (errno == 0) {
                closedir(dir);
                break;
            } else {
                perror(dirPath);
                closedir(dir);
                return UPPM_ERROR;
            }
        }

        if ((strcmp(dir_entry->d_name, ".") == 0) || (strcmp(dir_entry->d_name, "..") == 0)) {
            continue;
        }

        size_t   filePathLength = dirPathLength + strlen(dir_entry->d_name) + 2U;
        char     filePath[filePathLength];
        snprintf(filePath, filePathLength, "%s/%s", dirPath, dir_entry->d_name);

        if (stat(filePath, &st) != 0) {
            perror(filePath);
            closedir(dir);
            return UPPM_ERROR;
        }

        if (S_ISDIR(st.st_mode)) {
            dprintf(outputFD, "d|%s/\n", &filePath[offset]);

            ret = record_installed_files_r(filePath, offset, outputFD);

            if (ret != UPPM_OK) {
                closedir(dir);
                return ret;
            }
        } else {
            dprintf(outputFD, "f|%s\n", &filePath[offset]);
        }
    }

    return ret;
}

int record_installed_files(const char * installedDIRPath) {
    size_t   installedDIRLength = strlen(installedDIRPath);

    size_t   installedManifestFilePathLength = installedDIRLength + 20U;
    char     installedManifestFilePath[installedManifestFilePathLength];
    snprintf(installedManifestFilePath, installedManifestFilePathLength, "%s/.uppm/manifest.txt", installedDIRPath);

    int fd = open(installedManifestFilePath, O_CREAT | O_TRUNC | O_WRONLY, 0666);

    if (fd == -1) {
        perror(installedManifestFilePath);
        return UPPM_ERROR;
    }

    int ret = record_installed_files_r(installedDIRPath, installedDIRLength + 1, fd);

    close(fd);

    return ret;
}
