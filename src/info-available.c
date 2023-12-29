#include <time.h>
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

int uppm_available_info(const char * packageName, const char * key) {
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

        FILE * formulaFile = fopen(formulaFilePath, "r");

        if (formulaFile == NULL) {
            perror(formulaFilePath);
            free(formulaFilePath);
            return UPPM_ERROR;
        }

        if (isatty(STDOUT_FILENO)) {
            printf("pkgname: %s%s%s\n", COLOR_GREEN, packageName, COLOR_OFF);
        } else {
            printf("pkgname: %s\n", packageName);
        }

        char   buff[1024];
        size_t size;

        for (;;) {
            size = fread(buff, 1, 1024, formulaFile);

            if (ferror(formulaFile)) {
                perror(formulaFilePath);
                fclose(formulaFile);
                free(formulaFilePath);
                return UPPM_ERROR;
            }

            if (size > 0) {
                if (fwrite(buff, 1, size, stdout) != size || ferror(stdout)) {
                    perror(NULL);
                    fclose(formulaFile);
                    free(formulaFilePath);
                    return UPPM_ERROR;
                }
            }

            if (feof(formulaFile)) {
                fclose(formulaFile);
                break;
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
        json_object_set_new(root, "unpackd", json_string(formula->unpackd));
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
    } else if (strcmp(key, "formula") == 0) {
        char * formulaFilePath = NULL;

        ret = uppm_formula_locate(packageName, &formulaFilePath);

        if (ret != UPPM_OK) {
            return ret;
        }

        printf("%s\n", formulaFilePath);
        free(formulaFilePath);
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

        char fileNameExtension[21] = {0};

        ret = uppm_examine_filetype_from_url(formula->bin_url, fileNameExtension, 20);

        if (ret != UPPM_OK) {
            uppm_formula_free(formula);
            return ret;
        }

        printf("%s\n", fileNameExtension);

        uppm_formula_free(formula);
    } else if (strcmp(key, "src-fp") == 0) {
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

        char fileNameExtension[21] = {0};

        ret = uppm_examine_filetype_from_url(formula->bin_url, fileNameExtension, 20);

        if (ret != UPPM_OK) {
            uppm_formula_free(formula);
            return ret;
        }

        printf("%s/downloads/%s%s\n", uppmHomeDIR, formula->bin_sha, fileNameExtension);

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
    } else if (strcmp(key, "unpackd") == 0) {
        UPPMFormula * formula = NULL;

        ret = uppm_formula_lookup(packageName, &formula);

        if (ret != UPPM_OK) {
            return ret;
        }

        if (formula->unpackd != NULL) {
            printf("%s\n", formula->unpackd);
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
    } else {
        return UPPM_ERROR_ARG_IS_UNKNOWN;
    }

    return ret;
}
