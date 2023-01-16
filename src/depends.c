#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <curl/curl.h>

#include "uppm.h"

int uppm_depends_make_dot_items(const char * packageName, char buff[256]) {
    UPPMFormula * formula = NULL;

    int resultCode = uppm_formula_parse(packageName, &formula);

    if (resultCode != UPPM_OK) {
        return resultCode;
    }

    if (formula->dep_pkg == NULL) {
        uppm_formula_free(formula);
        return UPPM_OK;
    }

    strcat(buff, "\"");
    strcat(buff, packageName);
    strcat(buff, "\"");
    strcat(buff, " -> { ");

    size_t depPackageNamesLength = strlen(formula->dep_pkg);
    size_t depPackageNamesCopyLength = depPackageNamesLength + 1;
    char   depPackageNamesCopy[depPackageNamesCopyLength];
    memset(depPackageNamesCopy, 0, depPackageNamesCopyLength);
    strncpy(depPackageNamesCopy, formula->dep_pkg, depPackageNamesLength);

    char * depPackageName = strtok(depPackageNamesCopy, " ");

    while (depPackageName != NULL) {
        strcat(buff, "\"");
        strcat(buff, depPackageName);
        strcat(buff, "\" ");

        depPackageName = strtok (NULL, " ");
    }

    strcat(buff, "}\n");

    ////////////////////////////////////////////////////////////////////////////

    size_t n = 0;

    char * ptr = NULL;

    char * p = formula->dep_pkg;

    for (;;) {
        if (p[0] <= 32) {
            if (n > 0) {
                size_t  depPackageNameLength = n + 1;
                char    depPackageName[depPackageNameLength];
                memset( depPackageName, 0, depPackageNameLength);
                strncpy(depPackageName, ptr, n);

                //printf("%lu\n", n);
                //printf("str=%s\n", str);

                resultCode = uppm_depends_make_dot_items(depPackageName, buff);

                if (resultCode != UPPM_OK) {
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

    return resultCode;
}

static int uppm_depends_make_box(const char * dotScriptStr) {
    curl_global_init(CURL_GLOBAL_ALL);

    CURL * curl = curl_easy_init();

    char * dataUrlEncoded = curl_easy_escape(curl, dotScriptStr, strlen(dotScriptStr));

    size_t  urlLength = strlen(dataUrlEncoded) + 66;
    char    url[urlLength];
    memset (url, 0, urlLength);
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

    int resultCode = UPPM_OK;

    if (curlcode != CURLE_OK) {
        fprintf(stderr, "%s\n", curl_easy_strerror(curlcode));
        resultCode = UPPM_NETWORK_ERROR;
    }

    curl_free(dataUrlEncoded);
    curl_easy_cleanup(curl);

    curl_global_cleanup();

    return resultCode;
}

static int uppm_depends_make_png(const char * dotScriptStr) {
    size_t  cmdLength = strlen(dotScriptStr) + 22;
    char    cmd[cmdLength];
    memset( cmd, 0, cmdLength);
    snprintf(cmd, cmdLength, "dot -Tpng <<EOF\n%s\nEOF", dotScriptStr);

    if (system(cmd) == 0) {
        return UPPM_OK;
    } else {
        fprintf(stderr, "command not found: dot\n");
        return UPPM_ERROR;
    }
}

static int uppm_depends_make_svg(const char * dotScriptStr) {
    size_t  cmdLength = strlen(dotScriptStr) + 22;
    char    cmd[cmdLength];
    memset( cmd, 0, cmdLength);
    snprintf(cmd, cmdLength, "dot -Tsvg <<EOF\n%s\nEOF", dotScriptStr);

    if (system(cmd) == 0) {
        return UPPM_OK;
    } else {
        fprintf(stderr, "command not found: dot\n");
        return UPPM_ERROR;
    }
}

int uppm_depends(const char * packageName, UPPMDependsOutputFormat outputFormat) {
    int resultCode = uppm_check_if_the_given_argument_matches_package_name_pattern(packageName);

    if (resultCode != UPPM_OK) {
        return resultCode;
    }

    char buff[256] = {0};

    resultCode = uppm_depends_make_dot_items(packageName, buff);

    if (resultCode != UPPM_OK) {
        return resultCode;
    }

    size_t n = strlen(buff);

    if (n == 0) {
        return UPPM_OK;
    }

           if (outputFormat == UPPMDependsOutputFormat_DOT) {
        printf("digraph G {\n%s}\n", buff);
        return UPPM_OK;
    } else if (outputFormat == UPPMDependsOutputFormat_BOX) {
        size_t  dotScriptStrLength = n + 15;
        char    dotScriptStr[dotScriptStrLength];
        memset( dotScriptStr, 0, dotScriptStrLength);
        snprintf(dotScriptStr, dotScriptStrLength, "digraph G {\n%s}", buff);

        return uppm_depends_make_box(dotScriptStr);
    } else if (outputFormat == UPPMDependsOutputFormat_PNG) {
        size_t  dotScriptStrLength = n + 15;
        char    dotScriptStr[dotScriptStrLength];
        memset( dotScriptStr, 0, dotScriptStrLength);
        snprintf(dotScriptStr, dotScriptStrLength, "digraph G {\n%s}", buff);

        return uppm_depends_make_png(dotScriptStr);
    } else if (outputFormat == UPPMDependsOutputFormat_SVG) {
        size_t  dotScriptStrLength = n + 15;
        char    dotScriptStr[dotScriptStrLength];
        memset( dotScriptStr, 0, dotScriptStrLength);
        snprintf(dotScriptStr, dotScriptStrLength, "digraph G {\n%s}", buff);

        return uppm_depends_make_svg(dotScriptStr);
    } else {
        return UPPM_ERROR;
    }
}
