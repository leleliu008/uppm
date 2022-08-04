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

int uppm_install(const char * packageName) {
    char * userHomeDir = getenv("HOME");

    if (userHomeDir == NULL || strcmp(userHomeDir, "") == 0) {
        fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        return UPPM_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    size_t  installDirLength = userHomeDirLength + strlen(packageName) + 20;
    char    installDir[installDirLength];
    memset (installDir, 0, installDirLength);
    sprintf(installDir, "%s/.uppm/installed/%s", userHomeDir, packageName);

    size_t  installedMetadataFilePathLength = installDirLength + 26;
    char    installedMetadataFilePath[installedMetadataFilePathLength];
    memset (installedMetadataFilePath, 0, installedMetadataFilePathLength);
    sprintf(installedMetadataFilePath, "%s/uppm-installed-metadata", installDir);

    if (exists_and_is_a_directory(installDir)) {
        if (exists_and_is_a_regular_file(installedMetadataFilePath)) {
            fprintf(stderr, "package [%s] already has been installed.\n", packageName);
            return UPPM_OK;
        } else {
            if (rm_r(installDir) != 0) {
                return UPPM_ERROR;
            }
        }
    }

    UPPMFormula * formula = NULL;

    int resultCode = uppm_formula_parse(packageName, &formula);

    if (resultCode != UPPM_OK) {
        return resultCode;
    }

    size_t  urlLength = strlen(formula->bin_url);
    size_t  urlCopyLength = urlLength + 1;
    char    urlCopy[urlCopyLength];
    memset (urlCopy, 0, urlCopyLength);
    strcpy(urlCopy, formula->bin_url);

    const char * binFileName = basename(urlCopy);

    size_t  binDirLength = userHomeDirLength + 18;
    char    binDir[binDirLength];
    memset (binDir, 0, binDirLength);
    sprintf(binDir, "%s/.uppm/downloads", userHomeDir);

    if (!exists_and_is_a_directory(binDir)) {
        resultCode = mkdir(binDir, S_IRWXU);

        if (resultCode != UPPM_OK) {
            uppm_formula_free(formula);
            return resultCode;
        }
    }

    size_t  binFilePathLength = binDirLength + strlen(binFileName) + 2;
    char    binFilePath[binFilePathLength];
    memset (binFilePath, 0, binFilePathLength);
    sprintf(binFilePath, "%s/%s", binDir, binFileName);

    bool needFetch = true;

    if (exists_and_is_a_regular_file(binFilePath)) {
        char * actualSHA256SUM = sha256sum_of_file(binFilePath);

        if (strcmp(actualSHA256SUM, formula->bin_sha) == 0) {
            needFetch = false;
        }

        free(actualSHA256SUM);
    }

    if (needFetch) {
        resultCode = http_fetch_to_file(formula->bin_url, binFilePath, true, true);

        if (resultCode != UPPM_OK) {
            uppm_formula_free(formula);
            return resultCode;
        }

        char * actualSHA256SUM = sha256sum_of_file(binFilePath);

        if (strcmp(actualSHA256SUM, formula->bin_sha) == 0) {
            fprintf(stderr, "%s already have been fetched.\n", binFilePath);
            free(actualSHA256SUM);
        } else {
            fprintf(stderr, "sha256sum mismatch.\n    expect : %s\n    actual : %s\n", formula->bin_sha, actualSHA256SUM);
            free(actualSHA256SUM);
            uppm_formula_free(formula);
            return UPPM_SHA256_MISMATCH;
        }
    } else {
        fprintf(stderr, "%s already have been fetched.\n", binFilePath);
    }

    if (formula->install == NULL) {
        resultCode = untar_extract(installDir, binFilePath, ARCHIVE_EXTRACT_TIME, true, 1);

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
                binFilePath,
                installDir,
                formula->install);

        printf("run shell code:\n%s\n", shellCode);

        resultCode = system(shellCode);

        if (resultCode != UPPM_OK) {
            uppm_formula_free(formula);
            return resultCode;
        }
    }

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

    char buff[1024];
    int  size = 0;
    while((size = fread(buff, 1, 1024, formulaFile)) != 0) {
        fwrite(buff, 1, size, installedMetadataFile);
    }

    fclose(formulaFile);

    fprintf(installedMetadataFile, "pkgname: %s\ndatatime: %lu\nuppmvers: %s\n", packageName, time(NULL), UPPM_VERSION);

    fclose(installedMetadataFile);

    return UPPM_OK;
}
