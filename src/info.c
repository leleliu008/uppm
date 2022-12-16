#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <jansson.h>

#include "core/fs.h"
#include "core/log.h"
#include "uppm.h"

#include <dirent.h>
#include <fnmatch.h>

int uppm_info_all_available_packages(const char * key) {
    UPPMFormulaRepoList * formulaRepoList = NULL;

    int resultCode = uppm_formula_repo_list_new(&formulaRepoList);

    if (resultCode != UPPM_OK) {
        return resultCode;
    }

    bool isFirst = true;

    for (size_t i = 0; i < formulaRepoList->size; i++) {
        char *  formulaRepoPath  = formulaRepoList->repos[i]->path;
        size_t  formulaDirLength = strlen(formulaRepoPath) + 10;
        char    formulaDir[formulaDirLength];
        memset (formulaDir, 0, formulaDirLength);
        sprintf(formulaDir, "%s/formula", formulaRepoPath);

        DIR           * dir;
        struct dirent * dir_entry;

        dir = opendir(formulaDir);

        if (dir == NULL) {
            uppm_formula_repo_list_free(formulaRepoList);
            perror(formulaDir);
            return UPPM_ERROR;
        }

        while ((dir_entry = readdir(dir))) {
            //puts(dir_entry->d_name);
            if ((strcmp(dir_entry->d_name, ".") == 0) || (strcmp(dir_entry->d_name, "..") == 0)) {
                continue;
            }

            int r = fnmatch("*.yml", dir_entry->d_name, 0);

            if (r == 0) {
                size_t  fileNameLength = strlen(dir_entry->d_name);
                char    packageName[fileNameLength];
                memset (packageName, 0, fileNameLength);
                strncpy(packageName, dir_entry->d_name, fileNameLength - 4);

                if (!isFirst) {
                    printf("\n");
                }

                //printf("%s\n", packageName);
                resultCode = uppm_info(packageName, key);

                if (resultCode != UPPM_OK) {
                    uppm_formula_repo_list_free(formulaRepoList);
                    closedir(dir);
                    return resultCode;
                }

                if (isFirst) {
                    isFirst = false;
                }
            } else if(r == FNM_NOMATCH) {
                ;
            } else {
                uppm_formula_repo_list_free(formulaRepoList);
                fprintf(stderr, "fnmatch() error\n");
                closedir(dir);
                return UPPM_ERROR;
            }
        }

        closedir(dir);
    }

    uppm_formula_repo_list_free(formulaRepoList);

    return UPPM_OK;
}

