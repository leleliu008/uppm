#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <fcntl.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include "core/http.h"
#include "core/url.h"

#include "uppm.h"

static inline int rename_or_copy_file_to_destination(const char * fromFilePath, const char * toFilePath) {
    size_t  outputFilePathCopyLength = strlen(toFilePath) + 1U;
    char    outputFilePathCopy[outputFilePathCopyLength];
    strncpy(outputFilePathCopy, toFilePath, outputFilePathCopyLength);

    char * outputDIR = dirname(outputFilePathCopy);

    int ret = uppm_mkdir_p(outputDIR, false);

    if (ret != UPPM_OK) {
        return ret;
    }

    if (rename(fromFilePath, toFilePath) == 0) {
        return UPPM_OK;
    } else {
        if (errno == EXDEV) {
            return uppm_copy_file(fromFilePath, toFilePath);
        } else {
            perror(toFilePath);
            return UPPM_ERROR;
        }
    }
}

static int string_append(char ** outP, size_t * outSize, size_t * outCapcity, const char * buf, size_t bufLength) {
    size_t  oldCapcity = (*outCapcity);
    size_t needCapcity = oldCapcity + bufLength + 1U;

    if (needCapcity > oldCapcity) {
        size_t newCapcity = needCapcity + 256U;

        char * p = (char*)realloc(*outP, newCapcity * sizeof(char));

        if (p == NULL) {
            free(*outP);
            return UPPM_ERROR_MEMORY_ALLOCATE;
        } else {
            (*outP) = p;
            (*outCapcity) = newCapcity;
            memset(&p[*outSize], 0, 256U + bufLength + 1U);
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
} DIRectedPath;

static int uppm_depends2(const char * packageName, UPPMDependsOutputType outputType, const char * outputFilePath) {
    int ret = UPPM_OK;

    ////////////////////////////////////////////////////////////////

    char * p = NULL;
    size_t pSize = 0;
    size_t pCapcity = 0;

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
                UPPMPackage ** p = (UPPMPackage**)realloc(packageSet, (packageSetCapcity + 10U) * sizeof(UPPMPackage*));

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

        size_t   bufLength = strlen(packageName) + 12U;
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

            size_t  bufLength = strlen(depPackageName) + 4U;
            char    buf[bufLength];
            snprintf(buf, bufLength, " \"%s\"", depPackageName);

            ret = string_append(&p, &pSize, &pCapcity, buf, bufLength - 1U);

            if (ret != UPPM_OK) {
                goto finalize;
            }

            ////////////////////////////////////////////////////////////////

            ////////////////////////////////////////////////////////////////

            if (packageNameStackSize == packageNameStackCapcity) {
                char ** p = (char**)realloc(packageNameStack, (packageNameStackCapcity + 10U) * sizeof(char*));

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

    size_t   dotScriptStrLength = pSize + 14U;
    char     dotScriptStr[dotScriptStrLength];
    snprintf(dotScriptStr, dotScriptStrLength, "digraph G {\n%s}", p);

    free(p);

    dotScriptStrLength--;

    ////////////////////////////////////////////////////////////////

    if (outputType == UPPMDependsOutputType_DOT) {
        if (outputFilePath == NULL) {
            printf("%s\n", dotScriptStr);
            return UPPM_OK;
        }
    }

    ////////////////////////////////////////////////////////////////

    char   uppmHomeDIR[256] = {0};
    size_t uppmHomeDIRLength;

    ret = uppm_home_dir(uppmHomeDIR, 255, &uppmHomeDIRLength);

    if (ret != UPPM_OK) {
        return ret;
    }

    ////////////////////////////////////////////////////////////////

    struct stat st;

    size_t   uppmRunDIRLength = uppmHomeDIRLength + 5U;
    char     uppmRunDIR[uppmRunDIRLength];
    snprintf(uppmRunDIR, uppmRunDIRLength, "%s/run", uppmHomeDIR);

    if (stat(uppmRunDIR, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            fprintf(stderr, "%s was expected to be a directory, but it was not.\n", uppmRunDIR);
            return UPPM_ERROR;
        }
    } else {
        if (mkdir(uppmRunDIR, S_IRWXU) != 0) {
            if (errno != EEXIST) {
                perror(uppmRunDIR);
                return UPPM_ERROR;
            }
        }
    }

    ////////////////////////////////////////////////////////////////

    size_t   sessionDIRLength = uppmRunDIRLength + 20U;
    char     sessionDIR[sessionDIRLength];
    snprintf(sessionDIR, sessionDIRLength, "%s/%d", uppmRunDIR, getpid());

    if (stat(sessionDIR, &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            ret = uppm_rm_r(sessionDIR, false);

            if (ret != UPPM_OK) {
                return ret;
            }
        } else {
            fprintf(stderr, "%s was expected to be a directory, but it was not.\n", sessionDIR);
            return UPPM_ERROR;
        }
    } else {
        if (mkdir(sessionDIR, S_IRWXU) != 0) {
            perror(sessionDIR);
            return UPPM_ERROR;
        }
    }

    ////////////////////////////////////////////////////////////////

    if (outputType == UPPMDependsOutputType_BOX) {
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

        if (outputFilePath == NULL) {
            int ret = http_fetch_to_stream(url, stdout, false, false);

            if (ret == -1) {
                return UPPM_ERROR;
            }

            if (ret > 0) {
                return UPPM_ERROR_NETWORK_BASE + ret;
            }

            return UPPM_OK;
        } else {
            size_t   boxFilePathLength = sessionDIRLength + 18U;
            char     boxFilePath[boxFilePathLength];
            snprintf(boxFilePath, boxFilePathLength, "%s/dependencies.box", sessionDIR);

            int ret = http_fetch_to_file(url, boxFilePath, false, false);

            if (ret == -1) {
                perror(outputFilePath);
                return UPPM_ERROR;
            }

            if (ret > 0) {
                return UPPM_ERROR_NETWORK_BASE + ret;
            } else {
                return rename_or_copy_file_to_destination(boxFilePath, outputFilePath);
            }
        }
    } 

    ////////////////////////////////////////////////////////////////

    size_t   dotFilePathLength = sessionDIRLength + 18U;
    char     dotFilePath[dotFilePathLength];
    snprintf(dotFilePath, dotFilePathLength, "%s/dependencies.dot", sessionDIR);

    int dotFD = open(dotFilePath, O_CREAT | O_WRONLY | O_TRUNC, 0666);

    if (dotFD == -1) {
        perror(dotFilePath);
        return UPPM_ERROR;
    }

    ret = write(dotFD, dotScriptStr, dotScriptStrLength);

    if (ret == -1) {
        perror(dotFilePath);
        close(dotFD);
        return UPPM_ERROR;
    }

    close(dotFD);

    if ((size_t)ret != dotScriptStrLength) {
        fprintf(stderr, "not fully written: %s\n", dotFilePath);
        return UPPM_ERROR;
    }

    ////////////////////////////////////////////////////////////////

    if (outputType == UPPMDependsOutputType_DOT) {
        return rename_or_copy_file_to_destination(dotFilePath, outputFilePath);
    }

    ////////////////////////////////////////////////////////////////

    size_t   tmpFilePathLength = sessionDIRLength + 18U;
    char     tmpFilePath[tmpFilePathLength];
    snprintf(tmpFilePath, tmpFilePathLength, "%s/dependencies.tmp", sessionDIR);

    ////////////////////////////////////////////////////////////////

    int pid = fork();

    if (pid == -1) {
        perror(NULL);
        return UPPM_ERROR;
    }

    if (pid == 0) {
        if (outputType == UPPMDependsOutputType_PNG) {
            execlp("dot", "dot", "-Tpng", "-o", tmpFilePath, dotFilePath, NULL);
        } else if (outputType == UPPMDependsOutputType_SVG) {
            execlp("dot", "dot", "-Tsvg", "-o", tmpFilePath, dotFilePath, NULL);
        }

        perror("dot");
        exit(255);
    } else {
        int childProcessExitStatusCode;

        if (waitpid(pid, &childProcessExitStatusCode, 0) < 0) {
            perror(NULL);
            return UPPM_ERROR;
        }

        if (childProcessExitStatusCode == 0) {
            return rename_or_copy_file_to_destination(tmpFilePath, outputFilePath);
        }

        const char * type;

        if (outputType == UPPMDependsOutputType_PNG) {
            type = "-Tpng";
        } else if (outputType == UPPMDependsOutputType_SVG) {
            type = "-Tsvg";
        }

        if (WIFEXITED(childProcessExitStatusCode)) {
            fprintf(stderr, "running command 'dot %s -o %s %s' exit with status code: %d\n", type, tmpFilePath, dotFilePath, WEXITSTATUS(childProcessExitStatusCode));
        } else if (WIFSIGNALED(childProcessExitStatusCode)) {
            fprintf(stderr, "running command 'dot %s -o %s %s' killed by signal: %d\n", type, tmpFilePath, dotFilePath, WTERMSIG(childProcessExitStatusCode));
        } else if (WIFSTOPPED(childProcessExitStatusCode)) {
            fprintf(stderr, "running command 'dot %s -o %s %s' stopped by signal: %d\n", type, tmpFilePath, dotFilePath, WSTOPSIG(childProcessExitStatusCode));
        }

        return UPPM_ERROR;
    }
}

int uppm_depends(const char * packageName, UPPMDependsOutputType outputType, const char * outputPath) {
    char * outputFilePath = NULL;

    if (outputPath != NULL) {
        struct stat st;

        if (stat(outputPath, &st) == 0 && S_ISDIR(st.st_mode)) {
            size_t outputFilePathLength = strlen(outputPath) + strlen(packageName) + 20U;

            outputFilePath = (char*) malloc(outputFilePathLength);

            if (outputFilePath == NULL) {
                return UPPM_ERROR_MEMORY_ALLOCATE;
            }

            const char * outputFileNameSuffix;

            switch (outputType) {
                case UPPMDependsOutputType_DOT: outputFileNameSuffix = "dot"; break;
                case UPPMDependsOutputType_BOX: outputFileNameSuffix = "box"; break;
                case UPPMDependsOutputType_SVG: outputFileNameSuffix = "svg"; break;
                case UPPMDependsOutputType_PNG: outputFileNameSuffix = "png"; break;
            }

            snprintf(outputFilePath, outputFilePathLength, "%s/%s-dependencies.%s", outputPath, packageName, outputFileNameSuffix);
        } else {
            size_t outputPathLength = strlen(outputPath);

            if (outputPath[outputPathLength - 1] == '/') {
                size_t outputFilePathLength = strlen(outputPath) + strlen(packageName) + 20U;

                outputFilePath = (char*) malloc(outputFilePathLength);

                if (outputFilePath == NULL) {
                    return UPPM_ERROR_MEMORY_ALLOCATE;
                }

                const char * outputFileNameSuffix;

                switch (outputType) {
                    case UPPMDependsOutputType_DOT: outputFileNameSuffix = "dot"; break;
                    case UPPMDependsOutputType_BOX: outputFileNameSuffix = "box"; break;
                    case UPPMDependsOutputType_SVG: outputFileNameSuffix = "svg"; break;
                    case UPPMDependsOutputType_PNG: outputFileNameSuffix = "png"; break;
                }

                snprintf(outputFilePath, outputFilePathLength, "%s%s-dependencies.%s", outputPath, packageName, outputFileNameSuffix);
            } else {
                outputFilePath = strdup(outputPath);

                if (outputFilePath == NULL) {
                    return UPPM_ERROR_MEMORY_ALLOCATE;
                }
            }
        }
    }

    int ret = uppm_depends2(packageName, outputType, outputFilePath);

    free(outputFilePath);

    return ret;
}
