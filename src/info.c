#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <jansson.h>

#include "core/fs.h"
#include "core/log.h"
#include "uppm.h"

int uppm_info(const char * packageName, const char * key) {
    int resultCode = uppm_is_package_name(packageName);

    if (resultCode != UPPM_OK) {
        return resultCode;
    }

    if (key == NULL || strcmp(key, "") == 0) {
        char * formulaFilePath = NULL;

        resultCode = uppm_formula_path(packageName, &formulaFilePath);

        if (resultCode != UPPM_OK) {
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
    } else if (strcmp(key, "dep-pkg") == 0) {
        UPPMFormula * formula = NULL;

        resultCode = uppm_formula_parse(packageName, &formula);

        if (resultCode != UPPM_OK) {
            return resultCode;
        }

        if (formula->dep_pkg != NULL) {
            printf("%s\n", formula->dep_pkg);
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
    } else if (strcmp(key, "installed-dir") == 0) {
        char * userHomeDir = getenv("HOME");

        if (userHomeDir == NULL || strcmp(userHomeDir, "") == 0) {
            return UPPM_ENV_HOME_NOT_SET;
        }

        size_t userHomeDirLength = strlen(userHomeDir);

        size_t  installedDirLength = userHomeDirLength + strlen(packageName) + 20;
        char    installedDir[installedDirLength];
        memset (installedDir, 0, installedDirLength);
        sprintf(installedDir, "%s/.uppm/installed/%s", userHomeDir, packageName);

        size_t  installedMetadataFilePathLength = installedDirLength + 25;
        char    installedMetadataFilePath[installedMetadataFilePathLength];
        memset (installedMetadataFilePath, 0, installedMetadataFilePathLength);
        sprintf(installedMetadataFilePath, "%s/uppm-installed-metadata", installedDir);

        if (exists_and_is_a_regular_file(installedMetadataFilePath)) {
            printf("%s\n", installedDir);
        } else {
            return UPPM_PACKAGE_IS_NOT_INSTALLED;
        }
    } else if (strcmp(key, "installed-metadata-path") == 0) {
        char * userHomeDir = getenv("HOME");

        if (userHomeDir == NULL || strcmp(userHomeDir, "") == 0) {
            return UPPM_ENV_HOME_NOT_SET;
        }

        size_t userHomeDirLength = strlen(userHomeDir);

        size_t  installedDirLength = userHomeDirLength + strlen(packageName) + 20;
        char    installedDir[installedDirLength];
        memset (installedDir, 0, installedDirLength);
        sprintf(installedDir, "%s/.uppm/installed/%s", userHomeDir, packageName);

        size_t  installedMetadataFilePathLength = installedDirLength + 25;
        char    installedMetadataFilePath[installedMetadataFilePathLength];
        memset (installedMetadataFilePath, 0, installedMetadataFilePathLength);
        sprintf(installedMetadataFilePath, "%s/uppm-installed-metadata", installedDir);

        if (exists_and_is_a_regular_file(installedMetadataFilePath)) {
            printf("%s\n", installedMetadataFilePath);
        } else {
            return UPPM_PACKAGE_IS_NOT_INSTALLED;
        }
    } else if (strcmp(key, "installed-metadata-yaml") == 0) {
        char * userHomeDir = getenv("HOME");

        if (userHomeDir == NULL || strcmp(userHomeDir, "") == 0) {
            return UPPM_ENV_HOME_NOT_SET;
        }

        size_t userHomeDirLength = strlen(userHomeDir);

        size_t  installedDirLength = userHomeDirLength + strlen(packageName) + 20;
        char    installedDir[installedDirLength];
        memset (installedDir, 0, installedDirLength);
        sprintf(installedDir, "%s/.uppm/installed/%s", userHomeDir, packageName);

        size_t  installedMetadataFilePathLength = installedDirLength + 25;
        char    installedMetadataFilePath[installedMetadataFilePathLength];
        memset (installedMetadataFilePath, 0, installedMetadataFilePathLength);
        sprintf(installedMetadataFilePath, "%s/uppm-installed-metadata", installedDir);

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

        printf("location: %s\n", installedDir);
    } else if (strcmp(key, "installed-metadata-json") == 0) {
        UPPMInstalledMetadata * metadata = NULL;

        int resultCode = uppm_installed_metadata_parse(packageName, &metadata);

        if (resultCode != UPPM_OK) {
            return resultCode;
        }

        json_t * root = json_object();

        json_object_set_new(root, "summary", json_string(metadata->summary));
        json_object_set_new(root, "webpage", json_string(metadata->webpage));
        json_object_set_new(root, "version", json_string(metadata->version));
        json_object_set_new(root, "bin-url", json_string(metadata->bin_url));
        json_object_set_new(root, "bin-sha", json_string(metadata->bin_sha));

        if ((metadata->install != NULL) && (strcmp(metadata->install, "") != 0)) {
            json_object_set_new(root, "install", json_string(metadata->install));
        }

        char * jsonStr = json_dumps(root, 0);

        if (jsonStr == NULL) {
            resultCode = UPPM_ERROR;
        } else {
            printf("%s\n", jsonStr);
            free(jsonStr);
        }

        json_decref(root);

        uppm_installed_metadata_free(metadata);
    } else if (strcmp(key, "installed-package-version") == 0) {
        UPPMInstalledMetadata * metadata = NULL;

        int resultCode = uppm_installed_metadata_parse(packageName, &metadata);

        if (resultCode != UPPM_OK) {
            return resultCode;
        }

        printf("%s\n", metadata->version);

        uppm_installed_metadata_free(metadata);
    } else if (strcmp(key, "installed-datetime-unix") == 0) {
        UPPMInstalledMetadata * metadata = NULL;

        int resultCode = uppm_installed_metadata_parse(packageName, &metadata);

        if (resultCode != UPPM_OK) {
            return resultCode;
        }

        printf("%s\n", metadata->datatime);

        uppm_installed_metadata_free(metadata);
    } else if (strcmp(key, "installed-datetime-formatted") == 0) {
        UPPMInstalledMetadata * metadata = NULL;

        int resultCode = uppm_installed_metadata_parse(packageName, &metadata);

        if (resultCode != UPPM_OK) {
            return resultCode;
        }

        time_t tt = (time_t)atol(metadata->datatime);
        struct tm *tms = localtime(&tt);

        char buff[40] = {0};
        strftime(buff, sizeof(buff), "%Y-%m-%d %H:%M:%S %z %Z", tms);

        printf("%s\n", buff);

        uppm_installed_metadata_free(metadata);
    } else {
        return UPPM_INFO_UNRECOGNIZED_KEY;
    }

    return UPPM_OK;
}
