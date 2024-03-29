#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include <openssl/evp.h>

#include "core/zlib-flate.h"
#include "core/base16.h"
#include "core/base64.h"
#include "core/exe.h"
#include "core/log.h"

#include "sha256sum.h"
#include "uppm.h"

//invoked as 'uppm util <CMD> [ARGUMENTS]'
int uppm_util(int argc, char* argv[]) {
    if (argv[2] == NULL) {
        fprintf(stderr, "Usage: %s %s <COMMAND> , <COMMAND> is unspecified.\n", argv[0], argv[1]);
        return UPPM_ERROR_ARG_IS_NULL;
    }

    if (argv[2][0] == '\0') {
        fprintf(stderr, "Usage: %s %s <COMMAND> , <COMMAND> should be a non-empty string.\n", argv[0], argv[1]);
        return UPPM_ERROR_ARG_IS_NULL;
    }

    if (strcmp(argv[2], "base16-encode") == 0) {
        if (argv[3] == NULL) {
            unsigned char inputBuf[1024];

            for (;;) {
                ssize_t readSizeInBytes = read(STDIN_FILENO, inputBuf, 1024);

                if (readSizeInBytes == -1) {
                    perror(NULL);
                    return UPPM_ERROR;
                }

                if (readSizeInBytes == 0) {
                    if (isatty(STDOUT_FILENO)) {
                        printf("\n");
                    }

                    return UPPM_OK;
                }

                size_t outputBufSizeInBytes = readSizeInBytes << 1;
                char   outputBuf[outputBufSizeInBytes];

                if (base16_encode(outputBuf, inputBuf, readSizeInBytes, true) != 0) {
                    perror(NULL);
                    return UPPM_ERROR;
                }

                ssize_t writeSizeInBytes = write(STDOUT_FILENO, outputBuf, outputBufSizeInBytes);

                if (writeSizeInBytes == -1) {
                    perror(NULL);
                    return UPPM_ERROR;
                }

                if ((size_t)writeSizeInBytes != outputBufSizeInBytes) {
                    fprintf(stderr, "not fully written to stdout.\n");
                    return UPPM_ERROR;
                }
            }
        } else {
            if (argv[3][0] == '\0') {
                fprintf(stderr, "Usage: %s %s %s <STR> , <STR> should be a non-empty string.\n", argv[0], argv[1], argv[2]);
                return UPPM_ERROR_ARG_IS_NULL;
            }

            unsigned char * inputBuf = (unsigned char *)argv[3];
            size_t          inputBufSizeInBytes = strlen(argv[3]);

            size_t outputBufSizeInBytes = inputBufSizeInBytes << 1;
            char   outputBuf[outputBufSizeInBytes];

            if (base16_encode(outputBuf, inputBuf, inputBufSizeInBytes, true) != 0) {
                return UPPM_ERROR;
            }

            ssize_t writeSizeInBytes = write(STDOUT_FILENO, outputBuf, outputBufSizeInBytes);

            if (writeSizeInBytes == -1) {
                perror(NULL);
                return UPPM_ERROR;
            }

            if ((size_t)writeSizeInBytes != outputBufSizeInBytes) {
                fprintf(stderr, "not fully written to stdout.\n");
                return UPPM_ERROR;
            }

            if (isatty(STDOUT_FILENO)) {
                printf("\n");
            }

            return UPPM_OK;
        }
    }

    if (strcmp(argv[2], "base16-decode") == 0) {
        if (argv[3] == NULL) {
            fprintf(stderr, "Usage: %s %s %s <BASE16-ENCODED-STR> , <BASE16-ENCODED-STR> is unspecified.\n", argv[0], argv[1], argv[2]);
            return UPPM_ERROR_ARG_IS_NULL;
        }

        if (argv[3][0] == '\0') {
            fprintf(stderr, "Usage: %s %s %s <BASE16-ENCODED-STR> , <BASE16-ENCODED-STR> should be non-empty.\n", argv[0], argv[1], argv[2]);
            return UPPM_ERROR_ARG_IS_NULL;
        }

        size_t inputBufSizeInBytes = strlen(argv[3]);

        if ((inputBufSizeInBytes & 1) != 0) {
            fprintf(stderr, "Usage: %s %s %s <BASE16-ENCODED-STR> , <BASE16-ENCODED-STR> length should be an even number.\n", argv[0], argv[1], argv[2]);
            return UPPM_ERROR_ARG_IS_INVALID;
        }

        size_t        outputBufSizeInBytes = inputBufSizeInBytes >> 1;
        unsigned char outputBuf[outputBufSizeInBytes];

        if (base16_decode(outputBuf, argv[3], inputBufSizeInBytes) == 0) {
            ssize_t writeSizeInBytes = write(STDOUT_FILENO, outputBuf, outputBufSizeInBytes);

            if (writeSizeInBytes == -1) {
                perror(NULL);
                return UPPM_ERROR;
            }

            if ((size_t)writeSizeInBytes != outputBufSizeInBytes) {
                fprintf(stderr, "not fully written to stdout.\n");
                return UPPM_ERROR;
            }

            if (isatty(STDOUT_FILENO)) {
                printf("\n");
            }

            return UPPM_OK;
        } else {
            perror(NULL);

            if (errno == EINVAL) {
                return UPPM_ERROR_ARG_IS_INVALID;
            } else {
                return UPPM_ERROR;
            }
        }
    }

    if (strcmp(argv[2], "base64-encode") == 0) {
        if (argv[3] == NULL) {
            unsigned char inputBuf[1023];

            for (;;) {
                ssize_t readSizeInBytes = read(STDIN_FILENO, inputBuf, 1023);

                if (readSizeInBytes == -1) {
                    perror(NULL);
                    return UPPM_ERROR;
                }

                if (readSizeInBytes == 0) {
                    if (isatty(STDOUT_FILENO)) {
                        printf("\n");
                    }

                    return UPPM_OK;
                }

                unsigned int  x = (readSizeInBytes % 3) == 0 ? 0 : 1;
                unsigned int  outputBufSizeInBytes = (readSizeInBytes / 3 + x) << 2;
                unsigned char outputBuf[outputBufSizeInBytes];

                int ret = EVP_EncodeBlock(outputBuf, inputBuf, readSizeInBytes);

                if (ret < 0) {
                    return ret;
                }

                ssize_t writeSizeInBytes = write(STDOUT_FILENO, outputBuf, outputBufSizeInBytes);

                if (writeSizeInBytes == -1) {
                    perror(NULL);
                    return UPPM_ERROR;
                }

                if ((size_t)writeSizeInBytes != outputBufSizeInBytes) {
                    fprintf(stderr, "not fully written to stdout.\n");
                    return UPPM_ERROR;
                }
            }
        } else {
            if (argv[3][0] == '\0') {
                fprintf(stderr, "Usage: %s %s %s <STR> , <STR> should be a non-empty string.\n", argv[0], argv[1], argv[2]);
                return UPPM_ERROR_ARG_IS_NULL;
            }

            unsigned char * inputBuf = (unsigned char *)argv[3];
            unsigned int    inputBufSizeInBytes = strlen(argv[3]);

            unsigned int  x = (inputBufSizeInBytes % 3) == 0 ? 0 : 1;
            unsigned int  outputBufSizeInBytes = (inputBufSizeInBytes / 3 + x) << 2;
            unsigned char outputBuf[outputBufSizeInBytes];

            int ret = EVP_EncodeBlock(outputBuf, inputBuf, inputBufSizeInBytes);

            if (ret < 0) {
                return ret;
            }

            ssize_t writeSizeInBytes = write(STDOUT_FILENO, outputBuf, outputBufSizeInBytes);

            if (writeSizeInBytes == -1) {
                perror(NULL);
                return UPPM_ERROR;
            }

            if ((size_t)writeSizeInBytes != outputBufSizeInBytes) {
                fprintf(stderr, "not fully written to stdout.\n");
                return UPPM_ERROR;
            }

            if (isatty(STDOUT_FILENO)) {
                printf("\n");
            }

            return UPPM_OK;
        }
    }

    if (strcmp(argv[2], "base64-decode") == 0) {
        if (argv[3] == NULL) {
            unsigned char readBuf[1024];

            for (;;) {
                ssize_t readSizeInBytes = read(STDIN_FILENO, readBuf, 1024);

                if (readSizeInBytes == -1) {
                    perror(NULL);
                    return UPPM_ERROR;
                }

                if (readSizeInBytes == 0) {
                    if (isatty(STDOUT_FILENO)) {
                        printf("\n");
                    }

                    return UPPM_OK;
                }

                unsigned int  outputBufSizeInBytes = (readSizeInBytes >> 2) * 3;
                unsigned char outputBuf[outputBufSizeInBytes];

                // EVP_DecodeBlock() returns the length of the data decoded or -1 on error.
                int n = EVP_DecodeBlock(outputBuf, readBuf, readSizeInBytes);

                if (n < 0) {
                    return UPPM_ERROR_ARG_IS_INVALID;
                }

                ssize_t writeSizeInBytes = write(STDOUT_FILENO, outputBuf, outputBufSizeInBytes);

                if (writeSizeInBytes == -1) {
                    perror(NULL);
                    return UPPM_ERROR;
                }

                if ((size_t)writeSizeInBytes != outputBufSizeInBytes) {
                    fprintf(stderr, "not fully written to stdout.\n");
                    return UPPM_ERROR;
                }
            }
        } else {
            if (argv[3][0] == '\0') {
                fprintf(stderr, "Usage: %s %s %s <BASE64-ENCODED-STR> , <BASE64-ENCODED-STR> should be a non-empty string.\n", argv[0], argv[1], argv[2]);
                return UPPM_ERROR_ARG_IS_NULL;
            }

            unsigned char * inputBuf = (unsigned char *)argv[3];
            unsigned int    inputBufSizeInBytes = strlen(argv[3]);

            unsigned int  outputBufSizeInBytes = (inputBufSizeInBytes >> 2) * 3;
            unsigned char outputBuf[outputBufSizeInBytes];

            // EVP_DecodeBlock() returns the length of the data decoded or -1 on error.
            int n = EVP_DecodeBlock(outputBuf, inputBuf, inputBufSizeInBytes);

            if (n < 0) {
                return UPPM_ERROR_ARG_IS_INVALID;
            }

            ssize_t writeSizeInBytes = write(STDOUT_FILENO, outputBuf, outputBufSizeInBytes);

            if (writeSizeInBytes == -1) {
                perror(NULL);
                return UPPM_ERROR;
            }

            if ((size_t)writeSizeInBytes != outputBufSizeInBytes) {
                fprintf(stderr, "not fully written to stdout.\n");
                return UPPM_ERROR;
            }

            if (isatty(STDOUT_FILENO)) {
                printf("\n");
            }

            return UPPM_OK;
        }
    }

    if (strcmp(argv[2], "sha256sum") == 0) {
        if (argv[3] == NULL || strcmp(argv[3], "-") == 0) {
            char outputBuf[65] = {0};

            if (sha256sum_of_stream(outputBuf, stdin) == 0) {
                printf("%s\n", outputBuf);
                return UPPM_OK;
            } else {
                perror(NULL);
                return UPPM_ERROR;
            }
        } else if (strcmp(argv[3], "-h") == 0 || strcmp(argv[3], "--help") == 0) {
            fprintf(stderr, "Usage: %s %s %s [FILEPATH]\n", argv[0], argv[1], argv[2]);
            return UPPM_OK;
        } else {
            char outputBuf[65] = {0};

            if (sha256sum_of_file(outputBuf, argv[3]) == 0) {
                printf("%s\n", outputBuf);
                return UPPM_OK;
            } else {
                return UPPM_ERROR;
            }
        }
    }

    if (strcmp(argv[2], "zlib-deflate") == 0) {
        int level = 1;

        for (int i = 3; i < argc; i++) {
            if (strcmp(argv[i], "-L") == 0) {
                char * p = argv[i + 1];

                if (p == NULL) {
                    fprintf(stderr, "Usage: %s %s %s [-L N] (N>=1 && N <=9) , The smaller the N, the faster the speed and the lower the compression ratio.\n", argv[0], argv[1], argv[2]);
                    return UPPM_ERROR;
                }

                if (strlen(p) != 1) {
                    fprintf(stderr, "Usage: %s %s %s [-L N] (N>=1 && N <=9) , The smaller the N, the faster the speed and the lower the compression ratio.\n", argv[0], argv[1], argv[2]);
                    return UPPM_ERROR;
                }

                if (p[0] < '1' || p[0] > '9') {
                    fprintf(stderr, "Usage: %s %s %s [-L N] (N>=1 && N <=9) , The smaller the N, the faster the speed and the lower the compression ratio.\n", argv[0], argv[1], argv[2]);
                    return UPPM_ERROR;
                }

                level = atoi(p);

                i++;
            } else {
                fprintf(stderr, "unrecognized option: %s", argv[i]);
                fprintf(stderr, "Usage: %s %s %s [-L N] (N>=1 && N <=9) , The smaller the N, the faster the speed and the lower the compression ratio.\n", argv[0], argv[1], argv[2]);
                return UPPM_ERROR;
            }
        }

        return zlib_deflate_file_to_file(stdin, stdout, level);
    }

    if (strcmp(argv[2], "zlib-inflate") == 0) {
        return zlib_inflate_file_to_file(stdin, stdout);
    }

    if (strcmp(argv[2], "which") == 0) {
        if (argv[3] == NULL) {
            fprintf(stderr, "USAGE: %s %s %s <COMMAND-NAME> , <COMMAND-NAME> is unspecified.\n", argv[0], argv[1], argv[2]);
            return 1;
        }

        if (argv[3][0] == '\0') {
            fprintf(stderr, "USAGE: %s %s %s <COMMAND-NAME> , <COMMAND-NAME> should be a non-empty string.\n", argv[0], argv[1], argv[2]);
            return 1;
        }

        bool findAll = false;

        for (int i = 4; i < argc; i++) {
            if (strcmp(argv[i], "-a") == 0) {
                findAll = true;
            } else {
                fprintf(stderr, "unrecognized argument: %s\n", argv[i]);
                fprintf(stderr, "USAGE: %s %s %s <COMMAND-NAME> [-a]\n", argv[0], argv[1], argv[2]);
                return 1;
            }
        }

        char ** pathList = NULL;

        int ret = exe_search(argv[3], &pathList, findAll);

        if (ret > 0) {
            for (int i = 0; i < ret; i++) {
                printf("%s\n", pathList[i]);

                free(pathList[i]);
                pathList[i] = NULL;
            }

            free(pathList);
            pathList = NULL;

            return 0;
        }

        return ret;
    }

    LOG_ERROR2("unrecognized command: ", argv[2]);
    return UPPM_ERROR_ARG_IS_UNKNOWN;
}
