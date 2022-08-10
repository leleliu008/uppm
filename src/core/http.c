#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>
#include <stdbool.h>
#include <curl/curl.h>
#include "http.h"

static size_t write_callback(void * ptr, size_t size, size_t nmemb, void * stream) {
    return fwrite(ptr, size, nmemb, (FILE *)stream);
}

int http_fetch_to_stream(const char * url, FILE * outputFile, bool verbose, bool showProgress) {
    if (outputFile == NULL) {
        size_t  urlLength = strlen(url);
        size_t  urlCopyLength = urlLength + 1;
        char    urlCopy[urlCopyLength];
        memset (urlCopy, 0, urlCopyLength);
        strncpy(urlCopy, url, urlLength);

        const char * filename = basename(urlCopy);

        outputFile = fopen(filename, "wb");

        if (outputFile == NULL) {
            perror(filename);
            return 1;
        }
    }

    curl_global_init(CURL_GLOBAL_ALL);

    CURL * curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_URL, url);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, outputFile);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);

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

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

    CURLcode curlcode = curl_easy_perform(curl);
    
    if (curlcode != CURLE_OK) {
        fprintf(stderr, "%s\n", curl_easy_strerror(curlcode));
    }

    curl_easy_cleanup(curl);

    curl_global_cleanup();

    return curlcode;
}

int http_fetch_to_file(const char * url, const char * outputFilePath, bool verbose, bool showProgress) {
    FILE * file = fopen(outputFilePath, "wb");

    if (file == NULL) {
        perror(outputFilePath);
        return 1;
    }

    int resultCode = http_fetch_to_stream(url, file, verbose, showProgress);

    fclose(file);

    return resultCode;
}
