#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <yaml.h>
#include "core/sysinfo.h"
#include "uppm.h"

typedef enum {
    FORMULA_KEY_CODE_unknown,
    FORMULA_KEY_CODE_summary,
    FORMULA_KEY_CODE_version,
    FORMULA_KEY_CODE_license,
    FORMULA_KEY_CODE_web_url,
    FORMULA_KEY_CODE_bin_url,
    FORMULA_KEY_CODE_bin_sha,
    FORMULA_KEY_CODE_res_url,
    FORMULA_KEY_CODE_res_sha,
    FORMULA_KEY_CODE_dep_pkg,
    FORMULA_KEY_CODE_install,
} UPPMFormulaKeyCode;

void uppm_formula_dump(UPPMFormula * formula) {
    if (formula == NULL) {
        return;
    }

    printf("summary: %s\n", formula->summary);
    printf("version: %s\n", formula->version);
    printf("license: %s\n", formula->license);
    printf("web-url: %s\n", formula->web_url);
    printf("bin-url: %s\n", formula->bin_url);
    printf("bin-sha: %s\n", formula->bin_sha);
    printf("dep-pkg: %s\n", formula->dep_pkg);
    printf("install: %s\n", formula->install);
    printf("path:    %s\n", formula->path);
}

void uppm_formula_free(UPPMFormula * formula) {
    if (formula == NULL) {
        return;
    }

    if (formula->summary != NULL) {
        free(formula->summary);
        formula->summary = NULL;
    }

    if (formula->version != NULL) {
        free(formula->version);
        formula->version = NULL;
    }

    if (formula->license != NULL) {
        free(formula->license);
        formula->license = NULL;
    }

    if (formula->web_url != NULL) {
        free(formula->web_url);
        formula->web_url = NULL;
    }

    if (formula->bin_url != NULL) {
        free(formula->bin_url);
        formula->bin_url = NULL;
    }

    if (formula->bin_sha != NULL) {
        free(formula->bin_sha);
        formula->bin_sha = NULL;
    }

    if (formula->dep_pkg != NULL) {
        free(formula->dep_pkg);
        formula->dep_pkg = NULL;
    }

    if (formula->install != NULL) {
        free(formula->install);
        formula->install = NULL;
    }

    if (formula->path != NULL) {
        free(formula->path);
        formula->path = NULL;
    }

    free(formula);
}

static UPPMFormulaKeyCode uppm_formula_key_code_from_key_name(char * key) {
           if (strcmp(key, "summary") == 0) {
        return FORMULA_KEY_CODE_summary;
    } else if (strcmp(key, "web_url") == 0) {
        return FORMULA_KEY_CODE_web_url;
    } else if (strcmp(key, "webpage") == 0) {
        return FORMULA_KEY_CODE_web_url;
    } else if (strcmp(key, "version") == 0) {
        return FORMULA_KEY_CODE_version;
    } else if (strcmp(key, "license") == 0) {
        return FORMULA_KEY_CODE_license;
    } else if (strcmp(key, "bin-url") == 0) {
        return FORMULA_KEY_CODE_bin_url;
    } else if (strcmp(key, "bin-sha") == 0) {
        return FORMULA_KEY_CODE_bin_sha;
    } else if (strcmp(key, "res-url") == 0) {
        return FORMULA_KEY_CODE_res_url;
    } else if (strcmp(key, "res-sha") == 0) {
        return FORMULA_KEY_CODE_res_sha;
    } else if (strcmp(key, "dep-pkg") == 0) {
        return FORMULA_KEY_CODE_dep_pkg;
    } else if (strcmp(key, "install") == 0) {
        return FORMULA_KEY_CODE_install;
    } else {
        return FORMULA_KEY_CODE_unknown;
    }
}

static void uppm_formula_set_value(UPPMFormulaKeyCode keyCode, char * value, UPPMFormula * formula, bool isLinuxMuslLibc) {
    switch (keyCode) {
        case FORMULA_KEY_CODE_summary: if (formula->summary != NULL) free(formula->summary); formula->summary = strdup(value); break;
        case FORMULA_KEY_CODE_version: if (formula->version != NULL) free(formula->version); formula->version = strdup(value); break;
        case FORMULA_KEY_CODE_license: if (formula->license != NULL) free(formula->license); formula->license = strdup(value); break;
        case FORMULA_KEY_CODE_web_url: if (formula->web_url != NULL) free(formula->web_url); formula->web_url = strdup(value); break;
        case FORMULA_KEY_CODE_bin_url: if (formula->bin_url != NULL) free(formula->bin_url); formula->bin_url = strdup(value); break;
        case FORMULA_KEY_CODE_bin_sha: if (formula->bin_sha != NULL) free(formula->bin_sha); formula->bin_sha = strdup(value); break;
        case FORMULA_KEY_CODE_dep_pkg: if (formula->dep_pkg != NULL) free(formula->dep_pkg); formula->dep_pkg = strdup(value); break;
        case FORMULA_KEY_CODE_install: if (formula->install != NULL) free(formula->install); formula->install = strdup(value); break;
        case FORMULA_KEY_CODE_res_url:
            if (isLinuxMuslLibc) {
                if (formula->bin_url != NULL) {
                    free(formula->bin_url);
                }
                formula->bin_url = strdup(value);
            }
            break;
        case FORMULA_KEY_CODE_res_sha:
            if (isLinuxMuslLibc) {
                if (formula->bin_sha != NULL) {
                    free(formula->bin_sha);
                }
                formula->bin_sha = strdup(value);
            }
            break;
        default: break;
    }
}

