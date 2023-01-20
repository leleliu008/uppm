#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <curl/curl.h>

#include "uppm.h"

static int _my_realloc(char ** outP, size_t * outSize, size_t * outCapcity, size_t bufLength) {
    size_t  oldCapcity = (*outCapcity);
    size_t needCapcity = oldCapcity + bufLength + 1;
    size_t detaCapcity = needCapcity - oldCapcity;

    if (detaCapcity > 0) {
        size_t newCapcity = needCapcity + 256;

        char * p = (char*)realloc((*outP), newCapcity * sizeof(char));

        if (p == NULL) {
            free(*outP);
            return UPPM_ERROR_MEMORY_ALLOCATE;
        } else {
            (*outP) = p;
            (*outCapcity) = newCapcity;
            memset(&p[*outSize], 0, 256 + bufLength + 1);
        }
    }

    return UPPM_OK;
}

int uppm_depends_make_dot_items(const char * packageName, char ** outP, size_t * outSize, size_t * outCapcity) {
    UPPMFormula * formula = NULL;

    int ret = uppm_formula_lookup(packageName, &formula);

    if (ret != UPPM_OK) {
        return ret;
    }

    if (formula->dep_pkg == NULL) {
        uppm_formula_free(formula);
        return UPPM_OK;
    }

    size_t bufLength = strlen(packageName) + 12;
    char   buf[bufLength];
    snprintf(buf, bufLength, "    \"%s\" -> {", packageName);

    ret = _my_realloc(outP, outSize, outCapcity, bufLength);

    if (ret != UPPM_OK) {
        return ret;
    }

    strncat(*outP, buf, bufLength);

    (*outSize) += bufLength - 1;

    size_t depPackageNamesLength = strlen(formula->dep_pkg);

    size_t depPackageNamesCopyLength = depPackageNamesLength + 1;
    char   depPackageNamesCopy[depPackageNamesCopyLength];
    strncpy(depPackageNamesCopy, formula->dep_pkg, depPackageNamesCopyLength);

    char * depPackageName = strtok(depPackageNamesCopy, " ");

    while (depPackageName != NULL) {
        size_t  bufLength = strlen(depPackageName) + 4;
        char    buf[bufLength];
        snprintf(buf, bufLength, " \"%s\"", depPackageName);

        ret = _my_realloc(outP, outSize, outCapcity, bufLength);

        if (ret != UPPM_OK) {
            return ret;
        }

        strncat(*outP, buf, bufLength);

        (*outSize) += bufLength - 1;

        depPackageName = strtok (NULL, " ");
    }

    ret = _my_realloc(outP, outSize, outCapcity, 3);

    if (ret != UPPM_OK) {
        return ret;
    }

    char buff[3] = { ' ', '}', '\n' };
    strncat(*outP, buff, 3);

    (*outSize) += 3;

    ////////////////////////////////////////////////////////////////////////////

    size_t n = 0;

    char * ptr = NULL;

    char * p = formula->dep_pkg;

    for (;;) {
        if (p[0] <= 32) {
            if (n > 0) {
                size_t  depPackageNameLength = n + 1;
                char    depPackageName[depPackageNameLength];
                strncpy(depPackageName, ptr, n);

                depPackageName[n] = '\0';

                //printf("%lu\n", n);
                //printf("str=%s\n", str);

                ret = uppm_depends_make_dot_items(depPackageName, outP, outSize, outCapcity);

                if (ret != UPPM_OK) {
                    goto clean;
                }
            }

            if (p[0] == 0) {
                break;
            } else {
                ptr = NULL;
                n = 0;
                p++;
                continue;
            }
        } else {
            if (ptr == NULL) {
                ptr = p;
            }

            n++;
            p++;
        }
    }

clean:
    uppm_formula_free(formula);

    return ret;
}

static int uppm_depends_make_box(const char * dotScriptStr) {
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

    CURLcode curlcode = curl_easy_perform(curl);

    int ret = UPPM_OK;

    if (curlcode != CURLE_OK) {
        fprintf(stderr, "%s\n", curl_easy_strerror(curlcode));
        ret = UPPM_ERROR_NETWORK_BASE + curlcode;
    }

    curl_free(dataUrlEncoded);
    curl_easy_cleanup(curl);

    curl_global_cleanup();

    return ret;
}

static int uppm_depends_make_xxx(const char * dotScriptStr, size_t len, const char * type) {
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

    ////////////////////////////////////////////////////////////////////////////////////////////

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

    execlp("dot", "dot", type, filepath, NULL);

    perror("dot");

    return UPPM_ERROR;
}

int uppm_depends(const char * packageName, UPPMDependsOutputFormat outputFormat) {
    int ret = uppm_check_if_the_given_argument_matches_package_name_pattern(packageName);

    if (ret != UPPM_OK) {
        return ret;
    }

    char * p = NULL;
    size_t pSize = 0;
    size_t pCapcity = 0;

    ret = uppm_depends_make_dot_items(packageName, &p, &pSize, &pCapcity);

    if (ret != UPPM_OK) {
        return ret;
    }

    if (pSize == 0) {
        return UPPM_OK;
    }

           if (outputFormat == UPPMDependsOutputFormat_DOT) {
        printf("digraph G {\n%s}\n", p);

        free(p);

        return UPPM_OK;
    } else if (outputFormat == UPPMDependsOutputFormat_BOX) {
        size_t dotScriptStrLength = pSize + 14;
        char   dotScriptStr[dotScriptStrLength];
        snprintf(dotScriptStr, dotScriptStrLength, "digraph G {\n%s}", p);

        free(p);

        return uppm_depends_make_box(dotScriptStr);
    } else if (outputFormat == UPPMDependsOutputFormat_PNG) {
        size_t dotScriptStrLength = pSize + 14;
        char   dotScriptStr[dotScriptStrLength];
        snprintf(dotScriptStr, dotScriptStrLength, "digraph G {\n%s}", p);

        free(p);

        return uppm_depends_make_xxx(dotScriptStr, dotScriptStrLength - 1, "-Tpng");
    } else if (outputFormat == UPPMDependsOutputFormat_SVG) {
        size_t dotScriptStrLength = pSize + 14;
        char   dotScriptStr[dotScriptStrLength];
        snprintf(dotScriptStr, dotScriptStrLength, "digraph G {\n%s}", p);

        free(p);

        return uppm_depends_make_xxx(dotScriptStr, dotScriptStrLength - 1, "-Tsvg");
    } else {
        free(p);

        return UPPM_ERROR;
    }
}
