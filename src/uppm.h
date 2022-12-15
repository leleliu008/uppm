#ifndef UPPM_H
#define UPPM_H

#include <config.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef UPPM_VERSION
#define UPPM_VERSION "0.0.0"
#endif

#define UPPM_PACKAGE_NAME_PATTERN "^[A-Za-z0-9+-._]{1,50}$"


#define UPPM_OK               0
#define UPPM_ERROR            1

#define UPPM_ARG_IS_NULL      10
#define UPPM_ARG_IS_EMPTY     11
#define UPPM_ARG_IS_INVALID   12
#define UPPM_ARG_IS_UNKNOWN   13

#define UPPM_ENV_HOME_NOT_SET 5
#define UPPM_ENV_PATH_NOT_SET 6

#define UPPM_NETWORK_ERROR    7

#define UPPM_SHA256_MISMATCH  8

#define UPPM_PACKAGE_IS_NOT_AVAILABLE 25
#define UPPM_PACKAGE_IS_NOT_INSTALLED 26
#define UPPM_PACKAGE_IS_NOT_OUTDATED  27

#define UPPM_FORMULA_REPO_NOT_EXIST   30

#define UPPM_FORMULA_FILE_OPEN_ERROR  35
#define UPPM_FORMULA_FILE_READ_ERROR  36

#define UPPM_INSTALLED_METADATA_FILE_OPEN_ERROR  40
#define UPPM_INSTALLED_METADATA_FILE_READ_ERROR  41

#define UPPM_INSTALLED_FILES_CONFIG_FILE_NOT_EXISTS  45
#define UPPM_INSTALLED_FILES_CONFIG_FILE_OPEN_ERROR  46

#define UPPM_INFO_UNRECOGNIZED_KEY  51

void uppm_show_error_message(int errorCode, const char * str);

typedef struct {
    char * summary;
    char * webpage;
    char * version;
    char * license;
    char * bin_url;
    char * bin_sha;
    char * dep_pkg;
    char * install;
    char * path;
} UPPMFormula;

int  uppm_formula_parse (const char * packageName, UPPMFormula * * formula);
int  uppm_formula_path  (const char * packageName, char * * out);
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
} UPPMFormulaRepo ;

typedef struct {
    UPPMFormulaRepo * * repos;
    size_t size;
} UPPMFormulaRepoList ;

int  uppm_formula_repo_list_new (UPPMFormulaRepoList * * p);
void uppm_formula_repo_list_free(UPPMFormulaRepoList   * p);

int  uppm_formula_repo_list_printf();
int  uppm_formula_repo_list_update();

int   uppm_formula_repo_add(const char * formulaRepoName, const char * formulaRepoUrl, const char * branchName);
int   uppm_formula_repo_del(const char * formulaRepoName);

//////////////////////////////////////////////////////////////////////

typedef struct {
    char * summary;
    char * webpage;
    char * version;
    char * license;
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

#endif
