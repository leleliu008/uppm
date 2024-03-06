#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

#include "uppm.h"

static int uppm_record_installed_files_r(const char * dirPath, size_t offset, int outputFD) {
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

    int ret;

    struct stat st;

    for (;;) {
        errno = 0;

        struct dirent * dir_entry = readdir(dir);

        if (dir_entry == NULL) {
            if (errno == 0) {
                closedir(dir);
                return UPPM_OK;
            } else {
                perror(dirPath);
                closedir(dir);
                return UPPM_ERROR;
            }
        }

        if ((strcmp(dir_entry->d_name, ".") == 0) || (strcmp(dir_entry->d_name, "..") == 0)) {
            continue;
        }

        size_t filePathLength = dirPathLength + strlen(dir_entry->d_name) + 2U;
        char   filePath[filePathLength];

        ret = snprintf(filePath, filePathLength, "%s/%s", dirPath, dir_entry->d_name);

        if (ret < 0) {
            perror(NULL);
            closedir(dir);
            return UPPM_ERROR;
        }

        if (stat(filePath, &st) != 0) {
            perror(filePath);
            closedir(dir);
            return UPPM_ERROR;
        }

        if (S_ISDIR(st.st_mode)) {
            ret = dprintf(outputFD, "d|%s/\n", &filePath[offset]);

            if (ret < 0) {
                perror(NULL);
                closedir(dir);
                return UPPM_ERROR;
            }

            ret = uppm_record_installed_files_r(filePath, offset, outputFD);

            if (ret != UPPM_OK) {
                closedir(dir);
                return ret;
            }
        } else {
            ret = dprintf(outputFD, "f|%s\n", &filePath[offset]);

            if (ret < 0) {
                perror(NULL);
                closedir(dir);
                return UPPM_ERROR;
            }
        }
    }
}

int uppm_record_installed_files(const char * installedDIRPath) {
    size_t installedDIRLength = strlen(installedDIRPath);

    size_t installedManifestFilePathLength = installedDIRLength + 20U;
    char   installedManifestFilePath[installedManifestFilePathLength];

    int ret = snprintf(installedManifestFilePath, installedManifestFilePathLength, "%s/.uppm/manifest.txt", installedDIRPath);

    if (ret < 0) {
        perror(NULL);
        return UPPM_ERROR;
    }

    int fd = open(installedManifestFilePath, O_CREAT | O_TRUNC | O_WRONLY, 0666);

    if (fd == -1) {
        perror(installedManifestFilePath);
        return UPPM_ERROR;
    }

    ret = uppm_record_installed_files_r(installedDIRPath, installedDIRLength + 1, fd);

    close(fd);

    return ret;
}
