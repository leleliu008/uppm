#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <curl/curl.h>

#include "uppm.h"

static size_t write_callback(void * ptr, size_t size, size_t nmemb, void * stream) {
    return fwrite(ptr, size, nmemb, (FILE *)stream);
}

static int uppm_depends_make_box(const char * dotScriptStr, const char * outputFilePath) {
    curl_global_init(CURL_GLOBAL_ALL);

    CURL * curl = curl_easy_init();

    char * dataUrlEncoded = curl_easy_escape(curl, dotScriptStr, strlen(dotScriptStr));

    size_t urlLength = strlen(dataUrlEncoded) + 66;
    char   url[urlLength];
    snprintf(url, urlLength, "https://dot-to-ascii.ggerganov.com/dot-to-ascii.php?boxart=1&src=%s", dataUrlEncoded);

    //printf("url=%s\n", url);

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);

    bool verbose = false;
    bool showProgress = false;

    // https://curl.se/libcurl/c/CURLOPT_VERBOSE.html
    if (verbose) {
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
    } else {
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 0);
    }

    // https://curl.se/libcurl/c/CURLOPT_NOPROGRESS.html
    if (showProgress) {
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
    } else {
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
    }

    CURLcode curlcode = CURLE_OK;

    int ret = UPPM_OK;

    if (outputFilePath != NULL) {
        FILE * outputFile = fopen(outputFilePath, "wb");

        if (outputFile == NULL) {
            perror(outputFilePath);
            ret = UPPM_ERROR;
            goto clean;
        }

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, outputFile);
    }

    curlcode = curl_easy_perform(curl);

clean:

    if (curlcode != CURLE_OK) {
        fprintf(stderr, "%s\n", curl_easy_strerror(curlcode));
        ret = UPPM_ERROR_NETWORK_BASE + curlcode;
    }

    curl_free(dataUrlEncoded);
    curl_easy_cleanup(curl);

    curl_global_cleanup();

    return ret;
}

