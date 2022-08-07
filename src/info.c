#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/fs.h"
#include "core/log.h"
#include "uppm.h"

int uppm_info_of_the_available_package(const char * packageName, const char * key) {
    int resultCode = uppm_is_package_name(packageName);

    if (resultCode != UPPM_OK) {
        return resultCode;
    }

    if (key == NULL || strcmp(key, "") == 0) {
        char * formulaFilePath = NULL;

        resultCode = uppm_formula_path(packageName, &formulaFilePath);

        if (resultCode != UPPM_OK) {
            fprintf(stderr, "package [%s] is not avaiable.\n", packageName);
            return resultCode;
        }

        FILE * formulaFile = fopen(formulaFilePath, "r");

        if (formulaFile == NULL) {
            perror(formulaFilePath);
            free(formulaFilePath);
            return UPPM_FORMULA_FILE_OPEN_ERROR;
        }

        printf("pkgname: %s%s%s\n", COLOR_GREEN, packageName, COLOR_OFF);

        char buff[1024];
        int  size = 0;
        while((size = fread(buff, 1, 1024, formulaFile)) != 0) {
            fwrite(buff, 1, size, stdout);
        }

        fclose(formulaFile);

        printf("formula: %s\n", formulaFilePath);

        free(formulaFilePath);
    } else if (strcmp(key, "formula") == 0) {
        char * formulaFilePath = NULL;

        resultCode = uppm_formula_path(packageName, &formulaFilePath);

        if (resultCode == UPPM_OK) {
            printf("%s\n", formulaFilePath);
        } else {
            fprintf(stderr, "package [%s] is not avaiable.\n", packageName);
        }

        free(formulaFilePath);

        return resultCode;
    } else if (strcmp(key, "summary") == 0) {
        UPPMFormula * formula = NULL;

        resultCode = uppm_formula_parse(packageName, &formula);

        if (resultCode != UPPM_OK) {
            return resultCode;
        }

        if (formula->summary != NULL) {
            printf("%s\n", formula->summary);
        }

        uppm_formula_free(formula);
    } else if (strcmp(key, "webpage") == 0) {
        UPPMFormula * formula = NULL;

        resultCode = uppm_formula_parse(packageName, &formula);

        if (resultCode != UPPM_OK) {
            return resultCode;
        }

        if (formula->webpage != NULL) {
            printf("%s\n", formula->webpage);
        }

        uppm_formula_free(formula);
    } else if (strcmp(key, "version") == 0) {
        UPPMFormula * formula = NULL;

        resultCode = uppm_formula_parse(packageName, &formula);

        if (resultCode != UPPM_OK) {
            return resultCode;
        }

        if (formula->version != NULL) {
            printf("%s\n", formula->version);
        }

        uppm_formula_free(formula);
    } else if (strcmp(key, "bin-url") == 0) {
        UPPMFormula * formula = NULL;

        resultCode = uppm_formula_parse(packageName, &formula);

        if (resultCode != UPPM_OK) {
            return resultCode;
        }

        if (formula->bin_url != NULL) {
            printf("%s\n", formula->bin_url);
        }

        uppm_formula_free(formula);
    } else if (strcmp(key, "bin-sha") == 0) {
        UPPMFormula * formula = NULL;

        resultCode = uppm_formula_parse(packageName, &formula);

        if (resultCode != UPPM_OK) {
            return resultCode;
        }

        if (formula->bin_sha != NULL) {
            printf("%s\n", formula->bin_sha);
        }

        uppm_formula_free(formula);
    } else if (strcmp(key, "install") == 0) {
        UPPMFormula * formula = NULL;

        resultCode = uppm_formula_parse(packageName, &formula);

        if (resultCode != UPPM_OK) {
            return resultCode;
        }

        if (formula->install != NULL) {
            printf("%s\n", formula->install);
        }

        uppm_formula_free(formula);
    } else {
        return UPPM_INFO_UNRECOGNIZED_KEY;
    }

    return UPPM_OK;
}

int uppm_info_of_the_installed_package(const char * packageName, const char * key) {
    int resultCode = uppm_is_package_name(packageName);

    if (resultCode != UPPM_OK) {
        return resultCode;
    }

    if (key == NULL || strcmp(key, "") == 0) {
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

        size_t  installedMetadataFilePathLength = installDirLength + 25;
        char    installedMetadataFilePath[installedMetadataFilePathLength];
        memset (installedMetadataFilePath, 0, installedMetadataFilePathLength);
        sprintf(installedMetadataFilePath, "%s/uppm-installed-metadata", installDir);

        if (!exists_and_is_a_regular_file(installedMetadataFilePath)) {
            return UPPM_PACKAGE_IS_NOT_INSTALLED;
        }

        FILE * installedMetadataFile = fopen(installedMetadataFilePath, "r");

        if (installedMetadataFile == NULL) {
            perror(installedMetadataFilePath);
            return UPPM_INSTALLED_METADATA_FILE_OPEN_ERROR;
        }

        char buff[1024];
        int  size = 0;
        while((size = fread(buff, 1, 1024, installedMetadataFile)) != 0) {
            fwrite(buff, 1, size, stdout);
        }

        fclose(installedMetadataFile);

        printf("location: %s\n", installDir);
    }

    return UPPM_OK;
}
