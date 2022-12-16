#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <yaml.h>
#include "uppm.h"
#include "core/fs.h"

typedef enum {
    INSTALLED_METADATA_KEY_CODE_unknown,
    INSTALLED_METADATA_KEY_CODE_summary,
    INSTALLED_METADATA_KEY_CODE_version,
    INSTALLED_METADATA_KEY_CODE_license,
    INSTALLED_METADATA_KEY_CODE_web_url,
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
    printf("version: %s\n", receipt->version);
    printf("license: %s\n", receipt->license);
    printf("web-url: %s\n", receipt->web_url);
    printf("bin-url: %s\n", receipt->bin_url);
    printf("bin-sha: %s\n", receipt->bin_sha);
    printf("dep-pkg: %s\n", receipt->dep_pkg);
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

    if (receipt->version != NULL) {
        free(receipt->version);
        receipt->version = NULL;
    }

    if (receipt->license != NULL) {
        free(receipt->license);
        receipt->license = NULL;
    }

    if (receipt->web_url != NULL) {
        free(receipt->web_url);
        receipt->web_url = NULL;
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
    } else if (strcmp(key, "web_url") == 0) {
        return INSTALLED_METADATA_KEY_CODE_web_url;
    } else if (strcmp(key, "webpage") == 0) {
        return INSTALLED_METADATA_KEY_CODE_web_url;
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
        case INSTALLED_METADATA_KEY_CODE_version:  if (receipt->version != NULL)   free(receipt->version);   receipt->version = strdup(value); break;
        case INSTALLED_METADATA_KEY_CODE_license:  if (receipt->license != NULL)   free(receipt->license);   receipt->license = strdup(value); break;
        case INSTALLED_METADATA_KEY_CODE_web_url:  if (receipt->web_url != NULL)   free(receipt->web_url);   receipt->web_url = strdup(value); break;
        case INSTALLED_METADATA_KEY_CODE_bin_url:  if (receipt->bin_url != NULL)   free(receipt->bin_url);   receipt->bin_url = strdup(value); break;
        case INSTALLED_METADATA_KEY_CODE_bin_sha:  if (receipt->bin_sha != NULL)   free(receipt->bin_sha);   receipt->bin_sha = strdup(value); break;
        case INSTALLED_METADATA_KEY_CODE_dep_pkg:  if (receipt->dep_pkg != NULL)   free(receipt->dep_pkg);   receipt->dep_pkg = strdup(value); break;
        case INSTALLED_METADATA_KEY_CODE_install:  if (receipt->install != NULL)   free(receipt->install);   receipt->install = strdup(value); break;
        case INSTALLED_METADATA_KEY_CODE_timestamp:if (receipt->timestamp != NULL) free(receipt->timestamp); receipt->timestamp = strdup(value); break;
        case INSTALLED_METADATA_KEY_CODE_signature:if (receipt->signature != NULL) free(receipt->signature); receipt->signature = strdup(value); break;
        default: break;
    }
}

static int uppm_receipt_check(UPPMReceipt * receipt, const char * receiptFilePath) {
    if (receipt->summary == NULL) {
        fprintf(stderr, "scheme error in receipt file: %s : summary field not found.\n", receiptFilePath);
        return UPPM_RECEIPT_SCHEME_ERROR;
    }

    if (strcmp(receipt->summary, "") == 0) {
        fprintf(stderr, "scheme error in receipt file: %s : summary field's value must not be empty.\n", receiptFilePath);
        return UPPM_RECEIPT_SCHEME_ERROR;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////

    if (receipt->version == NULL) {
        fprintf(stderr, "scheme error in receipt file: %s : version field not found.\n", receiptFilePath);
        return UPPM_RECEIPT_SCHEME_ERROR;
    }

    if (strcmp(receipt->version, "") == 0) {
        fprintf(stderr, "scheme error in receipt file: %s : version field's value must not be empty.\n", receiptFilePath);
        return UPPM_RECEIPT_SCHEME_ERROR;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////

    if (receipt->web_url == NULL) {
        fprintf(stderr, "scheme error in receipt file: %s : web-url field not found.\n", receiptFilePath);
        return UPPM_RECEIPT_SCHEME_ERROR;
    }

    if (strcmp(receipt->web_url, "") == 0) {
        fprintf(stderr, "scheme error in receipt file: %s : web-url field's value must not be empty.\n", receiptFilePath);
        return UPPM_RECEIPT_SCHEME_ERROR;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////

    if (receipt->bin_url == NULL) {
        fprintf(stderr, "scheme error in receipt file: %s : bin-url field not found.\n", receiptFilePath);
        return UPPM_RECEIPT_SCHEME_ERROR;
    }

    if (strcmp(receipt->bin_url, "") == 0) {
        fprintf(stderr, "scheme error in receipt file: %s : bin-url field's value must not be empty.\n", receiptFilePath);
        return UPPM_RECEIPT_SCHEME_ERROR;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////

    if (receipt->bin_sha == NULL) {
        fprintf(stderr, "scheme error in receipt file: %s : bin-sha field not found.\n", receiptFilePath);
        return UPPM_RECEIPT_SCHEME_ERROR;
    }

    if (strlen(receipt->bin_sha) != 64) {
        fprintf(stderr, "scheme error in receipt file: %s : bin-sha field's value's length must be 64.\n", receiptFilePath);
        return UPPM_RECEIPT_SCHEME_ERROR;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////

    if (receipt->timestamp == NULL) {
        fprintf(stderr, "scheme error in receipt file: %s : timestamp field not found.\n", receiptFilePath);
        return UPPM_RECEIPT_SCHEME_ERROR;
    }

    size_t i = 0;
    char   c;

    for (;; i++) {
        c = receipt->timestamp[i];

        if (c == '\0') {
            break;
        }

        if ((c < '0') || (c > '9')) {
            fprintf(stderr, "scheme error in receipt file: %s : timestamp field's value should only contains non-numeric characters.\n", receiptFilePath);
            return UPPM_RECEIPT_SCHEME_ERROR;
        }
    }

    if (i != 10) {
        fprintf(stderr, "scheme error in receipt file: %s : timestamp field's value's length must be 10.\n", receiptFilePath);
        return UPPM_RECEIPT_SCHEME_ERROR;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////

    if (receipt->signature == NULL) {
        fprintf(stderr, "scheme error in receipt file: %s : signature field not found.\n", receiptFilePath);
        return UPPM_RECEIPT_SCHEME_ERROR;
    }

    if (strcmp(receipt->signature, "") == 0) {
        fprintf(stderr, "scheme error in receipt file: %s : signature field's value must not be empty.\n", receiptFilePath);
        return UPPM_RECEIPT_SCHEME_ERROR;
    }

    return UPPM_OK;
}


int uppm_receipt_parse(const char * packageName, UPPMReceipt * * out) {
    int resultCode = uppm_check_if_the_given_argument_matches_package_name_pattern(packageName);

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

    do {
        // https://libyaml.docsforge.com/master/api/yaml_parser_scan/
        if (yaml_parser_scan(&parser, &token) == 0) {
            fprintf(stderr, "syntax error in receipt file: %s\n", receiptFilePath);
            resultCode = UPPM_RECEIPT_SYNTAX_ERROR;
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

    //uppm_receipt_dump(receipt);

    if (resultCode == UPPM_OK) {
        resultCode = uppm_receipt_check(receipt, receiptFilePath);

        if (resultCode == UPPM_OK) {
            (*out) = receipt;
            return UPPM_OK;
        }
    }

    uppm_receipt_free(receipt);

    return resultCode;
}
