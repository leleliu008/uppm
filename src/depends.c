#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <curl/curl.h>

#include "uppm.h"

int uppm_depends_internal(const char * packageName, char buff[256]) {
    UPPMFormula * formula = NULL;

    int resultCode = uppm_formula_parse(packageName, &formula);

    if (resultCode != UPPM_OK) {
        return resultCode;
    }

    if ((formula->dep_pkg == NULL) || (strcmp(formula->dep_pkg, "") == 0)) {
        uppm_formula_free(formula);
        return UPPM_OK;
    }

    strcat(buff, "\"");
    strcat(buff, packageName);
    strcat(buff, "\"");
    strcat(buff, " -> { ");

    size_t index = 0;
    char * depList[10];

    char * depPackageName = strtok(strdup(formula->dep_pkg), " ");

    while (depPackageName != NULL) {
        strcat(buff, "\"");
        strcat(buff, depPackageName);
        strcat(buff, "\" ");

        depList[index] = depPackageName;
        index++;
        depPackageName = strtok (NULL, " ");
    }

    strcat(buff, "}\n");

    for (size_t i = 0; i < index; i++) {
        resultCode = uppm_depends_internal(depList[i], buff);

        if (resultCode != UPPM_OK) {
            uppm_formula_free(formula);
            return resultCode;
        }
    }

    uppm_formula_free(formula);
    return UPPM_OK;
}

int uppm_depends(const char * packageName) {
    int resultCode = uppm_is_package_name(packageName);

    if (resultCode != UPPM_OK) {
        return resultCode;
    }

    char buff[256] = {0};

    strcpy(buff, "digraph G {\n");

    resultCode = uppm_depends_internal(packageName, buff);

    if (resultCode == UPPM_OK) {
        strcat(buff, "}");
        //printf("%s\n", buff);
    } else {
        return resultCode;
    }

    ///////////////////////////////////////////////////////////////////////

    curl_global_init(CURL_GLOBAL_ALL);

    CURL * curl = curl_easy_init();

    char * dataUrlEncoded = curl_easy_escape(curl, buff, strlen(buff));

    size_t  urlLength = strlen(buff) + 1;
    char    url[urlLength];
    memset (url, 0, urlLength);
    sprintf(url, "https://dot-to-ascii.ggerganov.com/dot-to-ascii.php?boxart=1&src=%s", dataUrlEncoded);

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
    
    if (curlcode != CURLE_OK) {
        fprintf(stderr, "%s\n", curl_easy_strerror(curlcode));
        resultCode = UPPM_NETWORK_ERROR;
    }

    curl_free(dataUrlEncoded);
    curl_easy_cleanup(curl);

    curl_global_cleanup();

    return UPPM_OK;
}
