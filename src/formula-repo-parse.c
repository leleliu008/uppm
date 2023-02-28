#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <yaml.h>

#include "uppm.h"

typedef enum {
    UPPMFormulaRepoKeyCode_unknown,
    UPPMFormulaRepoKeyCode_url,
    UPPMFormulaRepoKeyCode_branch,
    UPPMFormulaRepoKeyCode_pinned,
    UPPMFormulaRepoKeyCode_enabled,
    UPPMFormulaRepoKeyCode_timestamp_created,
    UPPMFormulaRepoKeyCode_timestamp_updated
} UPPMFormulaRepoKeyCode;

void uppm_formula_repo_dump(UPPMFormulaRepo * formulaRepo) {
    if (formulaRepo == NULL) {
        return;
    }

    printf("name: %s\n", formulaRepo->name);
    printf("path: %s\n", formulaRepo->path);
    printf("url:  %s\n", formulaRepo->url);
    printf("branch: %s\n", formulaRepo->branch);
    printf("pinned: %s\n", formulaRepo->pinned ? "yes" : "no");
    printf("enabled: %s\n", formulaRepo->enabled ? "yes" : "no");
    printf("timestamp-created: %s\n", formulaRepo->timestamp_created);
    printf("timestamp-updated: %s\n", formulaRepo->timestamp_updated);
}

void uppm_formula_repo_free(UPPMFormulaRepo * formulaRepo) {
    if (formulaRepo == NULL) {
        return;
    }

    if (formulaRepo->name != NULL) {
        free(formulaRepo->name);
        formulaRepo->name = NULL;
    }

    if (formulaRepo->path != NULL) {
        free(formulaRepo->path);
        formulaRepo->path = NULL;
    }

    if (formulaRepo->url != NULL) {
        free(formulaRepo->url);
        formulaRepo->url = NULL;
    }

    if (formulaRepo->branch != NULL) {
        free(formulaRepo->branch);
        formulaRepo->branch = NULL;
    }

    if (formulaRepo->timestamp_created != NULL) {
        free(formulaRepo->timestamp_created);
        formulaRepo->timestamp_created = NULL;
    }

    if (formulaRepo->timestamp_updated != NULL) {
        free(formulaRepo->timestamp_updated);
        formulaRepo->timestamp_updated = NULL;
    }

    free(formulaRepo);
}

static UPPMFormulaRepoKeyCode uppm_formula_repo_key_code_from_key_name(char * key) {
           if (strcmp(key, "url") == 0) {
        return UPPMFormulaRepoKeyCode_url;
    } else if (strcmp(key, "branch") == 0) {
        return UPPMFormulaRepoKeyCode_branch;
    } else if (strcmp(key, "pinned") == 0) {
        return UPPMFormulaRepoKeyCode_pinned;
    } else if (strcmp(key, "enabled") == 0) {
        return UPPMFormulaRepoKeyCode_enabled;
    } else if (strcmp(key, "timestamp-created") == 0) {
        return UPPMFormulaRepoKeyCode_timestamp_created;
    } else if (strcmp(key, "timestamp-updated") == 0) {
        return UPPMFormulaRepoKeyCode_timestamp_updated;
    } else {
        return UPPMFormulaRepoKeyCode_unknown;
    }
}

static int uppm_formula_repo_set_value(UPPMFormulaRepoKeyCode keyCode, char * value, UPPMFormulaRepo * formulaRepo) {
    if (keyCode == UPPMFormulaRepoKeyCode_unknown) {
        return UPPM_OK;
    }

    char c;

    for (;;) {
        c = value[0];

        if (c == '\0') {
            return UPPM_OK;
        }

        // non-printable ASCII characters and space
        if (c <= 32) {
            value = &value[1];
        } else {
            break;
        }
    }

    switch (keyCode) {
        case UPPMFormulaRepoKeyCode_url:
            if (formulaRepo->url != NULL) {
                free(formulaRepo->url);
            }

            formulaRepo->url = strdup(value);

            return formulaRepo->url == NULL ? UPPM_ERROR_MEMORY_ALLOCATE : UPPM_OK;
        case UPPMFormulaRepoKeyCode_branch:
            if (formulaRepo->branch != NULL) {
                free(formulaRepo->branch);
            }

            formulaRepo->branch = strdup(value);

            return formulaRepo->branch == NULL ? UPPM_ERROR_MEMORY_ALLOCATE : UPPM_OK;
        case UPPMFormulaRepoKeyCode_timestamp_created:
            if (formulaRepo->timestamp_created != NULL) {
                free(formulaRepo->timestamp_created);
            }

            formulaRepo->timestamp_created = strdup(value);

            return formulaRepo->timestamp_created == NULL ? UPPM_ERROR_MEMORY_ALLOCATE : UPPM_OK;
        case UPPMFormulaRepoKeyCode_timestamp_updated:
            free(formulaRepo->timestamp_updated);
            formulaRepo->timestamp_updated = strdup(value);

            return formulaRepo->timestamp_updated == NULL ? UPPM_ERROR_MEMORY_ALLOCATE : UPPM_OK;
        case UPPMFormulaRepoKeyCode_pinned:
            if (strcmp(value, "1") == 0) {
                formulaRepo->pinned = 1;
                return UPPM_OK;
            } else if (strcmp(value, "0") == 0) {
                formulaRepo->pinned = 0;
                return UPPM_OK;
            } else {
                return UPPM_ERROR_FORMULA_REPO_CONFIG_SCHEME;
            }
        case UPPMFormulaRepoKeyCode_enabled:
            if (strcmp(value, "1") == 0) {
                formulaRepo->enabled = 1;
                return UPPM_OK;
            } else if (strcmp(value, "0") == 0) {
                formulaRepo->enabled = 0;
                return UPPM_OK;
            } else {
                return UPPM_ERROR_FORMULA_REPO_CONFIG_SCHEME;
            }
        case UPPMFormulaRepoKeyCode_unknown:
        default:
            return UPPM_OK;
    }
}

