#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <yaml.h>
#include "uppm.h"
#include "core/fs.h"

typedef enum {
    INSTALLED_METADATA_KEY_CODE_unknown,
    INSTALLED_METADATA_KEY_CODE_summary,
    INSTALLED_METADATA_KEY_CODE_webpage,
    INSTALLED_METADATA_KEY_CODE_version,
    INSTALLED_METADATA_KEY_CODE_license,
    INSTALLED_METADATA_KEY_CODE_bin_url,
    INSTALLED_METADATA_KEY_CODE_bin_sha,
    INSTALLED_METADATA_KEY_CODE_dep_pkg,
    INSTALLED_METADATA_KEY_CODE_install,
    INSTALLED_METADATA_KEY_CODE_timestamp,
    INSTALLED_METADATA_KEY_CODE_signature,
} UPPMReceiptKeyCode;

void uppm_receipt_dump(UPPMReceipt * receipt) {
    if (receipt == NULL) {
        return;
    }

    printf("summary: %s\n", receipt->summary);
    printf("webpage: %s\n", receipt->webpage);
    printf("version: %s\n", receipt->version);
    printf("license: %s\n", receipt->license);
    printf("bin_url: %s\n", receipt->bin_url);
    printf("bin_sha: %s\n", receipt->bin_sha);
    printf("dep_pkg: %s\n", receipt->dep_pkg);
    printf("install: %s\n", receipt->install);
    printf("signature: %s\n", receipt->signature);
    printf("timestamp: %s\n", receipt->timestamp);
}

void uppm_receipt_free(UPPMReceipt * receipt) {
    if (receipt == NULL) {
        return;
    }

    if (receipt->summary != NULL) {
        free(receipt->summary);
        receipt->summary = NULL;
    }

    if (receipt->webpage != NULL) {
        free(receipt->webpage);
        receipt->webpage = NULL;
    }

    if (receipt->version != NULL) {
        free(receipt->version);
        receipt->version = NULL;
    }

    if (receipt->license != NULL) {
        free(receipt->license);
        receipt->license = NULL;
    }

    if (receipt->bin_url != NULL) {
        free(receipt->bin_url);
        receipt->bin_url = NULL;
    }

    if (receipt->bin_sha != NULL) {
        free(receipt->bin_sha);
        receipt->bin_sha = NULL;
    }

    if (receipt->dep_pkg != NULL) {
        free(receipt->dep_pkg);
        receipt->dep_pkg = NULL;
    }

    if (receipt->install != NULL) {
        free(receipt->install);
        receipt->install = NULL;
    }

    if (receipt->timestamp != NULL) {
        free(receipt->timestamp);
        receipt->timestamp = NULL;
    }

    if (receipt->signature != NULL) {
        free(receipt->signature);
        receipt->signature = NULL;
    }

    free(receipt);
}

static UPPMReceiptKeyCode uppm_receipt_key_code_from_key_name(char * key) {
           if (strcmp(key, "summary") == 0) {
        return INSTALLED_METADATA_KEY_CODE_summary;
    } else if (strcmp(key, "webpage") == 0) {
        return INSTALLED_METADATA_KEY_CODE_webpage;
    } else if (strcmp(key, "version") == 0) {
        return INSTALLED_METADATA_KEY_CODE_version;
    } else if (strcmp(key, "license") == 0) {
        return INSTALLED_METADATA_KEY_CODE_license;
    } else if (strcmp(key, "bin-url") == 0) {
        return INSTALLED_METADATA_KEY_CODE_bin_url;
    } else if (strcmp(key, "bin-sha") == 0) {
        return INSTALLED_METADATA_KEY_CODE_bin_sha;
    } else if (strcmp(key, "dep-pkg") == 0) {
        return INSTALLED_METADATA_KEY_CODE_dep_pkg;
    } else if (strcmp(key, "install") == 0) {
        return INSTALLED_METADATA_KEY_CODE_install;
    } else if (strcmp(key, "timestamp") == 0) {
        return INSTALLED_METADATA_KEY_CODE_timestamp;
    } else if (strcmp(key, "signature") == 0) {
        return INSTALLED_METADATA_KEY_CODE_signature;
    } else {
        return INSTALLED_METADATA_KEY_CODE_unknown;
    }
}

void uppm_receipt_set_value(UPPMReceiptKeyCode keyCode, char * value, UPPMReceipt * receipt) {
    switch (keyCode) {
        case INSTALLED_METADATA_KEY_CODE_summary:  if (receipt->summary != NULL)   free(receipt->summary);   receipt->summary = strdup(value); break;
        case INSTALLED_METADATA_KEY_CODE_webpage:  if (receipt->webpage != NULL)   free(receipt->webpage);   receipt->webpage = strdup(value); break;
        case INSTALLED_METADATA_KEY_CODE_version:  if (receipt->version != NULL)   free(receipt->version);   receipt->version = strdup(value); break;
        case INSTALLED_METADATA_KEY_CODE_license:  if (receipt->license != NULL)   free(receipt->license);   receipt->license = strdup(value); break;
        case INSTALLED_METADATA_KEY_CODE_bin_url:  if (receipt->bin_url != NULL)   free(receipt->bin_url);   receipt->bin_url = strdup(value); break;
        case INSTALLED_METADATA_KEY_CODE_bin_sha:  if (receipt->bin_sha != NULL)   free(receipt->bin_sha);   receipt->bin_sha = strdup(value); break;
        case INSTALLED_METADATA_KEY_CODE_dep_pkg:  if (receipt->dep_pkg != NULL)   free(receipt->dep_pkg);   receipt->dep_pkg = strdup(value); break;
        case INSTALLED_METADATA_KEY_CODE_install:  if (receipt->install != NULL)   free(receipt->install);   receipt->install = strdup(value); break;
        case INSTALLED_METADATA_KEY_CODE_timestamp:if (receipt->timestamp != NULL) free(receipt->timestamp); receipt->timestamp = strdup(value); break;
        case INSTALLED_METADATA_KEY_CODE_signature:if (receipt->signature != NULL) free(receipt->signature); receipt->signature = strdup(value); break;
        default: break;
    }
}

