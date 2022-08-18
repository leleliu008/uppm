#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <libgen.h>
#include <sys/stat.h>

#include "core/fs.h"
#include "core/http.h"
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
}

extern int record_installed_files(const char * installedDirPath);

int uppm_install(const char * packageName, bool verbose) {
    fprintf(stderr, "prepare to install package [%s].\n", packageName);

    char * userHomeDir = getenv("HOME");

    if (userHomeDir == NULL || strcmp(userHomeDir, "") == 0) {
        return UPPM_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    size_t  packageInstallDirLength = userHomeDirLength + strlen(packageName) + 20;
    char    packageInstallDir[packageInstallDirLength];
    memset (packageInstallDir, 0, packageInstallDirLength);
    sprintf(packageInstallDir, "%s/.uppm/installed/%s", userHomeDir, packageName);

    size_t  installedMetadataFilePathLength = packageInstallDirLength + 26;
    char    installedMetadataFilePath[installedMetadataFilePathLength];
    memset (installedMetadataFilePath, 0, installedMetadataFilePathLength);
    sprintf(installedMetadataFilePath, "%s/installed-metadata-uppm", packageInstallDir);

    if (exists_and_is_a_regular_file(installedMetadataFilePath)) {
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
        resultCode = http_fetch_to_file(formula->bin_url, archiveFilePath, true, true);

        if (resultCode != UPPM_OK) {
            uppm_formula_free(formula);
            return resultCode;
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

    if (exists_and_is_a_directory(packageInstallDir)) {
        if (rm_r(packageInstallDir, verbose) != 0) {
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

    if (mkdir(packageInstallDir, S_IRWXU) != 0) {
        uppm_formula_free(formula);
        return UPPM_ERROR;
    }

    if (formula->install == NULL) {
        resultCode = untar_extract(packageInstallDir, archiveFilePath, ARCHIVE_EXTRACT_TIME, verbose, 1);

        if (resultCode != UPPM_OK) {
            uppm_formula_free(formula);
            return resultCode;
        }
    } else {
        size_t  shellCodeLength = strlen(formula->install) + 512;
        char    shellCode[shellCodeLength];
        memset (shellCode, 0, shellCodeLength);
        sprintf(shellCode,
                "set -ex\n"
                "NATIVE_OS_KIND='%s'\n"
                "NATIVE_OS_NAME='%s'\n"
                "NATIVE_OS_ARCH='%s'\n"
                "UPPM_VERSION='%s'\n"
                "UPPM_HOME='%s'\n"
                "PKG_SUMMARY='%s'\n"
                "PKG_WEBPAGE='%s'\n"
                "PKG_VERSION='%s'\n"
                "PKG_BIN_URL='%s'\n"
                "PKG_BIN_SHA='%s'\n"
                "PKG_DEP_PKG='%s'\n"
                "PKG_BIN_FILEPATH='%s'\n"
                "PKG_INSTALL_DIR='%s'\n"
                "%s",
                getenv("NATIVE_OS_KIND"),
                getenv("NATIVE_OS_NAME"),
                getenv("NATIVE_OS_ARCH"),
                getenv("UPPM_VERSION"),
                getenv("UPPM_HOME"),
                formula->summary == NULL ? "" : formula->summary,
                formula->webpage == NULL ? "" : formula->webpage,
                formula->version == NULL ? "" : formula->version,
                formula->bin_url == NULL ? "" : formula->bin_url,
                formula->bin_sha == NULL ? "" : formula->bin_sha,
                formula->dep_pkg == NULL ? "" : formula->dep_pkg,
                archiveFilePath,
                packageInstallDir,
                formula->install);

        printf("run shell code:\n%s\n", shellCode);

        resultCode = system(shellCode);

        if (resultCode != UPPM_OK) {
            uppm_formula_free(formula);
            return resultCode;
        }
    }

    //////////////////////////////////////////////////////////////////////

    resultCode = record_installed_files(packageInstallDir);

    if (resultCode != UPPM_OK) {
        uppm_formula_free(formula);
        return UPPM_ERROR;
    }

    //////////////////////////////////////////////////////////////////////

    FILE * installedMetadataFile = fopen(installedMetadataFilePath, "w");

    if (installedMetadataFile == NULL) {
        perror(installedMetadataFilePath);
        uppm_formula_free(formula);
        return UPPM_ERROR;
    }

    FILE * formulaFile = fopen(formula->path, "r");

    if (formulaFile == NULL) {
        perror(formula->path);
        uppm_formula_free(formula);
        return UPPM_FORMULA_FILE_OPEN_ERROR;
    }

    uppm_formula_free(formula);
    formula = NULL;

    char   buff[1024];
    size_t size = 0;
    while((size = fread(buff, 1, 1024, formulaFile)) != 0) {
        fwrite(buff, 1, size, installedMetadataFile);
    }

    fclose(formulaFile);

    fprintf(installedMetadataFile, "pkgname: %s\ndatatime: %lu\nuppmvers: %s\n", packageName, time(NULL), UPPM_VERSION);

    fclose(installedMetadataFile);

    return UPPM_OK;
}