int uppm_info(const char * packageName, const char * key) {
    if (packageName == NULL) {
        return UPPM_ARG_IS_NULL;
    }

    if (strcmp(packageName, "") == 0) {
        return UPPM_ARG_IS_EMPTY;
    }

    if (strcmp(packageName, "@all") == 0) {
        return uppm_info_all_available_packages(key);
    }

    int resultCode = uppm_check_if_the_given_argument_matches_package_name_pattern(packageName);

    if (resultCode != UPPM_OK) {
        return resultCode;
    }

    if ((key == NULL) || (strcmp(key, "") == 0) || (strcmp(key, "--yaml") == 0)) {
        char * formulaFilePath = NULL;

        resultCode = uppm_formula_path(packageName, &formulaFilePath);

        if (resultCode != UPPM_OK) {
            return resultCode;
        }

        FILE * formulaFile = fopen(formulaFilePath, "r");

        if (formulaFile == NULL) {
            perror(formulaFilePath);
            free(formulaFilePath);
            return UPPM_ERROR;
        }

        if (isatty(STDOUT_FILENO)) {
            if (uppm_check_if_the_given_package_is_installed(packageName) == UPPM_OK) {
                printf("pkgname: %s%s%s\n", COLOR_GREEN, packageName, COLOR_OFF);
            } else {
                printf("pkgname: %s%s%s\n", COLOR_RED,   packageName, COLOR_OFF);
            }
        } else {
            printf("pkgname: %s\n", packageName);
        }

        char buff[1024];
        int  size = 0;
        while((size = fread(buff, 1, 1024, formulaFile)) != 0) {
            fwrite(buff, 1, size, stdout);
        }

        fclose(formulaFile);

        printf("formula: %s\n", formulaFilePath);

        free(formulaFilePath);
    } else if (strcmp(key, "--json") == 0) {
        UPPMFormula * formula = NULL;

        resultCode = uppm_formula_parse(packageName, &formula);

        if (resultCode != UPPM_OK) {
            return resultCode;
        }

        json_t * root = json_object();

        json_object_set_new(root, "pkgname", json_string(packageName));
        json_object_set_new(root, "summary", json_string(formula->summary));
        json_object_set_new(root, "version", json_string(formula->version));
        json_object_set_new(root, "license", json_string(formula->license));
        json_object_set_new(root, "web-url", json_string(formula->web_url));
        json_object_set_new(root, "bin-url", json_string(formula->bin_url));
        json_object_set_new(root, "bin-sha", json_string(formula->bin_sha));
        json_object_set_new(root, "dep-pkg", json_string(formula->dep_pkg));
        json_object_set_new(root, "install", json_string(formula->install));

        char * jsonStr = json_dumps(root, 0);

        if (jsonStr == NULL) {
            resultCode = UPPM_ERROR;
        } else {
            printf("%s\n", jsonStr);
            free(jsonStr);
        }

        json_decref(root);

        uppm_formula_free(formula);
    } else if (strcmp(key, "--shell") == 0) {
        UPPMFormula * formula = NULL;

        resultCode = uppm_formula_parse(packageName, &formula);

        if (resultCode != UPPM_OK) {
            return resultCode;
        }

        printf("UPPM_PKG_PKGNAME='%s'\n", packageName);
        printf("UPPM_PKG_SUMMARY='%s'\n", formula->summary);
        printf("UPPM_PKG_WEBPAGE='%s'\n", formula->web_url);
        printf("UPPM_PKG_VERSION='%s'\n", formula->version);
        printf("UPPM_PKG_BIN_URL='%s'\n", formula->bin_url);
        printf("UPPM_PKG_BIN_SHA='%s'\n", formula->bin_sha);

        if ((formula->install != NULL) && (strcmp(formula->install, "") != 0)) {
            printf("UPPM_PKG_INSTALL=\"%s\"\n", formula->install);
        }

        uppm_formula_free(formula);
    } else if (strcmp(key, "formula") == 0) {
        char * formulaFilePath = NULL;

        resultCode = uppm_formula_path(packageName, &formulaFilePath);

        if (resultCode != UPPM_OK) {
            return resultCode;
        }

        FILE * file = fopen(formulaFilePath, "r");

        if (file == NULL) {
            perror(formulaFilePath);
            free(formulaFilePath);
            return UPPM_ERROR;
        }

        printf("formula: %s\n", formulaFilePath);

        free(formulaFilePath);
        formulaFilePath = NULL;

        char buff[1024];
        int  size = 0;
        while((size = fread(buff, 1, 1024, file)) != 0) {
            fwrite(buff, 1, size, stdout);
        }

        fclose(file);
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
    } else if (strcmp(key, "web_url") == 0) {
        UPPMFormula * formula = NULL;

        resultCode = uppm_formula_parse(packageName, &formula);

        if (resultCode != UPPM_OK) {
            return resultCode;
        }

        if (formula->web_url != NULL) {
            printf("%s\n", formula->web_url);
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
    } else if (strcmp(key, "license") == 0) {
        UPPMFormula * formula = NULL;

        resultCode = uppm_formula_parse(packageName, &formula);

        if (resultCode != UPPM_OK) {
            return resultCode;
        }

        if (formula->license != NULL) {
            printf("%s\n", formula->license);
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

        size_t  receiptFilePathLength = installedDirLength + 20;
        char    receiptFilePath[receiptFilePathLength];
        memset (receiptFilePath, 0, receiptFilePathLength);
        sprintf(receiptFilePath, "%s/.uppm/receipt.yml", installedDir);

        if (exists_and_is_a_regular_file(receiptFilePath)) {
            printf("%s\n", installedDir);
        } else {
            return UPPM_PACKAGE_IS_NOT_INSTALLED;
        }
    } else if (strcmp(key, "installed-files") == 0) {
        char * userHomeDir = getenv("HOME");

        if (userHomeDir == NULL || strcmp(userHomeDir, "") == 0) {
            return UPPM_ENV_HOME_NOT_SET;
        }

        size_t userHomeDirLength = strlen(userHomeDir);

        size_t  installedDirLength = userHomeDirLength + strlen(packageName) + 20;
        char    installedDir[installedDirLength];
        memset (installedDir, 0, installedDirLength);
        sprintf(installedDir, "%s/.uppm/installed/%s", userHomeDir, packageName);

        size_t  receiptFilePathLength = installedDirLength + 20;
        char    receiptFilePath[receiptFilePathLength];
        memset (receiptFilePath, 0, receiptFilePathLength);
        sprintf(receiptFilePath, "%s/.uppm/receipt.yml", installedDir);

        if (!exists_and_is_a_regular_file(receiptFilePath)) {
            return UPPM_PACKAGE_IS_NOT_INSTALLED;
        }

        size_t  installedManifestFilePathLength = installedDirLength + 20;
        char    installedManifestFilePath[installedManifestFilePathLength];
        memset (installedManifestFilePath, 0, installedManifestFilePathLength);
        sprintf(installedManifestFilePath, "%s/.uppm/manifest.txt", installedDir);

        FILE * installedManifestFile = fopen(installedManifestFilePath, "r");

        if (installedManifestFile == NULL) {
            perror(installedManifestFilePath);
            return UPPM_ERROR;
        }

        char buff[1024];
        int  size = 0;
        while((size = fread(buff, 1, 1024, installedManifestFile)) != 0) {
            fwrite(buff, 1, size, stdout);
        }

        fclose(installedManifestFile);
    } else if (strcmp(key, "installed-receipt-path") == 0) {
        char * userHomeDir = getenv("HOME");

        if (userHomeDir == NULL || strcmp(userHomeDir, "") == 0) {
            return UPPM_ENV_HOME_NOT_SET;
        }

        size_t userHomeDirLength = strlen(userHomeDir);

        size_t  installedDirLength = userHomeDirLength + strlen(packageName) + 20;
        char    installedDir[installedDirLength];
        memset (installedDir, 0, installedDirLength);
        sprintf(installedDir, "%s/.uppm/installed/%s", userHomeDir, packageName);

        size_t  receiptFilePathLength = installedDirLength + 20;
        char    receiptFilePath[receiptFilePathLength];
        memset (receiptFilePath, 0, receiptFilePathLength);
        sprintf(receiptFilePath, "%s/.uppm/receipt.yml", installedDir);

        if (exists_and_is_a_regular_file(receiptFilePath)) {
            printf("%s\n", receiptFilePath);
        } else {
            return UPPM_PACKAGE_IS_NOT_INSTALLED;
        }
    } else if (strcmp(key, "installed-receipt-yaml") == 0) {
        char * userHomeDir = getenv("HOME");

        if (userHomeDir == NULL || strcmp(userHomeDir, "") == 0) {
            return UPPM_ENV_HOME_NOT_SET;
        }

        size_t userHomeDirLength = strlen(userHomeDir);

        size_t  installedDirLength = userHomeDirLength + strlen(packageName) + 20;
        char    installedDir[installedDirLength];
        memset (installedDir, 0, installedDirLength);
        sprintf(installedDir, "%s/.uppm/installed/%s", userHomeDir, packageName);

        size_t  receiptFilePathLength = installedDirLength + 20;
        char    receiptFilePath[receiptFilePathLength];
        memset (receiptFilePath, 0, receiptFilePathLength);
        sprintf(receiptFilePath, "%s/.uppm/receipt.yml", installedDir);

        if (!exists_and_is_a_regular_file(receiptFilePath)) {
            return UPPM_PACKAGE_IS_NOT_INSTALLED;
        }

        FILE * receiptFile = fopen(receiptFilePath, "r");

        if (receiptFile == NULL) {
            perror(receiptFilePath);
            return UPPM_ERROR;
        }

        char buff[1024];
        int  size = 0;
        while((size = fread(buff, 1, 1024, receiptFile)) != 0) {
            fwrite(buff, 1, size, stdout);
        }

        fclose(receiptFile);
    } else if (strcmp(key, "installed-receipt-json") == 0) {
        UPPMReceipt * receipt = NULL;

        int resultCode = uppm_receipt_parse(packageName, &receipt);

        if (resultCode != UPPM_OK) {
            return resultCode;
        }

        json_t * root = json_object();

        json_object_set_new(root, "pkgname", json_string(packageName));
        json_object_set_new(root, "summary", json_string(receipt->summary));
        json_object_set_new(root, "version", json_string(receipt->version));
        json_object_set_new(root, "license", json_string(receipt->license));
        json_object_set_new(root, "web-url", json_string(receipt->web_url));
        json_object_set_new(root, "bin-url", json_string(receipt->bin_url));
        json_object_set_new(root, "bin-sha", json_string(receipt->bin_sha));
        json_object_set_new(root, "dep-pkg", json_string(receipt->dep_pkg));
        json_object_set_new(root, "install", json_string(receipt->install));
        json_object_set_new(root, "signature", json_string(receipt->signature));
        json_object_set_new(root, "timestamp", json_string(receipt->timestamp));

        char * jsonStr = json_dumps(root, 0);

        if (jsonStr == NULL) {
            resultCode = UPPM_ERROR;
        } else {
            printf("%s\n", jsonStr);
            free(jsonStr);
        }

        json_decref(root);

        uppm_receipt_free(receipt);
    } else if (strcmp(key, "installed-version") == 0) {
        UPPMReceipt * receipt = NULL;

        int resultCode = uppm_receipt_parse(packageName, &receipt);

        if (resultCode != UPPM_OK) {
            return resultCode;
        }

        printf("%s\n", receipt->version);

        uppm_receipt_free(receipt);
    } else if (strcmp(key, "installed-timestamp-unix") == 0) {
        UPPMReceipt * receipt = NULL;

        int resultCode = uppm_receipt_parse(packageName, &receipt);

        if (resultCode != UPPM_OK) {
            return resultCode;
        }

        printf("%s\n", receipt->timestamp);

        uppm_receipt_free(receipt);
    } else if (strcmp(key, "installed-timestamp-rfc-3339") == 0) {
        UPPMReceipt * receipt = NULL;

        int resultCode = uppm_receipt_parse(packageName, &receipt);

        if (resultCode != UPPM_OK) {
            return resultCode;
        }

        time_t tt = (time_t)atol(receipt->timestamp);
        struct tm *tms = localtime(&tt);

        char buff[26] = {0};
        strftime(buff, 26, "%Y-%m-%d %H:%M:%S%z", tms);

        buff[24] = buff[23];
        buff[23] = buff[22];
        buff[22] = ':';

        printf("%s\n", buff);

        uppm_receipt_free(receipt);
    } else if (strcmp(key, "installed-timestamp-iso-8601") == 0) {
        UPPMReceipt * receipt = NULL;

        int resultCode = uppm_receipt_parse(packageName, &receipt);

        if (resultCode != UPPM_OK) {
            return resultCode;
        }

        time_t tt = (time_t)atol(receipt->timestamp);
        struct tm *tms = localtime(&tt);

        char buff[26] = {0};
        strftime(buff, 26, "%Y-%m-%dT%H:%M:%S%z", tms);

        buff[24] = buff[23];
        buff[23] = buff[22];
        buff[22] = ':';

        printf("%s\n", buff);

        uppm_receipt_free(receipt);
    } else {
        return UPPM_ARG_IS_UNKNOWN;
    }

    return resultCode;
}
