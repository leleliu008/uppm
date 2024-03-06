#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <sys/stat.h>

#include <jansson.h>

#include "core/log.h"

#include "uppm.h"

int uppm_installed_info(const char * packageName, const char * key) {
    int ret = uppm_check_if_the_given_argument_matches_package_name_pattern(packageName);

    if (ret != UPPM_OK) {
        return ret;
    }

    if ((key == NULL) || (key[0] == '\0') || (strcmp(key, "--yaml") == 0)) {
        char   uppmHomeDIR[PATH_MAX];
        size_t uppmHomeDIRLength;

        int ret = uppm_home_dir(uppmHomeDIR, PATH_MAX, &uppmHomeDIRLength);

        if (ret != UPPM_OK) {
            return ret;
        }

        struct stat st;

        size_t installedDIRLength = uppmHomeDIRLength + strlen(packageName) + 12U;
        char   installedDIR[installedDIRLength];

        ret = snprintf(installedDIR, installedDIRLength, "%s/installed/%s", uppmHomeDIR, packageName);

        if (ret < 0) {
            perror(NULL);
            return UPPM_ERROR;
        }

        if (stat(installedDIR, &st) == 0) {
            if (!S_ISDIR(st.st_mode)) {
                return UPPM_ERROR_PACKAGE_IS_BROKEN;
            }
        } else {
            return UPPM_ERROR_PACKAGE_NOT_INSTALLED;
        }

        size_t receiptFilePathLength = installedDIRLength + 20U;
        char   receiptFilePath[receiptFilePathLength];

        ret = snprintf(receiptFilePath, receiptFilePathLength, "%s/.uppm/receipt.yml", installedDIR);

        if (ret < 0) {
            perror(NULL);
            return UPPM_ERROR;
        }

        if (stat(receiptFilePath, &st) != 0 || (!S_ISREG(st.st_mode))) {
            return UPPM_ERROR_PACKAGE_IS_BROKEN;
        }

        int receiptFD = open(receiptFilePath, O_RDONLY);

        if (receiptFD == -1) {
            perror(receiptFilePath);
            return UPPM_ERROR;
        }

        char buf[1024];

        for (;;) {
            ssize_t readSize = read(receiptFD, buf, 1024);

            if (readSize == -1) {
                perror(receiptFilePath);
                close(receiptFD);
                return UPPM_ERROR;
            }

            if (readSize == 0) {
                close(receiptFD);
                return UPPM_OK;
            }

            ssize_t writeSize = write(STDOUT_FILENO, buf, readSize);

            if (writeSize == -1) {
                perror(NULL);
                close(receiptFD);
                return UPPM_ERROR;
            }

            if (writeSize != readSize) {
                perror(NULL);
                close(receiptFD);
                return UPPM_ERROR;
            }
        }
    } else if (strcmp(key, "--prefix") == 0) {
        char   uppmHomeDIR[PATH_MAX];
        size_t uppmHomeDIRLength;

        int ret = uppm_home_dir(uppmHomeDIR, PATH_MAX, &uppmHomeDIRLength);

        if (ret != UPPM_OK) {
            return ret;
        }

        struct stat st;

        size_t installedDIRLength = uppmHomeDIRLength + strlen(packageName) + 12U;
        char   installedDIR[installedDIRLength];

        ret = snprintf(installedDIR, installedDIRLength, "%s/installed/%s", uppmHomeDIR, packageName);

        if (ret < 0) {
            perror(NULL);
            return UPPM_ERROR;
        }

        if (stat(installedDIR, &st) == 0) {
            if (!S_ISDIR(st.st_mode)) {
                return UPPM_ERROR_PACKAGE_IS_BROKEN;
            }
        } else {
            return UPPM_ERROR_PACKAGE_NOT_INSTALLED;
        }

        size_t receiptFilePathLength = installedDIRLength + 20U;
        char   receiptFilePath[receiptFilePathLength];

        ret = snprintf(receiptFilePath, receiptFilePathLength, "%s/.uppm/receipt.yml", installedDIR);

        if (ret < 0) {
            perror(NULL);
            return UPPM_ERROR;
        }

        if (stat(receiptFilePath, &st) == 0) {
            if (S_ISREG(st.st_mode)) {
                printf("%s\n", installedDIR);
            } else {
                return UPPM_ERROR_PACKAGE_IS_BROKEN;
            }
        } else {
            return UPPM_ERROR_PACKAGE_IS_BROKEN;
        }
    } else if (strcmp(key, "--files") == 0) {
        char   uppmHomeDIR[PATH_MAX];
        size_t uppmHomeDIRLength;

        int ret = uppm_home_dir(uppmHomeDIR, PATH_MAX, &uppmHomeDIRLength);

        if (ret != UPPM_OK) {
            return ret;
        }

        size_t installedDIRLength = uppmHomeDIRLength + strlen(packageName) + 12U;
        char   installedDIR[installedDIRLength];

        ret = snprintf(installedDIR, installedDIRLength, "%s/installed/%s", uppmHomeDIR, packageName);

        if (ret < 0) {
            perror(NULL);
            return UPPM_ERROR;
        }

        struct stat st;

        if (stat(installedDIR, &st) == 0) {
            if (!S_ISDIR(st.st_mode)) {
                return UPPM_ERROR_PACKAGE_IS_BROKEN;
            }
        } else {
            return UPPM_ERROR_PACKAGE_NOT_INSTALLED;
        }

        size_t receiptFilePathLength = installedDIRLength + 20U;
        char   receiptFilePath[receiptFilePathLength];

        ret = snprintf(receiptFilePath, receiptFilePathLength, "%s/.uppm/receipt.yml", installedDIR);

        if (ret < 0) {
            perror(NULL);
            return UPPM_ERROR;
        }

        if (stat(receiptFilePath, &st) != 0 || (!S_ISREG(st.st_mode))) {
            return UPPM_ERROR_PACKAGE_IS_BROKEN;
        }

        size_t manifestFilePathLength = installedDIRLength + 20U;
        char   manifestFilePath[manifestFilePathLength];

        ret = snprintf(manifestFilePath, manifestFilePathLength, "%s/.uppm/manifest.txt", installedDIR);

        if (ret < 0) {
            perror(NULL);
            return UPPM_ERROR;
        }

        if (stat(manifestFilePath, &st) != 0 || (!S_ISREG(st.st_mode))) {
            return UPPM_ERROR_PACKAGE_IS_BROKEN;
        }

        int manifestFD = open(manifestFilePath, O_RDONLY);

        if (manifestFD == -1) {
            perror(manifestFilePath);
            return UPPM_ERROR;
        }

        char buf[1024];

        for (;;) {
            ssize_t readSize = read(manifestFD, buf, 1024);

            if (readSize == -1) {
                perror(manifestFilePath);
                close(manifestFD);
                return UPPM_ERROR;
            }

            if (readSize == 0) {
                close(manifestFD);
                break;
            }

            ssize_t writeSize = write(STDOUT_FILENO, buf, readSize);

            if (writeSize == -1) {
                perror(manifestFilePath);
                close(manifestFD);
                return UPPM_ERROR;
            }

            if (writeSize != readSize) {
                fprintf(stderr, "not fully written to stdout.\n");
                close(manifestFD);
                return UPPM_ERROR;
            }
        }
    } else if (strcmp(key, "--json") == 0) {
        UPPMReceipt * receipt = NULL;

        ret = uppm_receipt_parse(packageName, &receipt);

        if (ret != UPPM_OK) {
            return ret;
        }

        json_t * root = json_object();

        json_object_set_new(root, "pkgname", json_string(packageName));
        json_object_set_new(root, "summary", json_string(receipt->summary));
        json_object_set_new(root, "version", json_string(receipt->version));
        json_object_set_new(root, "license", json_string(receipt->license));
        json_object_set_new(root, "webpage", json_string(receipt->webpage));
        json_object_set_new(root, "bin-url", json_string(receipt->bin_url));
        json_object_set_new(root, "bin-sha", json_string(receipt->bin_sha));
        json_object_set_new(root, "dep-pkg", json_string(receipt->dep_pkg));
        json_object_set_new(root, "install", json_string(receipt->install));

        char * jsonStr = json_dumps(root, 0);

        if (jsonStr == NULL) {
            ret = UPPM_ERROR;
        } else {
            printf("%s\n", jsonStr);
            free(jsonStr);
        }

        json_decref(root);

        uppm_receipt_free(receipt);
    } else if (strcmp(key, "--shell") == 0) {
        UPPMReceipt * receipt = NULL;

        ret = uppm_receipt_parse(packageName, &receipt);

        if (ret != UPPM_OK) {
            return ret;
        }

        printf("UPPM_PKG_PKGNAME='%s'\n", packageName);
        printf("UPPM_PKG_SUMMARY='%s'\n", receipt->summary);
        printf("UPPM_PKG_VERSION='%s'\n", receipt->version);
        printf("UPPM_PKG_WEB_URL='%s'\n", receipt->webpage);
        printf("UPPM_PKG_BIN_URL='%s'\n", receipt->bin_url);
        printf("UPPM_PKG_BIN_SHA='%s'\n", receipt->bin_sha);
        printf("UPPM_PKG_INSTALL=\"%s\"\n", receipt->install == NULL ? "" : receipt->install);

        uppm_receipt_free(receipt);
    } else if (strcmp(key, "summary") == 0) {
        UPPMReceipt * receipt = NULL;

        ret = uppm_receipt_parse(packageName, &receipt);

        if (ret != UPPM_OK) {
            return ret;
        }

        if (receipt->summary != NULL) {
            printf("%s\n", receipt->summary);
        }

        uppm_receipt_free(receipt);
    } else if (strcmp(key, "webpage") == 0) {
        UPPMReceipt * receipt = NULL;

        ret = uppm_receipt_parse(packageName, &receipt);

        if (ret != UPPM_OK) {
            return ret;
        }

        if (receipt->webpage != NULL) {
            printf("%s\n", receipt->webpage);
        }

        uppm_receipt_free(receipt);
    } else if (strcmp(key, "version") == 0) {
        UPPMReceipt * receipt = NULL;

        ret = uppm_receipt_parse(packageName, &receipt);

        if (ret != UPPM_OK) {
            return ret;
        }

        if (receipt->version != NULL) {
            printf("%s\n", receipt->version);
        }

        uppm_receipt_free(receipt);
    } else if (strcmp(key, "license") == 0) {
        UPPMReceipt * receipt = NULL;

        ret = uppm_receipt_parse(packageName, &receipt);

        if (ret != UPPM_OK) {
            return ret;
        }

        if (receipt->license != NULL) {
            printf("%s\n", receipt->license);
        }

        uppm_receipt_free(receipt);
    } else if (strcmp(key, "bin-url") == 0) {
        UPPMReceipt * receipt = NULL;

        ret = uppm_receipt_parse(packageName, &receipt);

        if (ret != UPPM_OK) {
            return ret;
        }

        if (receipt->bin_url != NULL) {
            printf("%s\n", receipt->bin_url);
        }

        uppm_receipt_free(receipt);
    } else if (strcmp(key, "bin-sha") == 0) {
        UPPMReceipt * receipt = NULL;

        ret = uppm_receipt_parse(packageName, &receipt);

        if (ret != UPPM_OK) {
            return ret;
        }

        if (receipt->bin_sha != NULL) {
            printf("%s\n", receipt->bin_sha);
        }

        uppm_receipt_free(receipt);
    } else if (strcmp(key, "dep-pkg") == 0) {
        UPPMReceipt * receipt = NULL;

        ret = uppm_receipt_parse(packageName, &receipt);

        if (ret != UPPM_OK) {
            return ret;
        }

        if (receipt->dep_pkg != NULL) {
            printf("%s\n", receipt->dep_pkg);
        }

        uppm_receipt_free(receipt);
    } else if (strcmp(key, "install") == 0) {
        UPPMReceipt * receipt = NULL;

        ret = uppm_receipt_parse(packageName, &receipt);

        if (ret != UPPM_OK) {
            return ret;
        }

        if (receipt->install != NULL) {
            printf("%s\n", receipt->install);
        }

        uppm_receipt_free(receipt);
    } else if (strcmp(key, "timestamp-unix") == 0) {
        UPPMReceipt * receipt = NULL;

        int ret = uppm_receipt_parse(packageName, &receipt);

        if (ret != UPPM_OK) {
            return ret;
        }

        printf("%s\n", receipt->timestamp);

        uppm_receipt_free(receipt);
    } else if (strcmp(key, "timestamp-rfc-3339") == 0) {
        UPPMReceipt * receipt = NULL;

        int ret = uppm_receipt_parse(packageName, &receipt);

        if (ret != UPPM_OK) {
            return ret;
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
    } else if (strcmp(key, "timestamp-iso-8601") == 0) {
        UPPMReceipt * receipt = NULL;

        int ret = uppm_receipt_parse(packageName, &receipt);

        if (ret != UPPM_OK) {
            return ret;
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
        return UPPM_ERROR_ARG_IS_UNKNOWN;
    }

    return ret;
}
