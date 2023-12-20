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

static int package_name_callback(const char * packageName, size_t i, const void * key) {
    if (i != 0) {
        printf("\n");
    }

    return uppm_info(packageName, (char*)key);
}

int uppm_info(const char * packageName, const char * key) {
    if (packageName == NULL) {
        return UPPM_ERROR_ARG_IS_NULL;
    }

    if (packageName[0] == '\0') {
        return UPPM_ERROR_ARG_IS_EMPTY;
    }

    if (strcmp(packageName, "@all") == 0) {
        return uppm_list_the_available_packages(package_name_callback, key);
    }

    int ret = uppm_check_if_the_given_argument_matches_package_name_pattern(packageName);

    if (ret != UPPM_OK) {
        return ret;
    }

    if ((key == NULL) || (key[0] == '\0') || (strcmp(key, "--yaml") == 0)) {
        char * formulaFilePath = NULL;

        ret = uppm_formula_locate(packageName, &formulaFilePath);

        if (ret != UPPM_OK) {
            return ret;
        }

        int formulaFD = open(formulaFilePath, O_RDONLY);

        if (formulaFD == -1) {
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

        char buf[1024];

        for (;;) {
            ssize_t readSize = read(formulaFD, buf, 1024);

            if (readSize == -1) {
                perror(formulaFilePath);
                close(formulaFD);
                free(formulaFilePath);
                return UPPM_ERROR;
            }

            if (readSize == 0) {
                close(formulaFD);
                break;
            }

            ssize_t writeSize = write(STDOUT_FILENO, buf, readSize);

            if (writeSize == -1) {
                perror(NULL);
                close(formulaFD);
                free(formulaFilePath);
                return UPPM_ERROR;
            }

            if (writeSize != readSize) {
                fprintf(stderr, "not fully written to stdout.\n");
                close(formulaFD);
                free(formulaFilePath);
                return UPPM_ERROR;
            }
        }

        printf("formula: %s\n", formulaFilePath);

        free(formulaFilePath);
    } else if (strcmp(key, "--json") == 0) {
        UPPMFormula * formula = NULL;

        ret = uppm_formula_lookup(packageName, &formula);

        if (ret != UPPM_OK) {
            return ret;
        }

        json_t * root = json_object();

        json_object_set_new(root, "pkgname", json_string(packageName));
        json_object_set_new(root, "summary", json_string(formula->summary));
        json_object_set_new(root, "version", json_string(formula->version));
        json_object_set_new(root, "license", json_string(formula->license));
        json_object_set_new(root, "webpage", json_string(formula->webpage));
        json_object_set_new(root, "bin-url", json_string(formula->bin_url));
        json_object_set_new(root, "bin-sha", json_string(formula->bin_sha));
        json_object_set_new(root, "dep-pkg", json_string(formula->dep_pkg));
        json_object_set_new(root, "install", json_string(formula->install));

        char * jsonStr = json_dumps(root, 0);

        if (jsonStr == NULL) {
            ret = UPPM_ERROR;
        } else {
            printf("%s\n", jsonStr);
            free(jsonStr);
        }

        json_decref(root);

        uppm_formula_free(formula);
    } else if (strcmp(key, "--shell") == 0) {
        UPPMFormula * formula = NULL;

        ret = uppm_formula_lookup(packageName, &formula);

        if (ret != UPPM_OK) {
            return ret;
        }

        printf("UPPM_PKG_PKGNAME='%s'\n", packageName);
        printf("UPPM_PKG_SUMMARY='%s'\n", formula->summary);
        printf("UPPM_PKG_VERSION='%s'\n", formula->version);
        printf("UPPM_PKG_WEB_URL='%s'\n", formula->webpage);
        printf("UPPM_PKG_BIN_URL='%s'\n", formula->bin_url);
        printf("UPPM_PKG_BIN_SHA='%s'\n", formula->bin_sha);
        printf("UPPM_PKG_INSTALL=\"%s\"\n", formula->install == NULL ? "" : formula->install);

        uppm_formula_free(formula);
    } else if (strcmp(key, "formula") == 0) {
        char * formulaFilePath = NULL;

        ret = uppm_formula_locate(packageName, &formulaFilePath);

        if (ret == UPPM_OK) {
            printf("%s\n", formulaFilePath);
            free(formulaFilePath);
        }

        return ret;
    } else if (strcmp(key, "summary") == 0) {
        UPPMFormula * formula = NULL;

        ret = uppm_formula_lookup(packageName, &formula);

        if (ret != UPPM_OK) {
            return ret;
        }

        if (formula->summary != NULL) {
            printf("%s\n", formula->summary);
        }

        uppm_formula_free(formula);
    } else if (strcmp(key, "webpage") == 0) {
        UPPMFormula * formula = NULL;

        ret = uppm_formula_lookup(packageName, &formula);

        if (ret != UPPM_OK) {
            return ret;
        }

        if (formula->webpage != NULL) {
            printf("%s\n", formula->webpage);
        }

        uppm_formula_free(formula);
    } else if (strcmp(key, "version") == 0) {
        UPPMFormula * formula = NULL;

        ret = uppm_formula_lookup(packageName, &formula);

        if (ret != UPPM_OK) {
            return ret;
        }

        if (formula->version != NULL) {
            printf("%s\n", formula->version);
        }

        uppm_formula_free(formula);
    } else if (strcmp(key, "license") == 0) {
        UPPMFormula * formula = NULL;

        ret = uppm_formula_lookup(packageName, &formula);

        if (ret != UPPM_OK) {
            return ret;
        }

        if (formula->license != NULL) {
            printf("%s\n", formula->license);
        }

        uppm_formula_free(formula);
    } else if (strcmp(key, "bin-url") == 0) {
        UPPMFormula * formula = NULL;

        ret = uppm_formula_lookup(packageName, &formula);

        if (ret != UPPM_OK) {
            return ret;
        }

        if (formula->bin_url != NULL) {
            printf("%s\n", formula->bin_url);
        }

        uppm_formula_free(formula);
    } else if (strcmp(key, "bin-sha") == 0) {
        UPPMFormula * formula = NULL;

        ret = uppm_formula_lookup(packageName, &formula);

        if (ret != UPPM_OK) {
            return ret;
        }

        if (formula->bin_sha != NULL) {
            printf("%s\n", formula->bin_sha);
        }

        uppm_formula_free(formula);
    } else if (strcmp(key, "bin-ft") == 0) {
        UPPMFormula * formula = NULL;

        ret = uppm_formula_lookup(packageName, &formula);

        if (ret != UPPM_OK) {
            return ret;
        }

        char binFileNameExtension[21] = {0};

        ret = uppm_examine_file_extension_from_url(binFileNameExtension, 20, formula->bin_url);

        if (ret != UPPM_OK) {
            uppm_formula_free(formula);
            return ret;
        }

        printf("%s\n", binFileNameExtension);

        uppm_formula_free(formula);
    } else if (strcmp(key, "bin-fp") == 0) {
        char   uppmHomeDIR[PATH_MAX];
        size_t uppmHomeDIRLength;

        int ret = uppm_home_dir(uppmHomeDIR, PATH_MAX, &uppmHomeDIRLength);

        if (ret != UPPM_OK) {
            return ret;
        }

        UPPMFormula * formula = NULL;

        ret = uppm_formula_lookup(packageName, &formula);

        if (ret != UPPM_OK) {
            return ret;
        }

        char binFileNameExtension[21] = {0};

        ret = uppm_examine_file_extension_from_url(binFileNameExtension, 20, formula->bin_url);

        if (ret != UPPM_OK) {
            uppm_formula_free(formula);
            return ret;
        }

        printf("%s/downloads/%s%s\n", uppmHomeDIR, formula->bin_sha, binFileNameExtension);

        uppm_formula_free(formula);
    } else if (strcmp(key, "dep-pkg") == 0) {
        UPPMFormula * formula = NULL;

        ret = uppm_formula_lookup(packageName, &formula);

        if (ret != UPPM_OK) {
            return ret;
        }

        if (formula->dep_pkg != NULL) {
            printf("%s\n", formula->dep_pkg);
        }

        uppm_formula_free(formula);
    } else if (strcmp(key, "install") == 0) {
        UPPMFormula * formula = NULL;

        ret = uppm_formula_lookup(packageName, &formula);

        if (ret != UPPM_OK) {
            return ret;
        }

        if (formula->install != NULL) {
            printf("%s\n", formula->install);
        }

        uppm_formula_free(formula);
    } else if (strcmp(key, "installed-dir") == 0) {
        char   uppmHomeDIR[PATH_MAX];
        size_t uppmHomeDIRLength;

        int ret = uppm_home_dir(uppmHomeDIR, PATH_MAX, &uppmHomeDIRLength);

        if (ret != UPPM_OK) {
            return ret;
        }

        struct stat st;

        size_t   installedDIRLength = uppmHomeDIRLength + strlen(packageName) + 12U;
        char     installedDIR[installedDIRLength];
        snprintf(installedDIR, installedDIRLength, "%s/installed/%s", uppmHomeDIR, packageName);

        if (stat(installedDIR, &st) == 0) {
            if (!S_ISDIR(st.st_mode)) {
                return UPPM_ERROR_PACKAGE_IS_BROKEN;
            }
        } else {
            return UPPM_ERROR_PACKAGE_NOT_INSTALLED;
        }

        size_t receiptFilePathLength = installedDIRLength + 20U;
        char   receiptFilePath[receiptFilePathLength];
        snprintf(receiptFilePath, receiptFilePathLength, "%s/.uppm/receipt.yml", installedDIR);

        if (stat(receiptFilePath, &st) == 0) {
            if (S_ISREG(st.st_mode)) {
                printf("%s\n", installedDIR);
            } else {
                return UPPM_ERROR_PACKAGE_IS_BROKEN;
            }
        } else {
            return UPPM_ERROR_PACKAGE_IS_BROKEN;
        }
    } else if (strcmp(key, "installed-files") == 0) {
        char   uppmHomeDIR[PATH_MAX];
        size_t uppmHomeDIRLength;

        int ret = uppm_home_dir(uppmHomeDIR, PATH_MAX, &uppmHomeDIRLength);

        if (ret != UPPM_OK) {
            return ret;
        }

        struct stat st;

        size_t   installedDIRLength = uppmHomeDIRLength + strlen(packageName) + 12U;
        char     installedDIR[installedDIRLength];
        snprintf(installedDIR, installedDIRLength, "%s/installed/%s", uppmHomeDIR, packageName);

        if (stat(installedDIR, &st) == 0) {
            if (!S_ISDIR(st.st_mode)) {
                return UPPM_ERROR_PACKAGE_IS_BROKEN;
            }
        } else {
            return UPPM_ERROR_PACKAGE_NOT_INSTALLED;
        }

        size_t   receiptFilePathLength = installedDIRLength + 20U;
        char     receiptFilePath[receiptFilePathLength];
        snprintf(receiptFilePath, receiptFilePathLength, "%s/.uppm/receipt.yml", installedDIR);

        if (stat(receiptFilePath, &st) != 0 || (!S_ISREG(st.st_mode))) {
            return UPPM_ERROR_PACKAGE_IS_BROKEN;
        }

        size_t   manifestFilePathLength = installedDIRLength + 20U;
        char     manifestFilePath[manifestFilePathLength];
        snprintf(manifestFilePath, manifestFilePathLength, "%s/.uppm/manifest.txt", installedDIR);

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
    } else if (strcmp(key, "installed-receipt-path") == 0) {
        char   uppmHomeDIR[PATH_MAX];
        size_t uppmHomeDIRLength;

        int ret = uppm_home_dir(uppmHomeDIR, PATH_MAX, &uppmHomeDIRLength);

        if (ret != UPPM_OK) {
            return ret;
        }

        struct stat st;

        size_t   installedDIRLength = uppmHomeDIRLength + strlen(packageName) + 12U;
        char     installedDIR[installedDIRLength];
        snprintf(installedDIR, installedDIRLength, "%s/installed/%s", uppmHomeDIR, packageName);

        if (stat(installedDIR, &st) == 0) {
            if (!S_ISDIR(st.st_mode)) {
                return UPPM_ERROR_PACKAGE_IS_BROKEN;
            }
        } else {
            return UPPM_ERROR_PACKAGE_NOT_INSTALLED;
        }

        size_t receiptFilePathLength = installedDIRLength + 20U;
        char   receiptFilePath[receiptFilePathLength];
        snprintf(receiptFilePath, receiptFilePathLength, "%s/.uppm/receipt.yml", installedDIR);

        if (stat(receiptFilePath, &st) == 0) {
            if (S_ISREG(st.st_mode)) {
                printf("%s\n", receiptFilePath);
            } else {
                return UPPM_ERROR_PACKAGE_IS_BROKEN;
            }
        } else {
            return UPPM_ERROR_PACKAGE_IS_BROKEN;
        }
    } else if (strcmp(key, "installed-receipt-yaml") == 0) {
        char   uppmHomeDIR[PATH_MAX];
        size_t uppmHomeDIRLength;

        int ret = uppm_home_dir(uppmHomeDIR, PATH_MAX, &uppmHomeDIRLength);

        if (ret != UPPM_OK) {
            return ret;
        }

        struct stat st;

        size_t   installedDIRLength = uppmHomeDIRLength + strlen(packageName) + 12U;
        char     installedDIR[installedDIRLength];
        snprintf(installedDIR, installedDIRLength, "%s/installed/%s", uppmHomeDIR, packageName);

        if (stat(installedDIR, &st) == 0) {
            if (!S_ISDIR(st.st_mode)) {
                return UPPM_ERROR_PACKAGE_IS_BROKEN;
            }
        } else {
            return UPPM_ERROR_PACKAGE_NOT_INSTALLED;
        }

        size_t   receiptFilePathLength = installedDIRLength + 20U;
        char     receiptFilePath[receiptFilePathLength];
        snprintf(receiptFilePath, receiptFilePathLength, "%s/.uppm/receipt.yml", installedDIR);

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
    } else if (strcmp(key, "installed-receipt-json") == 0) {
        UPPMReceipt * receipt = NULL;

        int ret = uppm_receipt_parse(packageName, &receipt);

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
        json_object_set_new(root, "signature", json_string(receipt->signature));
        json_object_set_new(root, "timestamp", json_string(receipt->timestamp));

        char * jsonStr = json_dumps(root, 0);

        if (jsonStr == NULL) {
            ret = UPPM_ERROR;
        } else {
            printf("%s\n", jsonStr);
            free(jsonStr);
        }

        json_decref(root);

        uppm_receipt_free(receipt);
    } else if (strcmp(key, "installed-version") == 0) {
        UPPMReceipt * receipt = NULL;

        int ret = uppm_receipt_parse(packageName, &receipt);

        if (ret != UPPM_OK) {
            return ret;
        }

        printf("%s\n", receipt->version);

        uppm_receipt_free(receipt);
    } else if (strcmp(key, "installed-timestamp-unix") == 0) {
        UPPMReceipt * receipt = NULL;

        int ret = uppm_receipt_parse(packageName, &receipt);

        if (ret != UPPM_OK) {
            return ret;
        }

        printf("%s\n", receipt->timestamp);

        uppm_receipt_free(receipt);
    } else if (strcmp(key, "installed-timestamp-rfc-3339") == 0) {
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
    } else if (strcmp(key, "installed-timestamp-iso-8601") == 0) {
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
