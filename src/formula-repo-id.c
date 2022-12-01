#include <string.h>
#include "core/sha256sum.h"
#include "uppm.h"

char* uppm_formula_repo_id(const char * formulaRepoUrl, const char * branchName) {
    size_t  tmpStrLength = strlen(formulaRepoUrl) + strlen(branchName) + 1;
    char    tmpStr[tmpStrLength];
    memset( tmpStr, 0, tmpStrLength);
    sprintf(tmpStr, "%s%s", formulaRepoUrl, branchName);

    return sha256sum_of_string(tmpStr);
}
