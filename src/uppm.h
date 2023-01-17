#ifndef UPPM_H
#define UPPM_H

#include <config.h>
#include <stdlib.h>
#include <stdbool.h>


#define UPPM_PACKAGE_NAME_PATTERN "^[A-Za-z0-9+-._]{1,50}$"


#define UPPM_OK                     0
#define UPPM_ERROR                  1

#define UPPM_ERROR_ARG_IS_NULL      2
#define UPPM_ERROR_ARG_IS_EMPTY     3
#define UPPM_ERROR_ARG_IS_INVALID   4
#define UPPM_ERROR_ARG_IS_UNKNOWN   5

#define UPPM_ERROR_MEMORY_ALLOCATE  6

#define UPPM_ERROR_SHA256_MISMATCH  7

#define UPPM_ERROR_ENV_HOME_NOT_SET 8
#define UPPM_ERROR_ENV_PATH_NOT_SET 9

#define UPPM_ERROR_PACKAGE_NOT_AVAILABLE 25
#define UPPM_ERROR_PACKAGE_NOT_INSTALLED 26
#define UPPM_ERROR_PACKAGE_NOT_OUTDATED  27
#define UPPM_ERROR_PACKAGE_IS_BROKEN     28

#define UPPM_ERROR_FORMULA_REPO_CONFIG_SYNTAX 30
#define UPPM_ERROR_FORMULA_REPO_CONFIG_SCHEME 31

#define UPPM_ERROR_FORMULA_SYNTAX     40
#define UPPM_ERROR_FORMULA_SCHEME     41

#define UPPM_ERROR_RECEIPT_SYNTAX     45
#define UPPM_ERROR_RECEIPT_SCHEME     46

#define UPPM_ERROR_URL_TRANSFORM_ENV_NOT_SET           50
#define UPPM_ERROR_URL_TRANSFORM_ENV_VALUE_IS_EMPTY    51
#define UPPM_ERROR_URL_TRANSFORM_ENV_POINT_TO_PATH_NOT_EXIST 52
#define UPPM_ERROR_URL_TRANSFORM_RUN_NO_RESULT         53

// libgit's error [-35, -1]
#define UPPM_ERROR_LIBGIT2_BASE    70

// libarchive's error [-30, 1]
#define UPPM_ERROR_ARCHIVE_BASE    110

// libcurl's error [1, 99]
#define UPPM_ERROR_NETWORK_BASE    150

#define UPPM_CHECK_IF_MEMORY_ALLOCATION_FAILURE(ptr) if ((ptr) == NULL) { return UPPM_ERROR_MEMORY_ALLOCATE; }


void uppm_show_error_message(int errorCode, const char * str);

typedef struct {
    char * summary;
    char * version;
    char * license;
    char * webpage;
    char * bin_url;
    char * bin_sha;
    char * dep_pkg;
    char * install;
    char * path;
} UPPMFormula;

int  uppm_formula_parse (const char * packageName, UPPMFormula * * formula);
int  uppm_formula_find  (const char * packageName, char * * out);
int  uppm_formula_cat   (const char * packageName);
int  uppm_formula_bat   (const char * packageName);

void uppm_formula_free(UPPMFormula * formula);
void uppm_formula_dump(UPPMFormula * formula);

//////////////////////////////////////////////////////////////////////

typedef struct {
    char * name;
    char * url;
    char * branch;
    char * path;
    char * timestamp_added;
    char * timestamp_last_updated;
    bool   pinned;
    bool   enabled;
} UPPMFormulaRepo ;

typedef struct {
    UPPMFormulaRepo * * repos;
    size_t size;
} UPPMFormulaRepoList ;

int  uppm_formula_repo_parse(const char * formulaRepoConfigFilePath, UPPMFormulaRepo * * formulaRepo);
void uppm_formula_repo_free(UPPMFormulaRepo * formulaRepo);
void uppm_formula_repo_dump(UPPMFormulaRepo * formulaRepo);
int  uppm_formula_repo_update(UPPMFormulaRepo * formulaRepo);

int  uppm_formula_repo_list_new (UPPMFormulaRepoList * * p);
void uppm_formula_repo_list_free(UPPMFormulaRepoList   * p);

int  uppm_formula_repo_list_printf();
int  uppm_formula_repo_list_update();

int   uppm_formula_repo_add(const char * formulaRepoName, const char * formulaRepoUrl, const char * branchName);
int   uppm_formula_repo_del(const char * formulaRepoName);

//////////////////////////////////////////////////////////////////////

typedef struct {
    char * summary;
    char * version;
    char * license;
    char * webpage;
    char * bin_url;
    char * bin_sha;
    char * dep_pkg;
    char * install;

    char * signature;
    char * timestamp;
} UPPMReceipt;

int  uppm_receipt_parse(const char * packageName, UPPMReceipt * * receipt);
void uppm_receipt_free(UPPMReceipt * receipt);
void uppm_receipt_dump(UPPMReceipt * receipt);

//////////////////////////////////////////////////////////////////////

typedef enum {
    UPPMDependsOutputFormat_DOT,
    UPPMDependsOutputFormat_BOX,
    UPPMDependsOutputFormat_SVG,
    UPPMDependsOutputFormat_PNG,
} UPPMDependsOutputFormat;

//////////////////////////////////////////////////////////////////////

int uppm_main(int argc, char* argv[]);

int uppm_help();

int uppm_env(bool verbose);

int uppm_search(const char * keyword);

int uppm_info(const char * packageName, const char * key);

int uppm_tree(const char * packageName, size_t argc, char* argv[]);

int uppm_depends(const char * packageName, UPPMDependsOutputFormat outputFormat);

int uppm_fetch(const char * packageName, bool verbose);

int uppm_install(const char * packageName, bool verbose);

int uppm_reinstall(const char * packageName, bool verbose);

int uppm_uninstall(const char * packageName, bool verbose);

int uppm_upgrade(const char * packageName, bool verbose);

int uppm_upgrade_self(bool verbose);

int uppm_integrate_zsh_completion (const char * outputDir, bool verbose);
int uppm_integrate_bash_completion(const char * outputDir, bool verbose);
int uppm_integrate_fish_completion(const char * outputDir, bool verbose);

int uppm_cleanup(bool verbose);

int uppm_check_if_the_given_argument_matches_package_name_pattern(const char * arg);

int uppm_check_if_the_given_package_is_available(const char * packageName);
int uppm_check_if_the_given_package_is_installed(const char * packageName);
int uppm_check_if_the_given_package_is_outdated (const char * packageName);

int uppm_list_the_available_packages();
int uppm_list_the_installed_packages();
int uppm_list_the_outdated__packages();

int uppm_fetch_via_git(const char * gitRepositoryDirPath, const char * remoteUrl, const char * refspec, const char * checkoutToBranchName);

int uppm_url_transform(const char * inUrl, char * * outUrlPointer, bool verbose);

#endif
