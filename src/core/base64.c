#include "base64.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>

int base64_encode_of_string(char * * output, size_t * outputSizeInBytes, const char * input, size_t inputSizeInBytes) {
    if (output == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (input == NULL) {
        errno = EINVAL;
        return -1;
    }

    return base64_encode_of_bytes(output, outputSizeInBytes, (unsigned char *)input, inputSizeInBytes == 0 ? strlen(input) : inputSizeInBytes);
}

int base64_encode_of_bytes(char * * output, size_t * outputSizeInBytes, const unsigned char * input, size_t inputSizeInBytes) {
    if (output == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (input == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (inputSizeInBytes == 0) {
        errno = EINVAL;
        return -1;
    }

    size_t bufLength = (inputSizeInBytes << 2) / 3 + 3;
    char   buf[bufLength];
    memset(buf, 0, bufLength);

    int n = EVP_EncodeBlock((unsigned char *)buf, input, inputSizeInBytes);

    if (outputSizeInBytes != NULL) {
        (*outputSizeInBytes) = n;
    }

    char * p = strndup(buf, n);

    if (p == NULL) {
        errno = ENOMEM;
        return -1;
    } else {
        (*output) = p;
        return 0;
    }
}

int base64_decode_to_bytes(unsigned char * * output, size_t * outputSizeInBytes, const char * input, size_t inputSizeInBytes) {
    if (output == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (input == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (inputSizeInBytes == 0) {
        inputSizeInBytes = strlen(input);
    }

    if (inputSizeInBytes == 0) {
        errno = EINVAL;
        return -1;
    }

    size_t bufLength = (inputSizeInBytes * 3) >> 2;
    unsigned char  buf[bufLength];
    memset(buf, 0, bufLength);

    // EVP_DecodeBlock() returns the length of the data decoded or -1 on error.
    int n = EVP_DecodeBlock(buf, (unsigned char *)input, inputSizeInBytes);

    if (n == -1) {
        errno = EINVAL;
        return -1;
    }

    unsigned char * p = (unsigned char *)calloc(n, sizeof(unsigned char));

    if (p == NULL) {
        errno = ENOMEM;
        return -1;
    }

    memcpy(p, buf, n);

    (*output) = p;

    if (outputSizeInBytes != NULL) {
        (*outputSizeInBytes) = n;
    }

    return 0;
}

int base64_decode_to_string(char * * output, size_t * outputSizeInBytes, const char * input, size_t inputSizeInBytes) {
    if (output == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (input == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (inputSizeInBytes == 0) {
        inputSizeInBytes = strlen(input);
    }

    if (inputSizeInBytes == 0) {
        errno = EINVAL;
        return -1;
    }

    size_t bufLength = ((inputSizeInBytes * 3) >> 2) + 1;
    char   buf[bufLength];
    memset(buf, 0, bufLength);

    // EVP_DecodeBlock() returns the length of the data decoded or -1 on error.
    int n = EVP_DecodeBlock((unsigned char *)buf, (unsigned char *)input, inputSizeInBytes);

    if (n == -1) {
        errno = EINVAL;
        return -1;
    }

    if (outputSizeInBytes != NULL) {
        (*outputSizeInBytes) = n;
    }

    char * p = strndup(buf, n);

    if (p == NULL) {
        errno = ENOMEM;
        return -1;
    } else {
        (*output) = p;
        return 0;
    }
}
