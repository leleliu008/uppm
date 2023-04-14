#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>

#include "core/http.h"
#include "core/url.h"

#include "uppm.h"

static int uppm_depends_make_box(const char * dotScriptStr, size_t dotScriptStrLength, const char * outputFilePath) {
    size_t urlEncodedBufLength = 3 * dotScriptStrLength + 1U;
    char   urlEncodedBuf[urlEncodedBufLength];
    memset(urlEncodedBuf, 0, urlEncodedBufLength);

    size_t urlEncodedRealLength = 0;

    if (url_encode(urlEncodedBuf, &urlEncodedRealLength, (unsigned char *)dotScriptStr, dotScriptStrLength, true) != 0) {
        perror(NULL);
        return UPPM_ERROR;
    }

    size_t   urlLength = urlEncodedRealLength + 66U;
    char     url[urlLength];
    snprintf(url, urlLength, "https://dot-to-ascii.ggerganov.com/dot-to-ascii.php?boxart=1&src=%s", urlEncodedBuf);

    //printf("url=%s\n", url);

    int ret;

    if (outputFilePath == NULL) {
        ret = http_fetch_to_stream(url, stdout, false, false);
    } else {
        ret = http_fetch_to_file(url, outputFilePath, false, false);
    }

    if (ret == -1) {
        perror(outputFilePath);
        return UPPM_ERROR;
    }

    if (ret > 0) {
        return UPPM_ERROR_NETWORK_BASE + ret;
    }

    return UPPM_OK;
}

