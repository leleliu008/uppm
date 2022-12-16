#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <libgen.h>
#include <sys/stat.h>

#include "core/fs.h"
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

        int resultCode = uppm_formula_parse(packageName, &formula);

        if (resultCode != UPPM_OK) {
            return resultCode;
        }
    }
    return 0;
}

extern int record_installed_files(const char * installedDirPath);

int uppm_install(const char * packageName, bool verbose) {
    fprintf(stderr, "prepare to install package [%s].\n", packageName);

    char * userHomeDir = getenv("HOME");

    if (userHomeDir == NULL || strcmp(userHomeDir, "") == 0) {
        return UPPM_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    size_t  packageInstalledDirLength = userHomeDirLength + strlen(packageName) + 20;
    char    packageInstalledDir[packageInstalledDirLength];
    memset (packageInstalledDir, 0, packageInstalledDirLength);
    sprintf(packageInstalledDir, "%s/.uppm/installed/%s", userHomeDir, packageName);

    size_t  packageInstalledMetaInfoDirLength = packageInstalledDirLength + 6;
    char    packageInstalledMetaInfoDir[packageInstalledMetaInfoDirLength];
    memset (packageInstalledMetaInfoDir, 0, packageInstalledMetaInfoDirLength);
    sprintf(packageInstalledMetaInfoDir, "%s/.uppm", packageInstalledDir);

    size_t  receiptFilePathLength = packageInstalledMetaInfoDirLength + 12;
    char    receiptFilePath[receiptFilePathLength];
    memset (receiptFilePath, 0, receiptFilePathLength);
    sprintf(receiptFilePath, "%s/receipt.yml", packageInstalledMetaInfoDir);

    if (exists_and_is_a_regular_file(receiptFilePath)) {
        fprintf(stderr, "package [%s] already has been installed.\n", packageName);
        return UPPM_OK;
    }

    UPPMFormula * formula = NULL;

    int resultCode = uppm_formula_parse(packageName, &formula);

    if (resultCode != UPPM_OK) {
        return resultCode;
    }

    if ((formula->dep_pkg != NULL) && (strcmp(formula->dep_pkg, "") != 0)) {
        size_t depPackageNamesLength = strlen(formula->dep_pkg);
        size_t depPackageNamesCopyLength = depPackageNamesLength + 1;
        char   depPackageNamesCopy[depPackageNamesCopyLength];
        memset(depPackageNamesCopy, 0, depPackageNamesCopyLength);
        strcpy(depPackageNamesCopy, formula->dep_pkg);

        size_t index = 0;
        char * depPackageNameList[10];

        char * depPackageName = strtok(depPackageNamesCopy, " ");

        while (depPackageName != NULL) {
            depPackageNameList[index] = depPackageName;
            index++;
            depPackageName = strtok (NULL, " ");
        }

        for (size_t i = 0; i < index; i++) {
            resultCode = uppm_install(depPackageNameList[i], verbose);

            if (resultCode != UPPM_OK) {
                uppm_formula_free(formula);
                return resultCode;
            }
        }
    }

    size_t  urlLength = strlen(formula->bin_url);
    size_t  urlCopyLength = urlLength + 1;
    char    urlCopy[urlCopyLength];
    memset (urlCopy, 0, urlCopyLength);
    strcpy(urlCopy, formula->bin_url);

    const char * archiveFileName = basename(urlCopy);

    size_t  downloadDirLength = userHomeDirLength + 18;
    char    downloadDir[downloadDirLength];
    memset (downloadDir, 0, downloadDirLength);
    sprintf(downloadDir, "%s/.uppm/downloads", userHomeDir);

    if (!exists_and_is_a_directory(downloadDir)) {
        if (mkdir(downloadDir, S_IRWXU) != 0) {
            uppm_formula_free(formula);
            return UPPM_ERROR;
        }
    }

    size_t  archiveFilePathLength = downloadDirLength + strlen(archiveFileName) + 2;
    char    archiveFilePath[archiveFilePathLength];
    memset (archiveFilePath, 0, archiveFilePathLength);
    sprintf(archiveFilePath, "%s/%s", downloadDir, archiveFileName);

    bool needFetch = true;

    if (exists_and_is_a_regular_file(archiveFilePath)) {
        char * actualSHA256SUM = sha256sum_of_file(archiveFilePath);

        if (strcmp(actualSHA256SUM, formula->bin_sha) == 0) {
            needFetch = false;
        }

        free(actualSHA256SUM);
    }

    if (needFetch) {
        if (http_fetch_to_file(formula->bin_url, archiveFilePath, verbose, verbose) != 0) {
            uppm_formula_free(formula);
            return UPPM_NETWORK_ERROR;
        }

        char * actualSHA256SUM = sha256sum_of_file(archiveFilePath);

        if (strcmp(actualSHA256SUM, formula->bin_sha) == 0) {
            fprintf(stderr, "%s already have been fetched.\n", archiveFilePath);
            free(actualSHA256SUM);
        } else {
            fprintf(stderr, "sha256sum mismatch.\n    expect : %s\n    actual : %s\n", formula->bin_sha, actualSHA256SUM);
            free(actualSHA256SUM);
            uppm_formula_free(formula);
            return UPPM_SHA256_MISMATCH;
        }
    } else {
        fprintf(stderr, "%s already have been fetched.\n", archiveFilePath);
    }

    if (exists_and_is_a_directory(packageInstalledDir)) {
        if (rm_r(packageInstalledDir, verbose) != 0) {
            uppm_formula_free(formula);
            return UPPM_ERROR;
        }
    } else {
        size_t  installedDirLength = userHomeDirLength + 20;
        char    installedDir[installedDirLength];
        memset (installedDir, 0, installedDirLength);
        sprintf(installedDir, "%s/.uppm/installed", userHomeDir);

        if (!exists_and_is_a_directory(installedDir)) {
            if (mkdir(installedDir, S_IRWXU) != 0) {
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
        resultCode = untar_extract(packageInstalledDir, archiveFilePath, ARCHIVE_EXTRACT_TIME, verbose, 1);

        if (resultCode != UPPM_OK) {
            uppm_formula_free(formula);
            return resultCode;
        }
    } else {
        SysInfo * sysinfo = NULL;

        if (sysinfo_make(&sysinfo) != 0) {
            uppm_formula_free(formula);
            return UPPM_ERROR;
        }

        char * libcName = NULL;

        switch(sysinfo->libc) {
            case LIBC_GLIBC: libcName = (char*)"glibc"; break;
            case LIBC_MUSL:  libcName = (char*)"musl";  break;
            default:         libcName = (char*)"unknown";
        }

        size_t  uppmHomeDirLength = strlen(userHomeDir) + 7;
        char    uppmHomeDir[uppmHomeDirLength];
        memset (uppmHomeDir, 0, uppmHomeDirLength);
        sprintf(uppmHomeDir, "%s/.uppm", userHomeDir);

        size_t  shellCodeLength = strlen(formula->install) + 512;
        char    shellCode[shellCodeLength];
        memset (shellCode, 0, shellCodeLength);
        sprintf(shellCode,
                "set -ex\n\n"
                "NATIVE_OS_KIND='%s'\n"
                "NATIVE_OS_TYPE='%s'\n"
                "NATIVE_OS_NAME='%s'\n"
                "NATIVE_OS_VERS='%s'\n"
                "NATIVE_OS_LIBC='%s'\n"
                "NATIVE_OS_ARCH='%s'\n"
                "NATIVE_OS_NCPU='%ld'\n\n"
                "UPPM_VERSION='%s'\n"
                "UPPM_HOME='%s'\n\n"
                "PKG_SUMMARY='%s'\n"
                "PKG_WEBPAGE='%s'\n"
                "PKG_VERSION='%s'\n"
                "PKG_BIN_URL='%s'\n"
                "PKG_BIN_SHA='%s'\n"
                "PKG_DEP_PKG='%s'\n"
                "PKG_BIN_FILEPATH='%s'\n"
                "PKG_INSTALL_DIR='%s'\n\n"
                "%s",
                sysinfo->kind,
                sysinfo->type,
                sysinfo->name,
                sysinfo->vers,
                libcName,
                sysinfo->arch,
                sysinfo->ncpu,
                UPPM_VERSION,
                uppmHomeDir,
                formula->summary == NULL ? "" : formula->summary,
                formula->web_url == NULL ? "" : formula->web_url,
                formula->version == NULL ? "" : formula->version,
                formula->bin_url == NULL ? "" : formula->bin_url,
                formula->bin_sha == NULL ? "" : formula->bin_sha,
                formula->dep_pkg == NULL ? "" : formula->dep_pkg,
                archiveFilePath,
                packageInstalledDir,
                formula->install);

        sysinfo_free(sysinfo);

        printf("run shell code:\n%s\n", shellCode);

        resultCode = system(shellCode);

        if (resultCode != 0) {
            uppm_formula_free(formula);
            return resultCode;
        }
    }

    //////////////////////////////////////////////////////////////////////

    if (mkdir(packageInstalledMetaInfoDir, S_IRWXU) != 0) {
        uppm_formula_free(formula);
        return UPPM_ERROR;
    }

    //////////////////////////////////////////////////////////////////////

    resultCode = record_installed_files(packageInstalledDir);

    if (resultCode != UPPM_OK) {
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

    fprintf(stderr, "package [%s] successfully installed.\n", packageName);

    return UPPM_OK;
}
