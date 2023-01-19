#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <libgen.h>
#include <sys/stat.h>

#include "core/http.h"
#include "core/sysinfo.h"
#include "core/sha256sum.h"
#include "core/untar.h"
#include "core/rm-r.h"
#include "uppm.h"

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
        size_t depPackageNamesCopyLength = depPackageNamesLength + 1;
        char   depPackageNamesCopy[depPackageNamesCopyLength];
        memset(depPackageNamesCopy, 0, depPackageNamesCopyLength);
        strncpy(depPackageNamesCopy, formula->dep_pkg, depPackageNamesLength);

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

    char * userHomeDir = getenv("HOME");

    if (userHomeDir == NULL) {
        return UPPM_ERROR_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    if (userHomeDirLength == 0) {
        return UPPM_ERROR_ENV_HOME_NOT_SET;
    }

    struct stat st;

    size_t  packageInstalledDirLength = userHomeDirLength + strlen(packageName) + 20;
    char    packageInstalledDir[packageInstalledDirLength];
    memset (packageInstalledDir, 0, packageInstalledDirLength);
    snprintf(packageInstalledDir, packageInstalledDirLength, "%s/.uppm/installed/%s", userHomeDir, packageName);

    size_t  packageInstalledMetaInfoDirLength = packageInstalledDirLength + 6;
    char    packageInstalledMetaInfoDir[packageInstalledMetaInfoDirLength];
    memset (packageInstalledMetaInfoDir, 0, packageInstalledMetaInfoDirLength);
    snprintf(packageInstalledMetaInfoDir, packageInstalledMetaInfoDirLength, "%s/.uppm", packageInstalledDir);

    size_t  receiptFilePathLength = packageInstalledMetaInfoDirLength + 12;
    char    receiptFilePath[receiptFilePathLength];
    memset (receiptFilePath, 0, receiptFilePathLength);
    snprintf(receiptFilePath, receiptFilePathLength, "%s/receipt.yml", packageInstalledMetaInfoDir);

    if (stat(receiptFilePath, &st) == 0 && S_ISREG(st.st_mode)) {
        uppm_formula_free(formula);
        fprintf(stderr, "package [%s] already has been installed.\n", packageName);
        return UPPM_OK;
    }

    //////////////////////////////////////////////////////////////////////////

    fprintf(stderr, "prepare to install package [%s].\n", packageName);

    size_t  uppmDownloadDirLength = userHomeDirLength + 18;
    char    uppmDownloadDir[uppmDownloadDirLength];
    memset (uppmDownloadDir, 0, uppmDownloadDirLength);
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

    size_t  binFileNameLength = strlen(formula->bin_sha) + strlen(binFileNameExtension) + 1;
    char    binFileName[binFileNameLength];
    memset( binFileName, 0, binFileNameLength);
    snprintf(binFileName, binFileNameLength, "%s%s", formula->bin_sha, binFileNameExtension);

    size_t  binFilePathLength = uppmDownloadDirLength + binFileNameLength + 1;
    char    binFilePath[binFilePathLength];
    memset (binFilePath, 0, binFilePathLength);
    snprintf(binFilePath, binFilePathLength, "%s/%s", uppmDownloadDir, binFileName);

    bool needFetch = true;

    if (stat(binFilePath, &st) == 0) {
        if (S_ISREG(st.st_mode)) {
            char actualSHA256SUM[65] = {0};

            ret = sha256sum_of_file(actualSHA256SUM, binFilePath);

            if (ret != 0) {
                uppm_formula_free(formula);
                return ret;
            }

            if (strcmp(actualSHA256SUM, formula->bin_sha) == 0) {
                needFetch = false;
            }
        }
    }

    if (needFetch) {
        ret = http_fetch_to_file(formula->bin_url, binFilePath, verbose, verbose);

        if (ret != UPPM_OK) {
            uppm_formula_free(formula);
            return ret;
        }

        char actualSHA256SUM[65] = {0};

        ret = sha256sum_of_file(actualSHA256SUM, binFilePath);

        if (ret != 0) {
            uppm_formula_free(formula);
            return ret;
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

    size_t  installedDirLength = userHomeDirLength + 20;
    char    installedDir[installedDirLength];
    memset (installedDir, 0, installedDirLength);
    snprintf(installedDir, installedDirLength, "%s/.uppm/installed", userHomeDir);

    if (stat(installedDir, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            if (unlink(installedDir) != 0) {
                perror(installedDir);
                uppm_formula_free(formula);
                return UPPM_ERROR;
            }
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
        ret = untar_extract(packageInstalledDir, binFilePath, ARCHIVE_EXTRACT_TIME, verbose, 1);

        if (ret != 0) {
            uppm_formula_free(formula);
            return abs(ret) + UPPM_ERROR_ARCHIVE_BASE;
        }
    } else {
        SysInfo sysinfo = {0};

        ret = sysinfo_make(&sysinfo);

        if (ret != UPPM_OK) {
            uppm_formula_free(formula);
            return ret;
        }

        char * libcName = NULL;

        switch(sysinfo.libc) {
            case LIBC_GLIBC: libcName = (char*)"glibc"; break;
            case LIBC_MUSL:  libcName = (char*)"musl";  break;
            default:         libcName = (char*)"";
        }

        size_t  uppmHomeDirLength = strlen(userHomeDir) + 7;
        char    uppmHomeDir[uppmHomeDirLength];
        memset (uppmHomeDir, 0, uppmHomeDirLength);
        snprintf(uppmHomeDir, uppmHomeDirLength, "%s/.uppm", userHomeDir);

        size_t  shellCodeLength = strlen(formula->install) + 1024;
        char    shellCode[shellCodeLength];
        memset (shellCode, 0, shellCodeLength);
        snprintf(shellCode, shellCodeLength,
                "set -ex\n\n"
                "NATIVE_OS_KIND='%s'\n"
                "NATIVE_OS_TYPE='%s'\n"
                "NATIVE_OS_NAME='%s'\n"
                "NATIVE_OS_VERS='%s'\n"
                "NATIVE_OS_LIBC='%s'\n"
                "NATIVE_OS_ARCH='%s'\n"
                "NATIVE_OS_NCPU='%ld'\n\n"
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

        ret = system(shellCode);

        if (ret == -1) {
            perror(NULL);
        }

        if (ret != 0) {
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
        return UPPM_ERROR;
    }

    uppm_formula_free(formula);
    formula = NULL;

    fprintf(receiptFile, "pkgname: %s\n", packageName);

    char   buff[1024];
    size_t size = 0;
    while((size = fread(buff, 1, 1024, formulaFile)) != 0) {
        fwrite(buff, 1, size, receiptFile);
    }

    fclose(formulaFile);

    fprintf(receiptFile, "\nsignature: %s\ntimestamp: %lu\n", UPPM_VERSION, time(NULL));

    fclose(receiptFile);

    fprintf(stderr, "\npackage [%s] successfully installed.\n", packageName);

    return UPPM_OK;
}
