#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <libgen.h>
#include <yaml.h>

#include "core/sysinfo.h"
#include "core/regex/regex.h"
#include "uppm.h"

typedef enum {
    UPPMFormulaKeyCode_unknown,
    UPPMFormulaKeyCode_summary,
    UPPMFormulaKeyCode_version,
    UPPMFormulaKeyCode_license,
    UPPMFormulaKeyCode_webpage,
    UPPMFormulaKeyCode_bin_url,
    UPPMFormulaKeyCode_bin_sha,
    UPPMFormulaKeyCode_dep_pkg,
    UPPMFormulaKeyCode_install,
} UPPMFormulaKeyCode;

void uppm_formula_dump(UPPMFormula * formula) {
    if (formula == NULL) {
        return;
    }

    printf("summary: %s\n", formula->summary);
    printf("version: %s\n", formula->version);
    printf("license: %s\n", formula->license);
    printf("webpage: %s\n", formula->webpage);
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

    if (formula->webpage != NULL) {
        free(formula->webpage);
        formula->webpage = NULL;
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

static UPPMFormulaKeyCode uppm_formula_key_code_from_key_name(char * key, bool isLinuxMuslLibc) {
           if (strcmp(key, "summary") == 0) {
        return UPPMFormulaKeyCode_summary;
    } else if (strcmp(key, "webpage") == 0) {
        return UPPMFormulaKeyCode_webpage;
    } else if (strcmp(key, "version") == 0) {
        return UPPMFormulaKeyCode_version;
    } else if (strcmp(key, "license") == 0) {
        return UPPMFormulaKeyCode_license;
    } else if (strcmp(key, "bin-url") == 0) {
        return UPPMFormulaKeyCode_bin_url;
    } else if (strcmp(key, "bin-sha") == 0) {
        return UPPMFormulaKeyCode_bin_sha;
    } else if (strcmp(key, "dep-pkg") == 0) {
        return UPPMFormulaKeyCode_dep_pkg;
    } else if (strcmp(key, "install") == 0) {
        return UPPMFormulaKeyCode_install;
    } else {
        if (isLinuxMuslLibc) {
                   if (strcmp(key, "res-url") == 0) {
                return UPPMFormulaKeyCode_bin_url;
            } else if (strcmp(key, "res-sha") == 0) {
                return UPPMFormulaKeyCode_bin_sha;
            }
        }

        return UPPMFormulaKeyCode_unknown;
    }
}

static int uppm_formula_set_value(UPPMFormulaKeyCode keyCode, char * value, UPPMFormula * formula) {
    if (keyCode == UPPMFormulaKeyCode_unknown) {
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
        case UPPMFormulaKeyCode_summary:
            if (formula->summary != NULL) {
                free(formula->summary);
            }

            formula->summary = strdup(value);

            if (formula->summary == NULL) {
                return UPPM_ERROR_MEMORY_ALLOCATION_FAILURE;
            } else {
                return UPPM_OK;
            }
        case UPPMFormulaKeyCode_version:
            if (formula->version != NULL) {
                free(formula->version);
            }

            formula->version = strdup(value);

            if (formula->version == NULL) {
                return UPPM_ERROR_MEMORY_ALLOCATION_FAILURE;
            } else {
                return UPPM_OK;
            }
        case UPPMFormulaKeyCode_license:
            if (formula->license != NULL) {
                free(formula->license);
            }

            formula->license = strdup(value);

            if (formula->license == NULL) {
                return UPPM_ERROR_MEMORY_ALLOCATION_FAILURE;
            } else {
                return UPPM_OK;
            }
        case UPPMFormulaKeyCode_webpage:
            if (formula->webpage != NULL) {
                free(formula->webpage);
            }

            formula->webpage = strdup(value);

            if (formula->webpage == NULL) {
                return UPPM_ERROR_MEMORY_ALLOCATION_FAILURE;
            } else {
                return UPPM_OK;
            }
        case UPPMFormulaKeyCode_bin_url:
            if (formula->bin_url != NULL) {
                free(formula->bin_url);
            }

            formula->bin_url = strdup(value);

            if (formula->bin_url == NULL) {
                return UPPM_ERROR_MEMORY_ALLOCATION_FAILURE;
            } else {
                return UPPM_OK;
            }
        case UPPMFormulaKeyCode_bin_sha:
            if (formula->bin_sha != NULL) {
                free(formula->bin_sha);
            }

            formula->bin_sha = strdup(value);

            if (formula->bin_sha == NULL) {
                return UPPM_ERROR_MEMORY_ALLOCATION_FAILURE;
            } else {
                return UPPM_OK;
            }
        case UPPMFormulaKeyCode_dep_pkg:
            if (formula->dep_pkg != NULL) {
                free(formula->dep_pkg);
            }

            formula->dep_pkg = strdup(value);

            if (formula->dep_pkg == NULL) {
                return UPPM_ERROR_MEMORY_ALLOCATION_FAILURE;
            } else {
                return UPPM_OK;
            }
        case UPPMFormulaKeyCode_install:
            if (formula->install != NULL) {
                free(formula->install);
            }

            formula->install = strdup(value);

            if (formula->install == NULL) {
                return UPPM_ERROR_MEMORY_ALLOCATION_FAILURE;
            } else {
                return UPPM_OK;
            }
        default: return UPPM_OK;
    }
}

static int uppm_formula_check(UPPMFormula * formula, const char * formulaFilePath) {
    if (formula->summary == NULL) {
        fprintf(stderr, "scheme error in formula file: %s : summary mapping not found.\n", formulaFilePath);
        return UPPM_FORMULA_SCHEME_ERROR;
    }

    if (formula->webpage == NULL) {
        fprintf(stderr, "scheme error in formula file: %s : webpage mapping not found.\n", formulaFilePath);
        return UPPM_FORMULA_SCHEME_ERROR;
    }

    if (formula->bin_url == NULL) {
        fprintf(stderr, "scheme error in formula file: %s : bin-url mapping not found.\n", formulaFilePath);
        return UPPM_FORMULA_SCHEME_ERROR;
    }

    if (formula->bin_sha == NULL) {
        fprintf(stderr, "scheme error in formula file: %s : bin-sha mapping not found.\n", formulaFilePath);
        return UPPM_FORMULA_SCHEME_ERROR;
    }

    if (strlen(formula->bin_sha) != 64) {
        fprintf(stderr, "scheme error in formula file: %s : bin-sha mapping's value's length must be 64.\n", formulaFilePath);
        return UPPM_FORMULA_SCHEME_ERROR;
    }

    if (formula->version == NULL) {
        size_t urlLength = strlen(formula->bin_url);
        size_t urlCopyLength = urlLength + 1;
        char   urlCopy[urlCopyLength];
        memset(urlCopy, 0, urlCopyLength);
        strncpy(urlCopy, formula->bin_url, urlLength);

        char * srcFileName = basename(urlCopy);

        size_t srcFileNameLength = 0;

        char c;

        for (;;) {
            c = srcFileName[srcFileNameLength];

            if (c == '\0') {
                break;
            }

            if (c == '_' || c == '@') {
                srcFileName[srcFileNameLength] = '-';
            }

            srcFileNameLength++;
        }

        if (srcFileNameLength > 8) {
                   if (strcmp(&srcFileName[srcFileNameLength - 8], ".tar.bz2") == 0) {
                srcFileName[srcFileNameLength - 8] = '\0';
                srcFileNameLength -= 8;
            } else if (strcmp(&srcFileName[srcFileNameLength - 7], ".tar.gz") == 0) {
                srcFileName[srcFileNameLength - 7] = '\0';
                srcFileNameLength -= 7;
            } else if (strcmp(&srcFileName[srcFileNameLength - 7], ".tar.xz") == 0) {
                srcFileName[srcFileNameLength - 7] = '\0';
                srcFileNameLength -= 7;
            } else if (strcmp(&srcFileName[srcFileNameLength - 7], ".tar.lz") == 0) {
                srcFileName[srcFileNameLength - 7] = '\0';
                srcFileNameLength -= 7;
            } else if (strcmp(&srcFileName[srcFileNameLength - 5], ".tbz2") == 0) {
                srcFileName[srcFileNameLength - 5] = '\0';
                srcFileNameLength -= 5;
            } else if (strcmp(&srcFileName[srcFileNameLength - 4], ".tgz") == 0) {
                srcFileName[srcFileNameLength - 4] = '\0';
                srcFileNameLength -= 4;
            } else if (strcmp(&srcFileName[srcFileNameLength - 4], ".txz") == 0) {
                srcFileName[srcFileNameLength - 4] = '\0';
                srcFileNameLength -= 4;
            } else if (strcmp(&srcFileName[srcFileNameLength - 4], ".tlz") == 0) {
                srcFileName[srcFileNameLength - 4] = '\0';
                srcFileNameLength -= 4;
            } else if (strcmp(&srcFileName[srcFileNameLength - 4], ".zip") == 0) {
                srcFileName[srcFileNameLength - 4] = '\0';
                srcFileNameLength -= 4;
            }
        } else if (srcFileNameLength > 7) {
                   if (strcmp(&srcFileName[srcFileNameLength - 7], ".tar.gz") == 0) {
                srcFileName[srcFileNameLength - 7] = '\0';
                srcFileNameLength -= 7;
            } else if (strcmp(&srcFileName[srcFileNameLength - 7], ".tar.xz") == 0) {
                srcFileName[srcFileNameLength - 7] = '\0';
                srcFileNameLength -= 7;
            } else if (strcmp(&srcFileName[srcFileNameLength - 7], ".tar.lz") == 0) {
                srcFileName[srcFileNameLength - 7] = '\0';
                srcFileNameLength -= 7;
            }
        } else if (srcFileNameLength > 5) {
                   if (strcmp(&srcFileName[srcFileNameLength - 5], ".tbz2") == 0) {
                srcFileName[srcFileNameLength - 5] = '\0';
                srcFileNameLength -= 5;
            }
        } else if (srcFileNameLength > 4) {
                   if (strcmp(&srcFileName[srcFileNameLength - 4], ".tgz") == 0) {
                srcFileName[srcFileNameLength - 4] = '\0';
                srcFileNameLength -= 4;
            } else if (strcmp(&srcFileName[srcFileNameLength - 4], ".txz") == 0) {
                srcFileName[srcFileNameLength - 4] = '\0';
                srcFileNameLength -= 4;
            } else if (strcmp(&srcFileName[srcFileNameLength - 4], ".tlz") == 0) {
                srcFileName[srcFileNameLength - 4] = '\0';
                srcFileNameLength -= 4;
            } else if (strcmp(&srcFileName[srcFileNameLength - 4], ".zip") == 0) {
                srcFileName[srcFileNameLength - 4] = '\0';
                srcFileNameLength -= 4;
            }
        }

        //printf("----------------srcFileName = %s\n", srcFileName);

        char * splitedStr = strtok(srcFileName, "-");

        while (splitedStr != NULL) {
            if (regex_matched(splitedStr, "^[0-9]+(\\.[0-9]+)+[a-z]?$")) {
                formula->version = strdup(splitedStr);

                if (formula->version == NULL) {
                    return UPPM_ERROR_MEMORY_ALLOCATION_FAILURE;
                } else {
                    return UPPM_OK;
                }

                break;
            } else if (regex_matched(splitedStr, "^[vV][0-9]+(\\.[0-9]+)+[a-z]?$")) {
                formula->version = strdup(&splitedStr[1]);

                if (formula->version == NULL) {
                    return UPPM_ERROR_MEMORY_ALLOCATION_FAILURE;
                } else {
                    return UPPM_OK;
                }

                break;
            } else if (regex_matched(splitedStr, "^[0-9]{3,8}$")) {
                formula->version = strdup(splitedStr);

                if (formula->version == NULL) {
                    return UPPM_ERROR_MEMORY_ALLOCATION_FAILURE;
                } else {
                    return UPPM_OK;
                }

                break;
            } else if (regex_matched(splitedStr, "^[vrR][0-9]{2,8}[a-z]?$")) {
                formula->version = strdup(&splitedStr[1]);

                if (formula->version == NULL) {
                    return UPPM_ERROR_MEMORY_ALLOCATION_FAILURE;
                } else {
                    return UPPM_OK;
                }

                break;
            }

            splitedStr = strtok(NULL, "-");
        }

        if (formula->version == NULL) {
            fprintf(stderr, "scheme error in formula file: %s : version mapping not found.\n", formulaFilePath);
            return UPPM_FORMULA_SCHEME_ERROR;
        }
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

    int resultCode = uppm_formula_find(packageName, &formulaFilePath);

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

    UPPMFormulaKeyCode formulaKeyCode = UPPMFormulaKeyCode_unknown;

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
                    formulaKeyCode = uppm_formula_key_code_from_key_name((char*)token.data.scalar.value, isLinuxMuslLibc);
                } else if (lastTokenType == 2) {
                    if (formula == NULL) {
                        formula = (UPPMFormula*)calloc(1, sizeof(UPPMFormula));

                        if (formula == NULL) {
                            resultCode = UPPM_ERROR_MEMORY_ALLOCATION_FAILURE;
                            goto clean;
                        }

                        formula->path = formulaFilePath;
                    }

                    resultCode = uppm_formula_set_value(formulaKeyCode, (char*)token.data.scalar.value, formula);

                    if (resultCode != UPPM_OK) {
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

    fclose(formulaFile);

    if (resultCode == UPPM_OK) {
        resultCode = uppm_formula_check(formula, formulaFilePath);
    }

    if (resultCode == UPPM_OK) {
        (*out) = formula;
        return UPPM_OK;
    }

    if (formula == NULL) {
        free(formulaFilePath);
    } else {
        uppm_formula_free(formula);
    }

    return resultCode;
}