static int uppm_formula_repo_check(UPPMFormulaRepo * formulaRepo, const char * formulaRepoConfigFilePath) {
    if (formulaRepo->url == NULL) {
        fprintf(stderr, "scheme error in formula repo config file: %s : summary mapping not found.\n", formulaRepoConfigFilePath);
        return UPPM_ERROR_FORMULA_REPO_CONFIG_SCHEME;
    }

    if (formulaRepo->branch == NULL) {
        fprintf(stderr, "scheme error in formula repo config file: %s : summary mapping not found.\n", formulaRepoConfigFilePath);
        return UPPM_ERROR_FORMULA_REPO_CONFIG_SCHEME;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    if (formulaRepo->timestamp_created == NULL) {
        fprintf(stderr, "scheme error in formula repo config file: %s : timestamp-created mapping not found.\n", formulaRepoConfigFilePath);
        return UPPM_ERROR_FORMULA_REPO_CONFIG_SCHEME;
    }

    size_t i = 0;
    char   c;

    for (;; i++) {
        c = formulaRepo->timestamp_created[i];

        if (c == '\0') {
            break;
        }

        if ((c < '0') || (c > '9')) {
            fprintf(stderr, "scheme error in formula repo config file: %s : timestamp-created mapping's value should only contains non-numeric characters.\n", formulaRepoConfigFilePath);
            return UPPM_ERROR_FORMULA_REPO_CONFIG_SCHEME;
        }
    }

    if (i != 10) {
        fprintf(stderr, "scheme error in formula repo config file: %s : timestamp-created mapping's value's length must be 10.\n", formulaRepoConfigFilePath);
        return UPPM_ERROR_FORMULA_REPO_CONFIG_SCHEME;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    if (formulaRepo->timestamp_updated != NULL) {
        i = 0;

        for (;; i++) {
            c = formulaRepo->timestamp_updated[i];

            if (c == '\0') {
                break;
            }

            if ((c < '0') || (c > '9')) {
                fprintf(stderr, "scheme error in formula repo config file: %s : timestamp-updated mapping's value should only contains non-numeric characters.\n", formulaRepoConfigFilePath);
                return UPPM_ERROR_FORMULA_REPO_CONFIG_SCHEME;
            }
        }

        if (i != 10) {
            fprintf(stderr, "scheme error in formula repo config file: %s : timestamp-updated mapping's value's length must be 10.\n", formulaRepoConfigFilePath);
            return UPPM_ERROR_FORMULA_REPO_CONFIG_SCHEME;
        }
    }

    return UPPM_OK;
}

int uppm_formula_repo_parse(const char * formulaRepoConfigFilePath, UPPMFormulaRepo * * out) {
    FILE * file = fopen(formulaRepoConfigFilePath, "r");

    if (file == NULL) {
        perror(formulaRepoConfigFilePath);
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

    UPPMFormulaRepoKeyCode formulaRepoKeyCode = UPPMFormulaRepoKeyCode_unknown;

    UPPMFormulaRepo * formulaRepo = NULL;

    int lastTokenType = 0;

    int ret = UPPM_OK;

    do {
        // https://libyaml.docsforge.com/master/api/yaml_parser_scan/
        if (yaml_parser_scan(&parser, &token) == 0) {
            fprintf(stderr, "syntax error in formula repo config file: %s\n", formulaRepoConfigFilePath);
            ret = UPPM_ERROR_FORMULA_REPO_CONFIG_SYNTAX;
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
                    formulaRepoKeyCode = uppm_formula_repo_key_code_from_key_name((char*)token.data.scalar.value);
                } else if (lastTokenType == 2) {
                    if (formulaRepo == NULL) {
                        formulaRepo = (UPPMFormulaRepo*)calloc(1, sizeof(UPPMFormulaRepo));

                        if (formulaRepo == NULL) {
                            ret = UPPM_ERROR_MEMORY_ALLOCATE;
                            goto clean;
                        }

                        formulaRepo->enabled = 1;
                    }

                    ret = uppm_formula_repo_set_value(formulaRepoKeyCode, (char*)token.data.scalar.value, formulaRepo);

                    if (ret != UPPM_OK) {
                        goto clean;
                    }
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

    if (ret == UPPM_OK) {
        ret = uppm_formula_repo_check(formulaRepo, formulaRepoConfigFilePath);
    }

    //uppm_formula_repo_dump(formulaRepo);

    if (ret == UPPM_OK) {
        (*out) = formulaRepo;
        return UPPM_OK;
    } else {
        uppm_formula_repo_free(formulaRepo);
        return ret;
    }
}