int uppm_receipt_parse(const char * packageName, UPPMReceipt * * out) {
    int resultCode = uppm_is_package_name(packageName);

    if (resultCode != UPPM_OK) {
        return resultCode;
    }

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

    FILE * file = fopen(receiptFilePath, "r");

    if (file == NULL) {
        perror(receiptFilePath);
        return UPPM_ERROR;
    }

    yaml_parser_t parser;
    yaml_token_t  token;

    // https://libyaml.docsforge.com/master/api/yaml_parser_initialize/
    if (yaml_parser_initialize(&parser) == 0) {
        perror("Failed to initialize yaml parser");
        return UPPM_ERROR;
    }

    yaml_parser_set_input_file(&parser, file);

    UPPMReceiptKeyCode receiptKeyCode = INSTALLED_METADATA_KEY_CODE_unknown;

    UPPMReceipt * receipt = NULL;

    int lastTokenType = 0;

    bool success = true;

    do {
        // https://libyaml.docsforge.com/master/api/yaml_parser_scan/
        if (yaml_parser_scan(&parser, &token) == 0) {
            fprintf(stderr, "syntax error: %s\n", receiptFilePath);
            success = false;
            goto clean;
        }

        switch(token.type) {
            case YAML_KEY_TOKEN:
                lastTokenType = 1;
                break;
            case YAML_VALUE_TOKEN:
                lastTokenType = 2;
                break;
            case YAML_SCALAR_TOKEN:
                if (lastTokenType == 1) {
                    receiptKeyCode = uppm_receipt_key_code_from_key_name((char*)token.data.scalar.value);
                } else if (lastTokenType == 2) {
                    if (receipt == NULL) {
                        receipt = (UPPMReceipt*)calloc(1, sizeof(UPPMReceipt));
                    }
                    uppm_receipt_set_value(receiptKeyCode, (char*)token.data.scalar.value, receipt);
                }
                break;
            default: 
                lastTokenType = 0;
                break;
        }

        if (token.type != YAML_STREAM_END_TOKEN) {
            yaml_token_delete(&token);
        }
    } while(token.type != YAML_STREAM_END_TOKEN);

clean:
    yaml_token_delete(&token);

    yaml_parser_delete(&parser);

    fclose(file);

    if (success) {
        if ((receipt->timestamp == NULL) || (strcmp(receipt->timestamp, "") == 0)) {
            fprintf(stderr, "timestamp not configed in %s\n", receiptFilePath);
            uppm_receipt_free(receipt);
            return UPPM_ERROR;
        }

        size_t timestampLength = strlen(receipt->timestamp);

        if (timestampLength != 10) {
            fprintf(stderr, "value of timestamp length must be 10\n");
            uppm_receipt_free(receipt);
            return UPPM_ERROR;
        }

        for (size_t i = 0; i < 10; i++) {
            if ((receipt->timestamp[i] < '0') || (receipt->timestamp[i] > '9')) {
                fprintf(stderr, "timestamp contains non-numeric character.\n");
                uppm_receipt_free(receipt);
                return UPPM_ERROR;
            }
        }

        if ((receipt->signature == NULL) || (strcmp(receipt->signature, "") == 0)) {
            fprintf(stderr, "signature not configed in %s\n", receiptFilePath);
            uppm_receipt_free(receipt);
            return UPPM_ERROR;
        }

        if ((receipt->bin_url == NULL) || (strcmp(receipt->bin_url, "") == 0)) {
            fprintf(stderr, "bin-url not configed in %s\n", receiptFilePath);
            uppm_receipt_free(receipt);
            return UPPM_ERROR;
        }

        if ((receipt->bin_sha == NULL) || (strcmp(receipt->bin_url, "") == 0)) {
            fprintf(stderr, "bin-sha not configed in %s\n", receiptFilePath);
            uppm_receipt_free(receipt);
            return UPPM_ERROR;
        }

        if (strlen(receipt->bin_sha) != 64) {
            fprintf(stderr, "value of bin-sha length must be 64\n");
            uppm_receipt_free(receipt);
            return UPPM_ERROR;
        }

        (*out) = receipt;
        return UPPM_OK;
    } else {
        if (receipt != NULL) {
            uppm_receipt_free(receipt);
        } else {
            //free(receiptFilePath);
        }
        return UPPM_ERROR;
    }
}
