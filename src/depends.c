#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include "core/http.h"
#include "core/url.h"

#include "uppm.h"

static int string_append(char ** outP, size_t * outSize, size_t * outCapacity, const char * buf, size_t bufLength) {
    size_t  oldCapacity = (*outCapacity);
    size_t needCapacity = oldCapacity + bufLength + 1U;

    if (needCapacity > oldCapacity) {
        size_t newCapacity = needCapacity + 256U;

        char * p = (char*)realloc(*outP, newCapacity * sizeof(char));

        if (p == NULL) {
            free(*outP);
            return UPPM_ERROR_MEMORY_ALLOCATE;
        } else {
            (*outP) = p;
            (*outCapacity) = newCapacity;
            memset(&p[*outSize], 0, 256U + bufLength + 1U);
        }
    }

    strncat(*outP, buf, bufLength);
        
    (*outSize) += bufLength;

    return UPPM_OK;
}

static int get_output_file_path(char outputFilePath[PATH_MAX], const char * packageName, UPPMDependsOutputType outputType, const char * outputPath) {
    const char * outputFileNameSuffix;

    switch (outputType) {
        case UPPMDependsOutputType_DOT: outputFileNameSuffix = "dot"; break;
        case UPPMDependsOutputType_BOX: outputFileNameSuffix = "box"; break;
        case UPPMDependsOutputType_SVG: outputFileNameSuffix = "svg"; break;
        case UPPMDependsOutputType_PNG: outputFileNameSuffix = "png"; break;
    }

    int ret;

    if (outputPath == NULL || outputPath[0] == '\0') {
        ret = snprintf(outputFilePath, strlen(packageName) + 20U, "%s-dependencies.%s", packageName, outputFileNameSuffix);
    } else {
        size_t outputPathLength = strlen(outputPath);

        if (strcmp(outputPath, ".") == 0) {
           ret = snprintf(outputFilePath, strlen(packageName) + 20U, "%s-dependencies.%s", packageName, outputFileNameSuffix);
        } else if (strcmp(outputPath, "..") == 0) {
           ret = snprintf(outputFilePath, strlen(packageName) + 23U, "../%s-dependencies.%s", packageName, outputFileNameSuffix);
        } else if (outputPath[outputPathLength - 1] == '/') {
           ret = snprintf(outputFilePath, strlen(outputPath) + strlen(packageName) + 20U, "%s%s-dependencies.%s", outputPath, packageName, outputFileNameSuffix);
        } else {
            size_t n = strlen(outputPath);
            strncpy(outputFilePath, outputPath, n);
            outputFilePath[n] = '\0';
        }
    }

    if (ret < 0) {
        perror(NULL);
        return UPPM_ERROR;
    }

    return UPPM_OK;
}

typedef struct {
    char * packageName;
    UPPMFormula * formula;
} UPPMPackage;

typedef struct {
    char * nodeList;
    size_t nodeListSize;
    size_t nodeListCapacity;
} DIRectedPath;