static int uppm_formula_check(UPPMFormula * formula, const char * formulaFilePath) {
    if (formula->summary == NULL) {
        fprintf(stderr, "scheme error in formula file: %s : summary mapping not found.\n", formulaFilePath);
        return UPPM_FORMULA_SCHEME_ERROR;
    }

    if (strcmp(formula->summary, "") == 0) {
        fprintf(stderr, "scheme error in formula file: %s : summary mapping's value must not be empty.\n", formulaFilePath);
        return UPPM_FORMULA_SCHEME_ERROR;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////

    if (formula->version == NULL) {
        fprintf(stderr, "scheme error in formula file: %s : version mapping not found.\n", formulaFilePath);
        return UPPM_FORMULA_SCHEME_ERROR;
    }

    if (strcmp(formula->version, "") == 0) {
        fprintf(stderr, "scheme error in formula file: %s : version mapping's value must not be empty.\n", formulaFilePath);
        return UPPM_FORMULA_SCHEME_ERROR;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////

    if (formula->web_url == NULL) {
        fprintf(stderr, "scheme error in formula file: %s : web-url mapping not found.\n", formulaFilePath);
        return UPPM_FORMULA_SCHEME_ERROR;
    }

    if (strcmp(formula->web_url, "") == 0) {
        fprintf(stderr, "scheme error in formula file: %s : web-url mapping's value must not be empty.\n", formulaFilePath);
        return UPPM_FORMULA_SCHEME_ERROR;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////

    if (formula->bin_url == NULL) {
        fprintf(stderr, "scheme error in formula file: %s : bin-url mapping not found.\n", formulaFilePath);
        return UPPM_FORMULA_SCHEME_ERROR;
    }

    if (strcmp(formula->bin_url, "") == 0) {
        fprintf(stderr, "scheme error in formula file: %s : bin-url mapping's value must not be empty.\n", formulaFilePath);
        return UPPM_FORMULA_SCHEME_ERROR;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////

    if (formula->bin_sha == NULL) {
        fprintf(stderr, "scheme error in formula file: %s : bin-sha mapping not found.\n", formulaFilePath);
        return UPPM_FORMULA_SCHEME_ERROR;
    }

    if (strlen(formula->bin_sha) != 64) {
        fprintf(stderr, "scheme error in formula file: %s : bin-sha mapping's value's length must be 64.\n", formulaFilePath);
        return UPPM_FORMULA_SCHEME_ERROR;
    }

    return UPPM_OK;
}

int uppm_formula_parse(const char * packageName, UPPMFormula * * out) {
    bool isLinuxMuslLibc = false;

    LIBC libc = LIBC_UNKNOWN;

    if (sysinfo_libc(&libc) == 0) {
        isLinuxMuslLibc = libc == LIBC_MUSL;
    } else {
        return UPPM_ERROR;
    }

    char * formulaFilePath = NULL;

    int resultCode = uppm_formula_path(packageName, &formulaFilePath);

    if (resultCode != UPPM_OK) {
        return resultCode;
    }

    FILE * formulaFile = fopen(formulaFilePath, "r");

    if (formulaFile == NULL) {
        perror(formulaFilePath);
        free(formulaFilePath);
        return UPPM_ERROR;
    }

    yaml_parser_t parser;
    yaml_token_t  token;

    // https://libyaml.docsforge.com/master/api/yaml_parser_initialize/
    if (yaml_parser_initialize(&parser) == 0) {
        perror("Failed to initialize yaml parser");
        free(formulaFilePath);
        return UPPM_ERROR;
    }

    yaml_parser_set_input_file(&parser, formulaFile);

    UPPMFormulaKeyCode formulaKeyCode = FORMULA_KEY_CODE_unknown;

    UPPMFormula * formula = NULL;

    int lastTokenType = 0;

    do {
        // https://libyaml.docsforge.com/master/api/yaml_parser_scan/
        if (yaml_parser_scan(&parser, &token) == 0) {
            fprintf(stderr, "syntax error in formula file: %s\n", formulaFilePath);
            resultCode = UPPM_FORMULA_SYNTAX_ERROR;
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
                    formulaKeyCode = uppm_formula_key_code_from_key_name((char*)token.data.scalar.value);
                } else if (lastTokenType == 2) {
                    if (formula == NULL) {
                        formula = (UPPMFormula*)calloc(1, sizeof(UPPMFormula));
                        formula->path = formulaFilePath;
                    }
                    uppm_formula_set_value(formulaKeyCode, (char*)token.data.scalar.value, formula, isLinuxMuslLibc);
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

    fclose(formulaFile);

    //uppm_formula_dump(formula);

    if (resultCode == UPPM_OK) {
        resultCode = uppm_formula_check(formula, formulaFilePath);

        if (resultCode == UPPM_OK) {
            (*out) = formula;
            return UPPM_OK;
        }
    }

    if (formula == NULL) {
        free(formulaFilePath);
    } else {
        uppm_formula_free(formula);
    }

    return resultCode;
}
