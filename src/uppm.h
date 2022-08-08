#ifndef UPPM_H
#define UPPM_H

#include <config.h>
#include <stdlib.h>

#ifndef UPPM_VERSION
#define UPPM_VERSION "0.0.0"
#endif

#define UPPM_OK               0
#define UPPM_ERROR            1

#define UPPM_ARG_IS_NULL      2
#define UPPM_ARG_IS_EMPTY     3
#define UPPM_ARG_IS_INVALID   4

#define UPPM_ENV_HOME_NOT_SET 5
#define UPPM_ENV_PATH_NOT_SET 6

#define UPPM_NETWORK_ERROR    7

#define UPPM_SHA256_MISMATCH  8

#define UPPM_PACKAGE_NAME_IS_NULL    10
#define UPPM_PACKAGE_NAME_IS_EMPTY   11
#define UPPM_PACKAGE_NAME_IS_INVALID 12

#define UPPM_PACKAGE_IS_NOT_AVAILABLE 15
#define UPPM_PACKAGE_IS_NOT_INSTALLED 16
#define UPPM_PACKAGE_IS_NOT_OUTDATED  17

#define UPPM_FORMULA_REPO_NOT_EXIST  20
#define UPPM_REPOS_CONFIG_READ_ERROR 21

#define UPPM_FORMULA_FILE_OPEN_ERROR  25
#define UPPM_FORMULA_FILE_READ_ERROR  26

#define UPPM_INSTALLED_METADATA_FILE_OPEN_ERROR  30
#define UPPM_INSTALLED_METADATA_FILE_READ_ERROR  31

#define UPPM_INFO_UNRECOGNIZED_KEY  41

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
int  uppm_formula_edit  (const char * packageName);
int  uppm_formula_create(const char * packageName);
int  uppm_formula_delete(const char * packageName);
int  uppm_formula_rename(const char * packageName, char * newPkgName);

void uppm_formula_free(UPPMFormula * formula);
void uppm_formula_dump(UPPMFormula * formula);

//////////////////////////////////////////////////////////////////////

typedef struct {
    char * name;
    char * url;
    char * path;
} UPPMFormulaRepo ;

typedef struct {
    UPPMFormulaRepo * * repos;
    size_t size;
} UPPMFormulaRepoList ;

typedef struct {
    char * * repoNames;
    size_t size;
} UPPMFormulaRepoNameList;

int  uppm_formula_repo_list_new (UPPMFormulaRepoList * * p);
void uppm_formula_repo_list_free(UPPMFormulaRepoList   * p);

int  uppm_formula_repo_name_list_new (UPPMFormulaRepoNameList * * p);
void uppm_formula_repo_name_list_free(UPPMFormulaRepoNameList   * p);

int  uppm_formula_repo_list_printf();
int  uppm_formula_repo_list_update();

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

    char * datatime;
    char * uppmvers;
} UPPMInstalledMetadata;

int  uppm_installed_metadata_parse(const char * packageName, UPPMInstalledMetadata * * metadata);
void uppm_installed_metadata_free(UPPMInstalledMetadata * metadata);
void uppm_installed_metadata_dump(UPPMInstalledMetadata * metadata);

//////////////////////////////////////////////////////////////////////

int uppm_main(int argc, char* argv[]);

int uppm_init();

int uppm_help();

int uppm_update_self();

int uppm_search(const char * keyword);

int uppm_info(const char * packageName, const char * key);

int uppm_tree(const char * packageName);

int uppm_depends(const char * packageName);

int uppm_fetch(const char * packageName);

int uppm_install(const char * packageName);

int uppm_reinstall(const char * packageName);

int uppm_uninstall(const char * packageName);

int uppm_upgrade(const char * packageName);

int uppm_integrate_zsh_completion(const char * outputDir);
int uppm_integrate_bash_completion(const char * outputDir);
int uppm_integrate_fish_completion(const char * outputDir);

int uppm_cleanup();

int uppm_is_package_name(const char * packageName);

int uppm_is_package_available(const char * packageName);
int uppm_is_package_installed(const char * packageName);
int uppm_is_package_outdated (const char * packageName);

int uppm_list_the_available_packages();
int uppm_list_the_installed_packages();
int uppm_list_the_outdated__packages();

#endif
