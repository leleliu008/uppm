#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fs.h"
#include "log.h"
#include "uppm.h"

int uppm_info(const char * pkgName) {
    char * formulaFilePath = NULL;

    int resultCode = uppm_formula_path(pkgName, &formulaFilePath);

    if (resultCode != UPPM_OK) {
        fprintf(stderr, "package [%s] is not avaiable.\n", pkgName);
        return resultCode;
    }

    FILE * formulaFile = fopen(formulaFilePath, "r");

    if (formulaFile == NULL) {
        perror(formulaFilePath);
        free(formulaFilePath);
        return UPPM_FORMULA_FILE_OPEN_ERROR;
    }

    printf("pkgname: %s%s%s\n", COLOR_GREEN, pkgName, COLOR_OFF);

    char buff[1024];
    int  size = 0;
    while((size = fread(buff, 1, 1024, formulaFile)) != 0) {
        fwrite(buff, 1, size, stdout);
    }

    fclose(formulaFile);

    printf("formula: %s\n", formulaFilePath);

    free(formulaFilePath);
    formulaFilePath = NULL;

    ////////////////////////////////////////////////////////////

    char * userHomeDir = getenv("HOME");

    if (userHomeDir == NULL || strcmp(userHomeDir, "") == 0) {
        fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        return UPPM_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    size_t  installDirLength = userHomeDirLength + strlen(pkgName) + 20;
    char    installDir[installDirLength];
    memset (installDir, 0, installDirLength);
    sprintf(installDir, "%s/.uppm/installed/%s", userHomeDir, pkgName);

    size_t  installedMetadataFilePathLength = installDirLength + 25;
    char    installedMetadataFilePath[installedMetadataFilePathLength];
    memset (installedMetadataFilePath, 0, installedMetadataFilePathLength);
    sprintf(installedMetadataFilePath, "%s/uppm-installed-metadata", installDir);

    if (!exists_and_is_a_regular_file(installedMetadataFilePath)) {
        return UPPM_OK;
    }

    FILE * installedMetadataFile = fopen(installedMetadataFilePath, "r");

    if (installedMetadataFile == NULL) {
        perror(installedMetadataFilePath);
        return UPPM_INSTALLED_METADATA_FILE_OPEN_ERROR;
    }

    LOG_GREEN("\ninstalled:");

    size = 0;
    while((size = fread(buff, 1, 1024, installedMetadataFile)) != 0) {
        fwrite(buff, 1, size, stdout);
    }

    fclose(installedMetadataFile);

    printf("location: %s\n", installDir);

    return UPPM_OK;
}
