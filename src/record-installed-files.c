#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>

#include "uppm.h"

static int record_installed_files_r(const char * dirPath, size_t offset, FILE * installedManifestFile) {
    if (dirPath == NULL) {
        return UPPM_ERROR_ARG_IS_NULL;
    }

    size_t dirPathLength = strlen(dirPath);

    if (dirPathLength == 0) {
        return UPPM_ERROR_ARG_IS_EMPTY;
    }

    DIR           * dir;
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

        size_t filePathLength = dirPathLength + strlen(dir_entry->d_name) + 2;
        char   filePath[filePathLength];
        snprintf(filePath, filePathLength, "%s/%s", dirPath, dir_entry->d_name);

        if (stat(filePath, &st) != 0) {
            perror(filePath);
            closedir(dir);
            return UPPM_ERROR;
        }

        if (S_ISDIR(st.st_mode)) {
            fprintf(installedManifestFile, "d|%s/\n", &filePath[offset]);

            ret = record_installed_files_r(filePath, offset, installedManifestFile);

            if (ret != UPPM_OK) {
                closedir(dir);
                return ret;
            }
        } else {
            fprintf(installedManifestFile, "f|%s\n", &filePath[offset]);
        }
    }

    return ret;
}

int record_installed_files(const char * installedDirPath) {
    size_t installedDirLength = strlen(installedDirPath);

    size_t installedManifestFilePathLength = installedDirLength + 20;
    char   installedManifestFilePath[installedManifestFilePathLength];
    snprintf(installedManifestFilePath, installedManifestFilePathLength, "%s/.uppm/manifest.txt", installedDirPath);

    FILE * installedManifestFile = fopen(installedManifestFilePath, "w");

    if (installedManifestFile == NULL) {
        perror(installedManifestFilePath);
        return UPPM_ERROR;
    }

    int ret = record_installed_files_r(installedDirPath, installedDirLength + 1, installedManifestFile);

    fclose(installedManifestFile);

    return ret;
}