int uppm_depends(const char * packageName, UPPMDependsOutputType outputType, const char * outputPath) {
    int ret = UPPM_OK;

    ////////////////////////////////////////////////////////////////

    char * p = NULL;
    size_t pSize = 0;
    size_t pCapacity = 0;

    ////////////////////////////////////////////////////////////////

    size_t         packageSetCapacity = 0;
    size_t         packageSetSize    = 0;
    UPPMPackage ** packageSet        = NULL;

    ////////////////////////////////////////////////////////////////

    size_t   packageNameStackCapacity = 1;
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

            if (packageSetSize == packageSetCapacity) {
                UPPMPackage ** p = (UPPMPackage**)realloc(packageSet, (packageSetCapacity + 10U) * sizeof(UPPMPackage*));

                if (p == NULL) {
                    free(packageName);
                    uppm_formula_free(formula);
                    ret = UPPM_ERROR_MEMORY_ALLOCATE;
                    goto finalize;
                }

                memset(p + packageSetCapacity, 0, 10);

                packageSet = p;
                packageSetCapacity += 10;
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

        size_t bufLength = strlen(packageName) + 12U;
        char   buf[bufLength];

        ret = snprintf(buf, bufLength, "    \"%s\" -> {", packageName);

        if (ret < 0) {
            perror(NULL);
            return UPPM_ERROR;
        }

        ret = string_append(&p, &pSize, &pCapacity, buf, bufLength - 1);

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

            size_t bufLength = strlen(depPackageName) + 4U;
            char   buf[bufLength];

            ret = snprintf(buf, bufLength, " \"%s\"", depPackageName);

            if (ret < 0) {
                perror(NULL);
                return UPPM_ERROR;
            }

            ret = string_append(&p, &pSize, &pCapacity, buf, bufLength - 1U);

            if (ret != UPPM_OK) {
                goto finalize;
            }

            ////////////////////////////////////////////////////////////////

            ////////////////////////////////////////////////////////////////

            if (packageNameStackSize == packageNameStackCapacity) {
                char ** p = (char**)realloc(packageNameStack, (packageNameStackCapacity + 10U) * sizeof(char*));

                if (p == NULL) {
                    ret = UPPM_ERROR_MEMORY_ALLOCATE;
                    goto finalize;
                }

                memset(p + packageNameStackCapacity, 0, 10);

                packageNameStack = p;
                packageNameStackCapacity += 10;
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

        ret = string_append(&p, &pSize, &pCapacity, " }\n", 3);
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

    size_t dotScriptStrLength = pSize + 14U;
    char   dotScriptStr[dotScriptStrLength];

    ret = snprintf(dotScriptStr, dotScriptStrLength, "digraph G {\n%s}", p);

    if (ret < 0) {
        perror(NULL);
        free(p);
        return UPPM_ERROR;
    }

    free(p);

    dotScriptStrLength--;

    ////////////////////////////////////////////////////////////////

    if (outputType == UPPMDependsOutputType_DOT) {
        if (outputPath == NULL) {
            printf("%s\n", dotScriptStr);
            return UPPM_OK;
        }
    }

    ////////////////////////////////////////////////////////////////

    char   uppmHomeDIR[PATH_MAX];
    size_t uppmHomeDIRLength;

    ret = uppm_home_dir(uppmHomeDIR, PATH_MAX, &uppmHomeDIRLength);

    if (ret != UPPM_OK) {
        return ret;
    }

    ////////////////////////////////////////////////////////////////

    struct stat st;

    size_t uppmRunDIRCapacity = uppmHomeDIRLength + 5U;
    char   uppmRunDIR[uppmRunDIRCapacity];

    ret = snprintf(uppmRunDIR, uppmRunDIRCapacity, "%s/run", uppmHomeDIR);

    if (ret < 0) {
        perror(NULL);
        return UPPM_ERROR;
    }

    if (lstat(uppmRunDIR, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            if (unlink(uppmRunDIR) != 0) {
                perror(uppmRunDIR);
                return UPPM_ERROR;
            }

            if (mkdir(uppmRunDIR, S_IRWXU) != 0) {
                if (errno != EEXIST) {
                    perror(uppmRunDIR);
                    return UPPM_ERROR;
                }
            }
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

    size_t sessionDIRCapacity = uppmRunDIRCapacity + 20U;
    char   sessionDIR[sessionDIRCapacity];

    ret = snprintf(sessionDIR, sessionDIRCapacity, "%s/%d", uppmRunDIR, getpid());

    if (ret < 0) {
        perror(NULL);
        return UPPM_ERROR;
    }

    if (lstat(sessionDIR, &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            ret = uppm_rm_r(sessionDIR, false);

            if (ret != UPPM_OK) {
                return ret;
            }

            if (mkdir(sessionDIR, S_IRWXU) != 0) {
                perror(sessionDIR);
                return UPPM_ERROR;
            }
        } else {
            if (unlink(sessionDIR) != 0) {
                perror(sessionDIR);
                return UPPM_ERROR;
            }

            if (mkdir(sessionDIR, S_IRWXU) != 0) {
                perror(sessionDIR);
                return UPPM_ERROR;
            }
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

        size_t urlCapacity = urlEncodedRealLength + 66U;
        char   url[urlCapacity];

        ret = snprintf(url, urlCapacity, "https://dot-to-ascii.ggerganov.com/dot-to-ascii.php?boxart=1&src=%s", urlEncodedBuf);

        if (ret < 0) {
            perror(NULL);
            return UPPM_ERROR;
        }

        //printf("url=%s\n", url);

        if (outputPath == NULL) {
            int ret = http_fetch_to_stream(url, stdout, false, false);

            if (ret == -1) {
                return UPPM_ERROR;
            }

            if (ret > 0) {
                return UPPM_ERROR_NETWORK_BASE + ret;
            }

            return UPPM_OK;
        } else {
            size_t boxFilePathLength = sessionDIRCapacity + 18U;
            char   boxFilePath[boxFilePathLength];

            ret = snprintf(boxFilePath, boxFilePathLength, "%s/dependencies.box", sessionDIR);

            if (ret < 0) {
                perror(NULL);
                return UPPM_ERROR;
            }

            ret = http_fetch_to_file(url, boxFilePath, false, false);

            if (ret == -1) {
                perror(boxFilePath);
                return UPPM_ERROR;
            }

            if (ret > 0) {
                return UPPM_ERROR_NETWORK_BASE + ret;
            } else {
                char outputFilePath[PATH_MAX];

                ret = get_output_file_path(outputFilePath, packageName, outputType, outputPath);

                if (ret != UPPM_OK) {
                    return ret;
                }

                return uppm_rename_or_copy_file(boxFilePath, outputFilePath);
            }
        }
    } 

    ////////////////////////////////////////////////////////////////

    size_t dotFilePathCapacity = sessionDIRCapacity + 18U;
    char   dotFilePath[dotFilePathCapacity];

    ret = snprintf(dotFilePath, dotFilePathCapacity, "%s/dependencies.dot", sessionDIR);

    if (ret < 0) {
        perror(NULL);
        return UPPM_ERROR;
    }

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
        char outputFilePath[PATH_MAX];

        ret = get_output_file_path(outputFilePath, packageName, outputType, outputPath);

        if (ret != UPPM_OK) {
            return ret;
        }

        return uppm_rename_or_copy_file(dotFilePath, outputFilePath);
    }

    ////////////////////////////////////////////////////////////////

    size_t tmpFilePathCapacity = sessionDIRCapacity + 18U;
    char   tmpFilePath[tmpFilePathCapacity];

    ret = snprintf(tmpFilePath, tmpFilePathCapacity, "%s/dependencies.tmp", sessionDIR);

    if (ret < 0) {
        perror(NULL);
        return UPPM_ERROR;
    }

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
            char outputFilePath[PATH_MAX];

            ret = get_output_file_path(outputFilePath, packageName, outputType, outputPath);

            if (ret != UPPM_OK) {
                return ret;
            }

            return uppm_rename_or_copy_file(tmpFilePath, outputFilePath);
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
