#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

#include "core/rm-r.h"

#include "uppm.h"

static int uppm_cleanup_downloads(bool verbose) {
    char   uppmHomeDIR[256] = {0};
    size_t uppmHomeDIRLength;

    int ret = uppm_home_dir(uppmHomeDIR, 255, &uppmHomeDIRLength);

    if (ret != UPPM_OK) {
        return ret;
    }

    size_t   downloadDIRLength = uppmHomeDIRLength + 11U;
    char     downloadDIR[downloadDIRLength];
    snprintf(downloadDIR, downloadDIRLength, "%s/downloads", uppmHomeDIR);

    if (chdir(downloadDIR) != 0) {
        if (errno == ENOENT) {
            return UPPM_OK;
        } else {
            perror(downloadDIR);
            return UPPM_ERROR;
        }
    }

    DIR * dir = opendir(downloadDIR);

    if (dir == NULL) {
        perror(downloadDIR);
        return UPPM_ERROR;
    }

    for (;;) {
        errno = 0;

        struct dirent * dir_entry = readdir(dir);

        if (dir_entry == NULL) {
            if (errno == 0) {
                closedir(dir);
                break;
            } else {
                perror(downloadDIR);
                closedir(dir);
                return UPPM_ERROR;
            }
        }

        //puts(dir_entry->d_name);

        const char * fileName = dir_entry->d_name;

        struct stat st;

        if (lstat(fileName, &st) == 0) {
            if (S_ISLNK(st.st_mode)) {
                if (strlen(fileName) == 64U) {
                    char buf[100] = {0};

                    if (readlink(fileName, buf, 99) == 0) {
                        if (lstat(buf, &st) == 0) {
                            if (!S_ISREG(st.st_mode)) {
                                fprintf(stderr, "unexpected symlink: %s/%s\n", downloadDIR, fileName);
                            }
                        } else {
                            fprintf(stderr, "dangling symlink: %s/%s\n", downloadDIR, fileName);
                        }
                    } else {
                        perror(fileName);
                        closedir(dir);
                        return UPPM_ERROR;
                    }
                } else {
                    fprintf(stderr, "unexpected symlink: %s/%s\n", downloadDIR, fileName);
                }
            } else if (S_ISREG(st.st_mode)) {

            } else {
                fprintf(stderr, "unexpected file or directory: %s/%s\n", downloadDIR, fileName);
            }
        }
    }
}

static int uppm_cleanup_installed(bool verbose) {
    char   uppmHomeDIR[256] = {0};
    size_t uppmHomeDIRLength;

    int ret = uppm_home_dir(uppmHomeDIR, 255, &uppmHomeDIRLength);

    if (ret != UPPM_OK) {
        return ret;
    }

    size_t   installedDIRLength = uppmHomeDIRLength + 11U;
    char     installedDIR[installedDIRLength];
    snprintf(installedDIR, installedDIRLength, "%s/installed", uppmHomeDIR);

    if (chdir(installedDIR) != 0) {
        if (errno == ENOENT) {
            return UPPM_OK;
        } else {
            perror(installedDIR);
            return UPPM_ERROR;
        }
    }

    DIR * dir = opendir(installedDIR);

    if (dir == NULL) {
        perror(installedDIR);
        return UPPM_ERROR;
    }

    for (;;) {
        errno = 0;

        struct dirent * dir_entry = readdir(dir);

        if (dir_entry == NULL) {
            if (errno == 0) {
                closedir(dir);
                break;
            } else {
                perror(installedDIR);
                closedir(dir);
                return UPPM_ERROR;
            }
        }

        //puts(dir_entry->d_name);

        const char * fileName = dir_entry->d_name;

        struct stat st;

        if (lstat(fileName, &st) == 0) {
            if (S_ISLNK(st.st_mode)) {
                if (strlen(fileName) == 64U) {
                    char buf[100] = {0};

                    if (readlink(fileName, buf, 99) == 0) {
                        if (lstat(buf, &st) == 0) {
                            if (!S_ISDIR(st.st_mode)) {
                                fprintf(stderr, "unexpected symlink: %s/%s\n", installedDIR, fileName);
                            }
                        } else {
                            fprintf(stderr, "dangling symlink: %s/%s\n", installedDIR, fileName);
                        }
                    } else {
                        perror(fileName);
                        closedir(dir);
                        return UPPM_ERROR;
                    }
                } else {
                    fprintf(stderr, "unexpected symlink: %s/%s\n", installedDIR, fileName);
                }
            } else if (S_ISDIR(st.st_mode)) {
                if (strlen(fileName) == 64U) {

                } else {
                    fprintf(stderr, "unexpected directory: %s/%s\n", installedDIR, fileName);
                }

            } else {
                fprintf(stderr, "unexpected file: %s/%s\n", installedDIR, fileName);
            }
        }
    }
}

int uppm_cleanup(bool verbose) {
    int ret = uppm_cleanup_downloads(verbose);

    if (ret != UPPM_OK) {
        return ret;
    }

    return uppm_cleanup_installed(verbose);
}
