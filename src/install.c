#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <libgen.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include "core/sysinfo.h"
#include "core/sha256sum.h"
#include "core/rm-r.h"
#include "core/tar.h"

#include "uppm.h"

extern int record_installed_files(const char * installedDirPath);

int uppm_install(const char * packageName, bool verbose) {
    UPPMFormula * formula = NULL;

    int ret = uppm_formula_lookup(packageName, &formula);

    if (ret != UPPM_OK) {
        return ret;
    }

    //////////////////////////////////////////////////////////////////////////

    if (formula->dep_pkg != NULL) {
        size_t depPackageNamesLength = strlen(formula->dep_pkg);

        size_t depPackageNamesCopyLength = depPackageNamesLength + 1U;
        char   depPackageNamesCopy[depPackageNamesCopyLength];
        strncpy(depPackageNamesCopy, formula->dep_pkg, depPackageNamesCopyLength);

        char * depPackageNameArrayList[10];
        size_t depPackageNameArrayListSize = 0;

        char * depPackageName = strtok(depPackageNamesCopy, " ");

        while (depPackageName != NULL) {
            depPackageNameArrayList[depPackageNameArrayListSize] = depPackageName;
            depPackageNameArrayListSize++;
            depPackageName = strtok(NULL, " ");
        }

        for (size_t i = 0; i < depPackageNameArrayListSize; i++) {
            ret = uppm_install(depPackageNameArrayList[i], verbose);

            if (ret != UPPM_OK) {
                uppm_formula_free(formula);
                return ret;
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////

    const char * const userHomeDir = getenv("HOME");

    if (userHomeDir == NULL) {
        return UPPM_ERROR_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    if (userHomeDirLength == 0U) {
        return UPPM_ERROR_ENV_HOME_NOT_SET;
    }

    struct stat st;

    size_t   packageInstalledDirLength = userHomeDirLength + strlen(packageName) + 20U;
    char     packageInstalledDir[packageInstalledDirLength];
    snprintf(packageInstalledDir, packageInstalledDirLength, "%s/.uppm/installed/%s", userHomeDir, packageName);

    size_t   packageInstalledMetaInfoDirLength = packageInstalledDirLength + 6U;
    char     packageInstalledMetaInfoDir[packageInstalledMetaInfoDirLength];
    snprintf(packageInstalledMetaInfoDir, packageInstalledMetaInfoDirLength, "%s/.uppm", packageInstalledDir);

    size_t   receiptFilePathLength = packageInstalledMetaInfoDirLength + 12U;
    char     receiptFilePath[receiptFilePathLength];
    snprintf(receiptFilePath, receiptFilePathLength, "%s/receipt.yml", packageInstalledMetaInfoDir);

    if (stat(receiptFilePath, &st) == 0 && S_ISREG(st.st_mode)) {
        uppm_formula_free(formula);
        fprintf(stderr, "package [%s] already has been installed.\n", packageName);
        return UPPM_OK;
    }

    //////////////////////////////////////////////////////////////////////////

    fprintf(stderr, "prepare to install package [%s].\n", packageName);

    size_t   uppmDownloadDirLength = userHomeDirLength + 18U;
    char     uppmDownloadDir[uppmDownloadDirLength];
    snprintf(uppmDownloadDir, uppmDownloadDirLength, "%s/.uppm/downloads", userHomeDir);

    if (stat(uppmDownloadDir, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            fprintf(stderr, "'%s\n' was expected to be a directory, but it was not.\n", uppmDownloadDir);
            return UPPM_ERROR;
        }
    } else {
        if (mkdir(uppmDownloadDir, S_IRWXU) != 0) {
            uppm_formula_free(formula);
            return UPPM_ERROR;
        }
    }

    char binFileNameExtension[21] = {0};

    ret = uppm_examine_file_extension_from_url(binFileNameExtension, 20, formula->bin_url);

    if (ret != UPPM_OK) {
        uppm_formula_free(formula);
        return ret;
    }

    size_t   binFileNameLength = strlen(formula->bin_sha) + strlen(binFileNameExtension) + 1U;
    char     binFileName[binFileNameLength];
    snprintf(binFileName, binFileNameLength, "%s%s", formula->bin_sha, binFileNameExtension);

    size_t   binFilePathLength = uppmDownloadDirLength + binFileNameLength + 1U;
    char     binFilePath[binFilePathLength];
    snprintf(binFilePath, binFilePathLength, "%s/%s", uppmDownloadDir, binFileName);

    bool needFetch = true;

    if (stat(binFilePath, &st) == 0) {
        if (S_ISREG(st.st_mode)) {
            char actualSHA256SUM[65] = {0};

            if (sha256sum_of_file(actualSHA256SUM, binFilePath) != 0) {
                perror(NULL);
                uppm_formula_free(formula);
                return UPPM_ERROR;
            }

            if (strcmp(actualSHA256SUM, formula->bin_sha) == 0) {
                needFetch = false;
            }
        }
    }

    if (needFetch) {
        ret = uppm_http_fetch_to_file(formula->bin_url, binFilePath, verbose, verbose);

        if (ret != UPPM_OK) {
            uppm_formula_free(formula);
            return ret;
        }

        char actualSHA256SUM[65] = {0};

        if (sha256sum_of_file(actualSHA256SUM, binFilePath) != 0) {
            perror(NULL);
            uppm_formula_free(formula);
            return UPPM_ERROR;
        }

        if (strcmp(actualSHA256SUM, formula->bin_sha) == 0) {
            fprintf(stderr, "%s already have been fetched.\n", binFilePath);
        } else {
            fprintf(stderr, "sha256sum mismatch.\n    expect : %s\n    actual : %s\n", formula->bin_sha, actualSHA256SUM);
            uppm_formula_free(formula);
            return UPPM_ERROR_SHA256_MISMATCH;
        }
    } else {
        fprintf(stderr, "%s already have been fetched.\n", binFilePath);
    }

    size_t   installedDirLength = userHomeDirLength + 20U;
    char     installedDir[installedDirLength];
    snprintf(installedDir, installedDirLength, "%s/.uppm/installed", userHomeDir);

    if (stat(installedDir, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            fprintf(stderr, "'%s\n' was expected to be a directory, but it was not.\n", installedDir);
            return UPPM_ERROR;
        }
    } else {
        if (mkdir(installedDir, S_IRWXU) != 0) {
            uppm_formula_free(formula);
            return UPPM_ERROR;
        }
    }

    if (stat(packageInstalledDir, &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            if (rm_r(packageInstalledDir, verbose) != 0) {
                uppm_formula_free(formula);
                return UPPM_ERROR;
            }
        } else {
            if (unlink(packageInstalledDir) != 0) {
                perror(packageInstalledDir);
                uppm_formula_free(formula);
                return UPPM_ERROR;
            }
        }
    }

    if (mkdir(packageInstalledDir, S_IRWXU) != 0) {
        uppm_formula_free(formula);
        return UPPM_ERROR;
    }

    if (formula->install == NULL) {
        ret = tar_extract(packageInstalledDir, binFilePath, ARCHIVE_EXTRACT_TIME, verbose, 1);

        if (ret != 0) {
            uppm_formula_free(formula);
            return abs(ret) + UPPM_ERROR_ARCHIVE_BASE;
        }
    } else {
        SysInfo sysinfo = {0};

        if (sysinfo_make(&sysinfo) != 0) {
            perror(NULL);
            uppm_formula_free(formula);
            return UPPM_ERROR;
        }

        char * libcName;

        switch(sysinfo.libc) {
            case 1:  libcName = (char*)"glibc"; break;
            case 2:  libcName = (char*)"musl";  break;
            default: libcName = (char*)"";
        }

        size_t   uppmHomeDirLength = strlen(userHomeDir) + 7U;
        char     uppmHomeDir[uppmHomeDirLength];
        snprintf(uppmHomeDir, uppmHomeDirLength, "%s/.uppm", userHomeDir);

        size_t   shellCodeLength = strlen(formula->install) + 1024U;
        char     shellCode[shellCodeLength];
        snprintf(shellCode, shellCodeLength,
                "set -ex\n\n"
                "NATIVE_OS_KIND='%s'\n"
                "NATIVE_OS_TYPE='%s'\n"
                "NATIVE_OS_NAME='%s'\n"
                "NATIVE_OS_VERS='%s'\n"
                "NATIVE_OS_LIBC='%s'\n"
                "NATIVE_OS_ARCH='%s'\n"
                "NATIVE_OS_NCPU='%u'\n\n"
                "UPPM_VERSION='%s'\n"
                "UPPM_VERSION_MAJOR='%d'\n"
                "UPPM_VERSION_MINOR='%d'\n"
                "UPPM_VERSION_PATCH='%d'\n"
                "UPPM_HOME='%s'\n\n"
                "UPPM_EXECUTABLE='%s'\n\n"
                "PKG_SUMMARY='%s'\n"
                "PKG_WEBPAGE='%s'\n"
                "PKG_VERSION='%s'\n"
                "PKG_BIN_URL='%s'\n"
                "PKG_BIN_SHA='%s'\n"
                "PKG_DEP_PKG='%s'\n"
                "PKG_BIN_FILENAME='%s'\n"
                "PKG_BIN_FILETYPE='%s'\n"
                "PKG_BIN_FILEPATH='%s'\n"
                "PKG_INSTALL_DIR='%s'\n\n"
                "%s",
                sysinfo.kind,
                sysinfo.type,
                sysinfo.name,
                sysinfo.vers,
                libcName,
                sysinfo.arch,
                sysinfo.ncpu,
                UPPM_VERSION,
                UPPM_VERSION_MAJOR,
                UPPM_VERSION_MINOR,
                UPPM_VERSION_PATCH,
                uppmHomeDir,
                "",
                formula->summary == NULL ? "" : formula->summary,
                formula->webpage == NULL ? "" : formula->webpage,
                formula->version == NULL ? "" : formula->version,
                formula->bin_url == NULL ? "" : formula->bin_url,
                formula->bin_sha == NULL ? "" : formula->bin_sha,
                formula->dep_pkg == NULL ? "" : formula->dep_pkg,
                binFileName,
                binFileNameExtension,
                binFilePath,
                packageInstalledDir,
                formula->install);

        sysinfo_free(sysinfo);

        printf("run shell code:\n%s\n", shellCode);

        int childProcessExitStatusCode = system(shellCode);

        if (childProcessExitStatusCode == -1) {
            perror(NULL);
            uppm_formula_free(formula);
            return UPPM_ERROR;
        }

        if (ret != 0) {
            if (WIFEXITED(childProcessExitStatusCode)) {
                fprintf(stderr, "running shell code exit with status code: %d\n", WEXITSTATUS(childProcessExitStatusCode));
            } else if (WIFSIGNALED(childProcessExitStatusCode)) {
                fprintf(stderr, "running shell code killed by signal: %d\n", WTERMSIG(childProcessExitStatusCode));
            } else if (WIFSTOPPED(childProcessExitStatusCode)) {
                fprintf(stderr, "running shell code stopped by signal: %d\n", WSTOPSIG(childProcessExitStatusCode));
            }

            uppm_formula_free(formula);
            return UPPM_ERROR;
        }
    }

    //////////////////////////////////////////////////////////////////////

    if (mkdir(packageInstalledMetaInfoDir, S_IRWXU) != 0) {
        perror(packageInstalledMetaInfoDir);
        uppm_formula_free(formula);
        return UPPM_ERROR;
    }

    //////////////////////////////////////////////////////////////////////

    ret = record_installed_files(packageInstalledDir);

    if (ret != UPPM_OK) {
        uppm_formula_free(formula);
        return UPPM_ERROR;
    }

    //////////////////////////////////////////////////////////////////////

    FILE * receiptFile = fopen(receiptFilePath, "w");

    if (receiptFile == NULL) {
        perror(receiptFilePath);
        uppm_formula_free(formula);
        return UPPM_ERROR;
    }

    FILE * formulaFile = fopen(formula->path, "r");

    if (formulaFile == NULL) {
        perror(formula->path);
        uppm_formula_free(formula);
        fclose(receiptFile);
        return UPPM_ERROR;
    }

    uppm_formula_free(formula);
    formula = NULL;

    fprintf(receiptFile, "pkgname: %s\n", packageName);

    char   buff[1024];
    size_t size;

    for (;;) {
        size = fread(buff, 1, 1024, formulaFile);

        if (ferror(formulaFile)) {
            fclose(formulaFile);
            fclose(receiptFile);
            return UPPM_ERROR;
        }

        if (size > 0) {
            if (fwrite(buff, 1, size, receiptFile) != size || ferror(receiptFile)) {
                fclose(formulaFile);
                fclose(receiptFile);
                return UPPM_ERROR;
            }
        }

        if (feof(formulaFile)) {
            fclose(formulaFile);
            break;
        }
    }

    fprintf(receiptFile, "\nsignature: %s\ntimestamp: %lu\n", UPPM_VERSION, time(NULL));

    fclose(receiptFile);

    fprintf(stderr, "\npackage [%s] successfully installed.\n", packageName);

    return UPPM_OK;
}

int uppm_install_the_given_packages(const char * packageNames[], size_t size) {
    for (size_t i = 0; i < size; i++) {
        const char * packageName = packageNames[i];

        UPPMFormula * formula = NULL;

        int ret = uppm_formula_lookup(packageName, &formula);

        if (ret != UPPM_OK) {
            return ret;
        }
    }
    return 0;
}
