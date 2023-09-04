#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "core/rm-r.h"

#include "uppm.h"

int uppm_uninstall(const char * packageName, bool verbose) {
    int ret = uppm_check_if_the_given_argument_matches_package_name_pattern(packageName);

    if (ret != UPPM_OK) {
        return ret;
    }

    char   uppmHomeDIR[256] = {0};
    size_t uppmHomeDIRLength;

    ret = uppm_home_dir(uppmHomeDIR, 255, &uppmHomeDIRLength);

    if (ret != UPPM_OK) {
        return ret;
    }

    size_t   packageInstalledDIRLength = uppmHomeDIRLength + strlen(packageName) + 12U;
    char     packageInstalledDIR[packageInstalledDIRLength];
    snprintf(packageInstalledDIR, packageInstalledDIRLength, "%s/installed/%s", uppmHomeDIR, packageName);

    struct stat st;

    if (stat(packageInstalledDIR, &st) != 0) {
        return UPPM_ERROR_PACKAGE_NOT_INSTALLED;
    }

    size_t   receiptFilePathLength = packageInstalledDIRLength + 20U;
    char     receiptFilePath[receiptFilePathLength];
    snprintf(receiptFilePath, receiptFilePathLength, "%s/.uppm/receipt.yml", packageInstalledDIR);

    if (stat(receiptFilePath, &st) == 0 && S_ISREG(st.st_mode)) {
        if (rm_r(packageInstalledDIR, verbose) == 0) {
            return UPPM_OK;
        } else {
            perror(packageInstalledDIR);
            return UPPM_ERROR;
        }
    } else {
        // package is broken. is not installed completely?
        return UPPM_ERROR_PACKAGE_NOT_INSTALLED;
    }
}
