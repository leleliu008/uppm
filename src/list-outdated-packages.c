#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <fnmatch.h>

#include "core/log.h"
#include "core/fs.h"
#include "uppm.h"

int uppm_list_the_outdated__packages() {
    char * userHomeDir = getenv("HOME");

    if (userHomeDir == NULL || strcmp(userHomeDir, "") == 0) {
        return UPPM_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    size_t  installedDirLength = userHomeDirLength + 17; 
    char    installedDir[installedDirLength];
    memset (installedDir, 0, installedDirLength);
    sprintf(installedDir, "%s/.uppm/installed", userHomeDir);

    if (!exists_and_is_a_directory(installedDir)) {
        return UPPM_OK;
    }

    DIR           * dir;
    struct dirent * dir_entry;

    dir = opendir(installedDir);

    if (dir == NULL) {
        perror(installedDir);
        return UPPM_ERROR;
    }

    while ((dir_entry = readdir(dir))) {
        if ((strcmp(dir_entry->d_name, ".") == 0) || (strcmp(dir_entry->d_name, "..") == 0)) {
            continue;
        }

        size_t  receiptFilePathLength = installedDirLength + strlen(dir_entry->d_name) + 20;
        char    receiptFilePath[receiptFilePathLength];
        memset (receiptFilePath, 0, receiptFilePathLength);
        sprintf(receiptFilePath, "%s/%s/.uppm/receipt.yml", installedDir, dir_entry->d_name);

        if (exists_and_is_a_regular_file(receiptFilePath)) {
            //printf("%s\n", dir_entry->d_name);

            UPPMReceipt * receipt = NULL;

            int resultCode = uppm_receipt_parse(dir_entry->d_name, &receipt);

            if (resultCode != UPPM_OK) {
                closedir(dir);
                uppm_receipt_free(receipt);
                receipt = NULL;
                return resultCode;
            }

            UPPMFormula * formula = NULL;

            resultCode = uppm_formula_parse(dir_entry->d_name, &formula);

            if (resultCode == UPPM_OK) {
                if (strcmp(receipt->version, formula->version) != 0) {
                    printf("%s %s => %s\n", dir_entry->d_name, receipt->version, formula->version);
                }
            }

            uppm_formula_free(formula);
            uppm_receipt_free(receipt);

            formula = NULL;
            receipt = NULL;
        }
    }

    closedir(dir);

    return UPPM_OK;
}
