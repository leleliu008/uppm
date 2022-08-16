#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <yaml.h>
#include "uppm.h"

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
    INSTALLED_METADATA_KEY_CODE_datetime,
    INSTALLED_METADATA_KEY_CODE_uppmvers,
} UPPMInstalledMetadataKeyCode;

void uppm_installed_metadata_dump(UPPMInstalledMetadata * metadata) {
    if (metadata != NULL) {
        printf("summary: %s\n", metadata->summary);
        printf("webpage: %s\n", metadata->webpage);
        printf("version: %s\n", metadata->version);
        printf("license: %s\n", metadata->license);
        printf("bin_url: %s\n", metadata->bin_url);
        printf("bin_sha: %s\n", metadata->bin_sha);
        printf("dep_pkg: %s\n", metadata->dep_pkg);
        printf("install: %s\n", metadata->install);
        printf("datetime: %s\n", metadata->datatime);
        printf("uppmvers: %s\n", metadata->uppmvers);
    }
}

void uppm_installed_metadata_free(UPPMInstalledMetadata * metadata) {
    if (metadata != NULL) {
        if (metadata->summary != NULL) {
            free(metadata->summary);
            metadata->summary = NULL;
        }

        if (metadata->webpage != NULL) {
            free(metadata->webpage);
            metadata->webpage = NULL;
        }

        if (metadata->version != NULL) {
            free(metadata->version);
            metadata->version = NULL;
        }

        if (metadata->license != NULL) {
            free(metadata->license);
            metadata->license = NULL;
        }

        if (metadata->bin_url != NULL) {
            free(metadata->bin_url);
            metadata->bin_url = NULL;
        }

        if (metadata->bin_sha != NULL) {
            free(metadata->bin_sha);
            metadata->bin_sha = NULL;
        }

        if (metadata->dep_pkg != NULL) {
            free(metadata->dep_pkg);
            metadata->dep_pkg = NULL;
        }

        if (metadata->install != NULL) {
            free(metadata->install);
            metadata->install = NULL;
        }

        if (metadata->datatime != NULL) {
            free(metadata->datatime);
            metadata->datatime = NULL;
        }

        if (metadata->uppmvers != NULL) {
            free(metadata->uppmvers);
            metadata->uppmvers = NULL;
        }

        free(metadata);
    }
}

static UPPMInstalledMetadataKeyCode uppm_installed_metadata_key_code_from_key_name(char * key) {
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
    } else if (strcmp(key, "datatime") == 0) {
        return INSTALLED_METADATA_KEY_CODE_datetime;
    } else if (strcmp(key, "uppmvers") == 0) {
        return INSTALLED_METADATA_KEY_CODE_uppmvers;
    } else {
        return INSTALLED_METADATA_KEY_CODE_unknown;
    }
}

void uppm_installed_metadata_set_value(UPPMInstalledMetadataKeyCode keyCode, char * value, UPPMInstalledMetadata * metadata) {
    value = strdup(value);
    switch (keyCode) {
        case INSTALLED_METADATA_KEY_CODE_summary:  metadata->summary = value; break;
        case INSTALLED_METADATA_KEY_CODE_webpage:  metadata->webpage = value; break;
        case INSTALLED_METADATA_KEY_CODE_version:  metadata->version = value; break;
        case INSTALLED_METADATA_KEY_CODE_license:  metadata->license = value; break;
        case INSTALLED_METADATA_KEY_CODE_bin_url:  metadata->bin_url = value; break;
        case INSTALLED_METADATA_KEY_CODE_bin_sha:  metadata->bin_sha = value; break;
        case INSTALLED_METADATA_KEY_CODE_dep_pkg:  metadata->dep_pkg = value; break;
        case INSTALLED_METADATA_KEY_CODE_install:  metadata->install = value; break;
        case INSTALLED_METADATA_KEY_CODE_datetime:  metadata->datatime = value; break;
        case INSTALLED_METADATA_KEY_CODE_uppmvers:  metadata->uppmvers = value; break;
        default: break;
    }
}

int uppm_installed_metadata_parse(const char * packageName, UPPMInstalledMetadata * * out) {
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

    size_t  installedMetadataFilePathLength = installedDirLength + 26;
    char    installedMetadataFilePath[installedMetadataFilePathLength];
    memset (installedMetadataFilePath, 0, installedMetadataFilePathLength);
    sprintf(installedMetadataFilePath, "%s/installed-metadata-uppm", installedDir);

    FILE * file = fopen(installedMetadataFilePath, "r");

    if (file == NULL) {
        perror(installedMetadataFilePath);
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

    UPPMInstalledMetadataKeyCode metadataKeyCode = INSTALLED_METADATA_KEY_CODE_unknown;

    UPPMInstalledMetadata * metadata = NULL;

    int lastTokenType = 0;

    bool success = true;

    do {
        // https://libyaml.docsforge.com/master/api/yaml_parser_scan/
        if (yaml_parser_scan(&parser, &token) == 0) {
            fprintf(stderr, "syntax error: %s\n", installedMetadataFilePath);
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
                    metadataKeyCode = uppm_installed_metadata_key_code_from_key_name((char*)token.data.scalar.value);
                } else if (lastTokenType == 2) {
                    if (metadata == NULL) {
                        metadata = (UPPMInstalledMetadata*)calloc(1, sizeof(UPPMInstalledMetadata));
                    }
                    uppm_installed_metadata_set_value(metadataKeyCode, (char*)token.data.scalar.value, metadata);
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
        if ((metadata->datatime == NULL) || (strcmp(metadata->datatime, "") == 0)) {
            fprintf(stderr, "datatime not configed in %s\n", installedMetadataFilePath);
            uppm_installed_metadata_free(metadata);
            return UPPM_ERROR;
        }

        size_t datatimeLength = strlen(metadata->datatime);

        if (datatimeLength != 10) {
            fprintf(stderr, "value of datatime length must be 64\n");
            uppm_installed_metadata_free(metadata);
            return UPPM_ERROR;
        }

        for (size_t i = 0; i < 10; i++) {
            if ((metadata->datatime[i] < '0') || (metadata->datatime[i] > '9')) {
                fprintf(stderr, "datatime contains non-numeric character.\n");
                uppm_installed_metadata_free(metadata);
                return UPPM_ERROR;
            }
        }

        if ((metadata->uppmvers == NULL) || (strcmp(metadata->uppmvers, "") == 0)) {
            fprintf(stderr, "uppmvers not configed in %s\n", installedMetadataFilePath);
            uppm_installed_metadata_free(metadata);
            return UPPM_ERROR;
        }

        if ((metadata->bin_url == NULL) || (strcmp(metadata->bin_url, "") == 0)) {
            fprintf(stderr, "bin-url not configed in %s\n", installedMetadataFilePath);
            uppm_installed_metadata_free(metadata);
            return UPPM_ERROR;
        }

        if ((metadata->bin_sha == NULL) || (strcmp(metadata->bin_url, "") == 0)) {
            fprintf(stderr, "bin-sha not configed in %s\n", installedMetadataFilePath);
            uppm_installed_metadata_free(metadata);
            return UPPM_ERROR;
        }

        if (strlen(metadata->bin_sha) != 64) {
            fprintf(stderr, "value of bin-sha length must be 64\n");
            uppm_installed_metadata_free(metadata);
            return UPPM_ERROR;
        }

        (*out) = metadata;
        return UPPM_OK;
    } else {
        if (metadata != NULL) {
            uppm_installed_metadata_free(metadata);
        } else {
            //free(installedMetadataFilePath);
        }
        return UPPM_ERROR;
    }
}