static int uppm_depends_make_xxx(const char * dotScriptStr, size_t len, const char * tOption, const char * oOption) {
    const char * const userHomeDir = getenv("HOME");

    if (userHomeDir == NULL) {
        return UPPM_ERROR_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    if (userHomeDirLength == 0U) {
        return UPPM_ERROR_ENV_HOME_NOT_SET;
    }

    ////////////////////////////////////////////////////////////////

    struct stat st;

    size_t   uppmHomeDirLength = userHomeDirLength + 7U;
    char     uppmHomeDir[uppmHomeDirLength];
    snprintf(uppmHomeDir, uppmHomeDirLength, "%s/.uppm", userHomeDir);

    if (stat(uppmHomeDir, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            fprintf(stderr, "'%s\n' was expected to be a directory, but it was not.\n", uppmHomeDir);
            return UPPM_ERROR;
        }
    } else {
        if (mkdir(uppmHomeDir, S_IRWXU) != 0) {
            perror(uppmHomeDir);
            return UPPM_ERROR;
        }
    }

    ////////////////////////////////////////////////////////////////

    size_t   uppmTmpDirLength = uppmHomeDirLength + 5U;
    char     uppmTmpDir[uppmTmpDirLength];
    snprintf(uppmTmpDir, uppmTmpDirLength, "%s/tmp", uppmHomeDir);

    if (stat(uppmTmpDir, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            fprintf(stderr, "'%s\n' was expected to be a directory, but it was not.\n", uppmTmpDir);
            return UPPM_ERROR;
        }
    } else {
        if (mkdir(uppmTmpDir, S_IRWXU) != 0) {
            perror(uppmTmpDir);
            return UPPM_ERROR;
        }
    }

    ////////////////////////////////////////////////////////////////

    char ts[11] = {0};
    snprintf(ts, 11, "%ld", time(NULL));

    size_t filepathLength = uppmTmpDirLength + strlen(ts) + 6;
    char   filepath[filepathLength];
    snprintf(filepath, filepathLength, "%s/%s.dot", uppmTmpDir, ts);

    FILE * file = fopen(filepath, "w");

    if (file == NULL) {
        perror(NULL);
        return UPPM_ERROR;
    }

    if (fwrite(dotScriptStr, 1, len, file) != len || ferror(file)) {
        perror(filepath);
        fclose(file);
        return UPPM_ERROR;
    }

    fclose(file);

    if (oOption == NULL) {
        execlp("dot", "dot", tOption,          filepath, NULL);
    } else {
        execlp("dot", "dot", tOption, oOption, filepath, NULL);
    }

    perror("dot");

    return UPPM_ERROR;
}

static int string_append(char ** outP, size_t * outSize, size_t * outCapcity, const char * buf, size_t bufLength) {
    size_t  oldCapcity = (*outCapcity);
    size_t needCapcity = oldCapcity + bufLength + 1U;

    if (needCapcity > oldCapcity) {
        size_t newCapcity = needCapcity + 256;

        char * p = (char*)realloc(*outP, newCapcity * sizeof(char));

        if (p == NULL) {
            free(*outP);
            return UPPM_ERROR_MEMORY_ALLOCATE;
        } else {
            (*outP) = p;
            (*outCapcity) = newCapcity;
            memset(&p[*outSize], 0, 256 + bufLength + 1);
        }
    }

    strncat(*outP, buf, bufLength);
        
    (*outSize) += bufLength;

    return UPPM_OK;
}

typedef struct {
    char * packageName;
    UPPMFormula * formula;
} UPPMPackage;

typedef struct {
    char * nodeList;
    size_t nodeListSize;
    size_t nodeListCapcity;
} DirectedPath;

int uppm_depends(const char * packageName, UPPMDependsOutputType outputType, const char * outputFilePath) {
    int ret = UPPM_OK;

    ////////////////////////////////////////////////////////////////

    char * p = NULL;
    size_t pSize = 0;
    size_t pCapcity = 0;

    ////////////////////////////////////////////////////////////////

    size_t          directedPathArrayListCapcity = 0;
    size_t          directedPathArrayListSize    = 0;
    DirectedPath ** directedPathArrayList        = NULL;

    ////////////////////////////////////////////////////////////////

    size_t         packageSetCapcity = 0;
    size_t         packageSetSize    = 0;
    UPPMPackage ** packageSet        = NULL;

    ////////////////////////////////////////////////////////////////

    size_t   packageNameStackCapcity = 1;
    size_t   packageNameStackSize    = 0;
    char * * packageNameStack = (char**)calloc(1, sizeof(char*));

    if (packageNameStack == NULL) {
        return UPPM_ERROR_MEMORY_ALLOCATE;
    }

    packageNameStack[0] = strdup(packageName);

    if (packageNameStack[0] == NULL) {
        free(packageNameStack);
        return UPPM_ERROR_MEMORY_ALLOCATE;
    }

    packageNameStackSize = 1;

    ////////////////////////////////////////////////////////////////

    while (packageNameStackSize > 0) {
        char * packageName = packageNameStack[packageNameStackSize - 1];
        packageNameStack[packageNameStackSize - 1] = NULL;
        packageNameStackSize--;

        UPPMFormula * formula = NULL;

        for (size_t i = 0; i < packageSetSize; i++) {
            if (strcmp(packageSet[i]->packageName, packageName) == 0) {
                formula = packageSet[i]->formula;
                free(packageName);
                packageName = packageSet[i]->packageName;
                break;
            }
        }

        if (formula == NULL) {
            ret = uppm_formula_lookup(packageName, &formula);

            if (ret != UPPM_OK) {
                free(packageName);
                goto finalize;
            }

            if (packageSetSize == packageSetCapcity) {
                UPPMPackage ** p = (UPPMPackage**)realloc(packageSet, (packageSetCapcity + 10) * sizeof(UPPMPackage*));

                if (p == NULL) {
                    free(packageName);
                    uppm_formula_free(formula);
                    ret = UPPM_ERROR_MEMORY_ALLOCATE;
                    goto finalize;
                }

                memset(p + packageSetCapcity, 0, 10);

                packageSet = p;
                packageSetCapcity += 10;
            }

            UPPMPackage * package = (UPPMPackage*)malloc(sizeof(UPPMPackage));

            if (package == NULL) {
                free(packageName);
                uppm_formula_free(formula);
                ret = UPPM_ERROR_MEMORY_ALLOCATE;
                goto finalize;
            }

            package->formula = formula;
            package->packageName = strdup(packageName);

            packageSet[packageSetSize] = package;
            packageSetSize++;

            if (package->packageName == NULL) {
                free(packageName);
                ret = UPPM_ERROR_MEMORY_ALLOCATE;
                goto finalize;
            }

            free(packageName);
            packageName = package->packageName;
        }

        if (formula->dep_pkg == NULL) {
            continue;
        }

        ////////////////////////////////////////////////////////////////

        size_t   bufLength = strlen(packageName) + 12;
        char     buf[bufLength];
        snprintf(buf, bufLength, "    \"%s\" -> {", packageName);

        ret = string_append(&p, &pSize, &pCapcity, buf, bufLength - 1);

        if (ret != UPPM_OK) {
            goto finalize;
        }

        ////////////////////////////////////////////////////////////////

        size_t depPackageNamesLength = strlen(formula->dep_pkg);

        size_t depPackageNamesCopyLength = depPackageNamesLength + 1U;
        char   depPackageNamesCopy[depPackageNamesCopyLength];
        strncpy(depPackageNamesCopy, formula->dep_pkg, depPackageNamesCopyLength);

        char * depPackageName = strtok(depPackageNamesCopy, " ");

        while (depPackageName != NULL) {
            if (strcmp(depPackageName, packageName) == 0) {
                fprintf(stderr, "package '%s' depends itself.\n", packageName);
                ret = UPPM_ERROR;
                goto finalize;
            }

            ////////////////////////////////////////////////////////////////

            size_t  bufLength = strlen(depPackageName) + 4;
            char    buf[bufLength];
            snprintf(buf, bufLength, " \"%s\"", depPackageName);

            ret = string_append(&p, &pSize, &pCapcity, buf, bufLength - 1);

            if (ret != UPPM_OK) {
                goto finalize;
            }

            ////////////////////////////////////////////////////////////////

            ////////////////////////////////////////////////////////////////

            if (packageNameStackSize == packageNameStackCapcity) {
                char ** p = (char**)realloc(packageNameStack, (packageNameStackCapcity + 10) * sizeof(char*));

                if (p == NULL) {
                    ret = UPPM_ERROR_MEMORY_ALLOCATE;
                    goto finalize;
                }

                memset(p + packageNameStackCapcity, 0, 10);

                packageNameStack = p;
                packageNameStackCapcity += 10;
            }

            char * p = strdup(depPackageName);

            if (p == NULL) {
                ret = UPPM_ERROR_MEMORY_ALLOCATE;
                goto finalize;
            }

            packageNameStack[packageNameStackSize] = p;
            packageNameStackSize++;

            depPackageName = strtok (NULL, " ");
        }

        ret = string_append(&p, &pSize, &pCapcity, " }\n", 3);
    }

finalize:
    for (size_t i = 0; i < packageNameStackSize; i++) {
        free(packageNameStack[i]);
        packageNameStack[i] = NULL;
    }

    free(packageNameStack);
    packageNameStack = NULL;

    //////////////////////////////////////////////////

    for (size_t i = 0; i < packageSetSize; i++) {
        free(packageSet[i]->packageName);
        uppm_formula_free(packageSet[i]->formula);

        packageSet[i]->formula = NULL;
        packageSet[i]->packageName = NULL;

        free(packageSet[i]);
        packageSet[i] = NULL;
    }

    free(packageSet);
    packageSet = NULL;

    //////////////////////////////////////////////////

    if (ret != UPPM_OK) {
        free(p);
        return ret;
    }

    ////////////////////////////////////////////////////////////////

    if (pSize == 0) {
        return UPPM_OK;
    }

    ////////////////////////////////////////////////////////////////

    if (outputFilePath != NULL && strcmp(outputFilePath, "") == 0) {
        outputFilePath = NULL;
    }

    ////////////////////////////////////////////////////////////////

    if (outputType == UPPMDependsOutputType_DOT) {
        if (outputFilePath == NULL) {
            printf("digraph G {\n%s}\n", p);
            free(p);
            return UPPM_OK;
        } else {
            FILE * outputFile = fopen(outputFilePath, "wb");

            if (outputFile == NULL) {
                perror(outputFilePath);
                free(p);
                return UPPM_ERROR;
            }

            fprintf(outputFile, "digraph G {\n%s}\n", p);

            free(p);

            if (ferror(outputFile)) {
                perror(outputFilePath);
                fclose(outputFile);
                return UPPM_ERROR;
            } else {
                fclose(outputFile);
                return UPPM_OK;
            }
        }
    } else if (outputType == UPPMDependsOutputType_BOX) {
        size_t   dotScriptStrLength = pSize + 14;
        char     dotScriptStr[dotScriptStrLength];
        snprintf(dotScriptStr, dotScriptStrLength, "digraph G {\n%s}", p);

        free(p);

        return uppm_depends_make_box(dotScriptStr, dotScriptStrLength - 1, outputFilePath);
    } else if (outputType == UPPMDependsOutputType_PNG) {
        size_t   dotScriptStrLength = pSize + 14;
        char     dotScriptStr[dotScriptStrLength];
        snprintf(dotScriptStr, dotScriptStrLength, "digraph G {\n%s}", p);

        free(p);

        if (outputFilePath == NULL) {
            return uppm_depends_make_xxx(dotScriptStr, dotScriptStrLength - 1, "-Tpng", NULL);
        } else {
            size_t   oOptionLength=strlen(outputFilePath) + 3;
            char     oOption[oOptionLength];
            snprintf(oOption, oOptionLength, "-o%s", outputFilePath);

            return uppm_depends_make_xxx(dotScriptStr, dotScriptStrLength - 1, "-Tpng", oOption);
        }
    } else if (outputType == UPPMDependsOutputType_SVG) {
        size_t   dotScriptStrLength = pSize + 14;
        char     dotScriptStr[dotScriptStrLength];
        snprintf(dotScriptStr, dotScriptStrLength, "digraph G {\n%s}", p);

        free(p);

        if (outputFilePath == NULL) {
            return uppm_depends_make_xxx(dotScriptStr, dotScriptStrLength - 1, "-Tsvg", NULL);
        } else {
            size_t   oOptionLength=strlen(outputFilePath) + 3;
            char     oOption[oOptionLength];
            snprintf(oOption, oOptionLength, "-o%s", outputFilePath);

            return uppm_depends_make_xxx(dotScriptStr, dotScriptStrLength - 1, "-Tsvg", oOption);
        }
    } else {
        free(p);

        return UPPM_ERROR;
    }
}
