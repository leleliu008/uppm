#ifndef UPPM_H
#define UPPM_H

#include <config.h>
#include <stdlib.h>

#ifndef UPPM_VERSION
#define UPPM_VERSION "0.0.0"
#endif

#define UPPM_OK               0
#define UPPM_ERROR            1
#define UPPM_ARG_INVALID      2
#define UPPM_ENV_HOME_NOT_SET 3
#define UPPM_ENV_PATH_NOT_SET 4
#define UPPM_ACCESS_NET_ERROR 5
#define UPPM_SHA256_MISMATCH  6

#define UPPM_PACKAGE_IS_NOT_AVAILABLE 10
#define UPPM_PACKAGE_IS_NOT_INSTALLED 11
#define UPPM_PACKAGE_IS_NOT_OUTDATED  12

#define UPPM_FORMULA_REPO_NOT_EXIST  15
#define UPPM_REPOS_CONFIG_READ_ERROR 16

#define UPPM_FORMULA_FILE_OPEN_ERROR  20
#define UPPM_FORMULA_FILE_READ_ERROR  21

#define UPPM_INSTALLED_METADATA_FILE_OPEN_ERROR  30
#define UPPM_INSTALLED_METADATA_FILE_READ_ERROR  31

typedef struct {
    char * summary;
    char * webpage;
    char * version;
    char * license;
    char * git_url;
    char * bin_url;
    char * bin_sha;
    char * dep_pkg;
    char * install;
} UPPMFormula;

int  uppm_formula_parse (const char * formulaFilePath, UPPMFormula * * formula);
int  uppm_formula_path  (const char * pkgName, char * * out);
int  uppm_formula_cat   (const char * pkgName);
int  uppm_formula_bat   (const char * pkgName);
int  uppm_formula_edit  (const char * pkgName);
int  uppm_formula_create(const char * pkgName);
int  uppm_formula_delete(const char * pkgName);
int  uppm_formula_rename(const char * pkgName, char * newPkgName);

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

int uppm_main(int argc, char* argv[]);

int uppm_init();

int uppm_help();

int uppm_update_self();

int uppm_search(const char* keyword);

int uppm_info(const char* pkgName);

int uppm_tree(const char* pkgName);

int uppm_logs(const char* pkgName);

int uppm_pack(const char* pkgName, const char* type);

int uppm_deploy(const char* pkgName, const char* type);

int uppm_depends(const char* pkgName);

int uppm_fetch(const char* pkgName);

int uppm_install(const char* pkgName);

int uppm_reinstall(const char* pkgName);

int uppm_uninstall(const char* pkgName);

int uppm_upgrade(const char* pkgName);

int uppm_cleanup();

int uppm_is_package_available(const char* pkgName);
int uppm_is_package_installed(const char* pkgName);
int uppm_is_package_outdated (const char* pkgName);

int uppm_list_the_available_packages();
int uppm_list_the_installed_packages();
int uppm_list_the_outdated__packages();

#endif
