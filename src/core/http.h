#ifndef _HTTP_H
#define _HTTP_H

#include <stdio.h>
#include <stdbool.h>

int http_fetch_to_stream(const char * url, FILE       * outputStream,   bool verbose, bool showProgress);
int http_fetch_to_file  (const char * url, const char * outputFilePath, bool verbose, bool showProgress);

#endif