static int uppm_depends_make_xxx(const char * dotScriptStr, size_t len, const char * tOption, const char * oOption) {
    char * userHomeDir = getenv("HOME");

    if (userHomeDir == NULL) {
        return UPPM_ERROR_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    if (userHomeDirLength == 0) {
        return UPPM_ERROR_ENV_HOME_NOT_SET;
    }

    ////////////////////////////////////////////////////////////////

    struct stat st;

    size_t uppmHomeDirLength = userHomeDirLength + 7;
    char   uppmHomeDir[uppmHomeDirLength];
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

    size_t uppmTmpDirLength = uppmHomeDirLength + 5;
    char   uppmTmpDir[uppmTmpDirLength];
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
    size_t needCapcity = oldCapcity + bufLength + 1;

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
                goto finally;
            }

            if (packageSetSize == packageSetCapcity) {
                UPPMPackage ** p = (UPPMPackage**)realloc(packageSet, (packageSetCapcity + 10) * sizeof(UPPMPackage*));

                if (p == NULL) {
                    free(packageName);
                    uppm_formula_free(formula);
                    ret = UPPM_ERROR_MEMORY_ALLOCATE;
                    goto finally;
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
                goto finally;
            }

            package->formula = formula;
            package->packageName = strdup(packageName);

            packageSet[packageSetSize] = package;
            packageSetSize++;

            if (package->packageName == NULL) {
                free(packageName);
                ret = UPPM_ERROR_MEMORY_ALLOCATE;
                goto finally;
            }

            free(packageName);
            packageName = package->packageName;
        }

        if (formula->dep_pkg == NULL) {
            continue;
        }

        ////////////////////////////////////////////////////////////////

        size_t bufLength = strlen(packageName) + 12;
        char   buf[bufLength];
        snprintf(buf, bufLength, "    \"%s\" -> {", packageName);

        ret = string_append(&p, &pSize, &pCapcity, buf, bufLength - 1);

        if (ret != UPPM_OK) {
            goto finally;
        }

        ////////////////////////////////////////////////////////////////

        size_t depPackageNamesLength = strlen(formula->dep_pkg);

        size_t depPackageNamesCopyLength = depPackageNamesLength + 1;
        char   depPackageNamesCopy[depPackageNamesCopyLength];
        strncpy(depPackageNamesCopy, formula->dep_pkg, depPackageNamesCopyLength);

        char * depPackageName = strtok(depPackageNamesCopy, " ");

        while (depPackageName != NULL) {
            if (strcmp(depPackageName, packageName) == 0) {
                fprintf(stderr, "package '%s' depends itself.\n", packageName);
                ret = UPPM_ERROR;
                goto finally;
            }

            ////////////////////////////////////////////////////////////////

            size_t  bufLength = strlen(depPackageName) + 4;
            char    buf[bufLength];
            snprintf(buf, bufLength, " \"%s\"", depPackageName);

            ret = string_append(&p, &pSize, &pCapcity, buf, bufLength - 1);

            if (ret != UPPM_OK) {
                goto finally;
            }

            ////////////////////////////////////////////////////////////////

            ////////////////////////////////////////////////////////////////

            if (packageNameStackSize == packageNameStackCapcity) {
                char ** p = (char**)realloc(packageNameStack, (packageNameStackCapcity + 10) * sizeof(char*));

                if (p == NULL) {
                    ret = UPPM_ERROR_MEMORY_ALLOCATE;
                    goto finally;
                }

                memset(p + packageNameStackCapcity, 0, 10);

                packageNameStack = p;
                packageNameStackCapcity += 10;
            }

            char * p = strdup(depPackageName);

            if (p == NULL) {
                ret = UPPM_ERROR_MEMORY_ALLOCATE;
                goto finally;
            }

            packageNameStack[packageNameStackSize] = p;
            packageNameStackSize++;

            depPackageName = strtok (NULL, " ");
        }

        ret = string_append(&p, &pSize, &pCapcity, " }\n", 3);
    }

finally:
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
        size_t dotScriptStrLength = pSize + 14;
        char   dotScriptStr[dotScriptStrLength];
        snprintf(dotScriptStr, dotScriptStrLength, "digraph G {\n%s}", p);

        free(p);

        return uppm_depends_make_box(dotScriptStr, outputFilePath);
    } else if (outputType == UPPMDependsOutputType_PNG) {
        size_t dotScriptStrLength = pSize + 14;
        char   dotScriptStr[dotScriptStrLength];
        snprintf(dotScriptStr, dotScriptStrLength, "digraph G {\n%s}", p);

        free(p);

        if (outputFilePath == NULL) {
            return uppm_depends_make_xxx(dotScriptStr, dotScriptStrLength - 1, "-Tpng", NULL);
        } else {
            size_t oOptionLength=strlen(outputFilePath) + 3;
            char   oOption[oOptionLength];
            snprintf(oOption, oOptionLength, "-o%s", outputFilePath);

            return uppm_depends_make_xxx(dotScriptStr, dotScriptStrLength - 1, "-Tpng", oOption);
        }
    } else if (outputType == UPPMDependsOutputType_SVG) {
        size_t dotScriptStrLength = pSize + 14;
        char   dotScriptStr[dotScriptStrLength];
        snprintf(dotScriptStr, dotScriptStrLength, "digraph G {\n%s}", p);

        free(p);

        if (outputFilePath == NULL) {
            return uppm_depends_make_xxx(dotScriptStr, dotScriptStrLength - 1, "-Tsvg", NULL);
        } else {
            size_t oOptionLength=strlen(outputFilePath) + 3;
            char   oOption[oOptionLength];
            snprintf(oOption, oOptionLength, "-o%s", outputFilePath);

            return uppm_depends_make_xxx(dotScriptStr, dotScriptStrLength - 1, "-Tsvg", oOption);
        }
    } else {
        free(p);

        return UPPM_ERROR;
    }
}
