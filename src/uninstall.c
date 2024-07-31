#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <limits.h>
#include <sys/stat.h>

#include "uppm.h"

int uppm_uninstall(const char * packageName, const bool verbose) {
    int ret = uppm_check_if_the_given_argument_matches_package_name_pattern(packageName);

    if (ret != UPPM_OK) {
        return ret;
    }

    char   uppmHomeDIR[PATH_MAX];
    size_t uppmHomeDIRLength;

    ret = uppm_home_dir(uppmHomeDIR, PATH_MAX, &uppmHomeDIRLength);

    if (ret != UPPM_OK) {
        return ret;
    }

    size_t packageInstalledRootDIRLength = uppmHomeDIRLength + strlen(packageName) + 11U;
    char   packageInstalledRootDIR[packageInstalledRootDIRLength];

    ret = snprintf(packageInstalledRootDIR, packageInstalledRootDIRLength, "%s/installed", uppmHomeDIR);

    if (ret < 0) {
        perror(NULL);
        return UPPM_ERROR;
    }

    size_t packageInstalledLinkDIRLength = packageInstalledRootDIRLength + strlen(packageName) + 2U;
    char   packageInstalledLinkDIR[packageInstalledLinkDIRLength];

    ret = snprintf(packageInstalledLinkDIR, packageInstalledLinkDIRLength, "%s/%s", packageInstalledRootDIR, packageName);

    if (ret < 0) {
        perror(NULL);
        return UPPM_ERROR;
    }

    struct stat st;

    if (lstat(packageInstalledLinkDIR, &st) == 0) {
        if (S_ISLNK(st.st_mode)) {
            size_t receiptFilePathLength = packageInstalledLinkDIRLength + 20U;
            char   receiptFilePath[receiptFilePathLength];

            ret = snprintf(receiptFilePath, receiptFilePathLength, "%s/.uppm/receipt.yml", packageInstalledLinkDIR);

            if (ret < 0) {
                perror(NULL);
                return UPPM_ERROR;
            }

            if (lstat(receiptFilePath, &st) == 0 && S_ISREG(st.st_mode)) {
                char buf[256] = {0};

                ssize_t readSize = readlink(packageInstalledLinkDIR, buf, 255);

                if (readSize == -1) {
                    perror(packageInstalledLinkDIR);
                    return UPPM_ERROR;
                } else if (readSize != 64) {
                    // package is broken by other tools?
                    return UPPM_ERROR_PACKAGE_NOT_INSTALLED;
                }

                size_t packageInstalledRealDIRLength = packageInstalledRootDIRLength + 66U;
                char   packageInstalledRealDIR[packageInstalledRealDIRLength];

                ret = snprintf(packageInstalledRealDIR, packageInstalledRealDIRLength, "%s/%s", packageInstalledRootDIR, buf);

                if (ret < 0) {
                    perror(NULL);
                    return UPPM_ERROR;
                }

                if (lstat(packageInstalledRealDIR, &st) == 0) {
                    if (S_ISDIR(st.st_mode)) {
                        if (unlink(packageInstalledLinkDIR) == 0) {
                            if (verbose) {
                                printf("rm %s\n", packageInstalledLinkDIR);
                            }
                        } else {
                            perror(packageInstalledLinkDIR);
                            return UPPM_ERROR;
                        }

                        return uppm_rm_r(packageInstalledRealDIR, verbose);
                    } else {
                        // package is broken by other tools?
                        return UPPM_ERROR_PACKAGE_NOT_INSTALLED;
                    }
                } else {
                    // package is broken by other tools?
                    return UPPM_ERROR_PACKAGE_NOT_INSTALLED;
                }
            } else {
                // package is broken. is not installed completely?
                return UPPM_ERROR_PACKAGE_NOT_INSTALLED;
            }
        } else {
            return UPPM_ERROR_PACKAGE_NOT_INSTALLED;
        }
    } else {
        return UPPM_ERROR_PACKAGE_NOT_INSTALLED;
    }
}
