#include <unistd.h>
#include "core/log.h"

int uppm_help() {
    if (isatty(STDOUT_FILENO)) {
        const char * str = ""
        COLOR_GREEN
        "uppm (Universal Prebuild Package Manager)\n\n"
        "uppm <ACTION> [ARGUMENT...]\n\n"
        "uppm --help\n"
        "uppm -h\n"
        COLOR_OFF
        "    show help of this command.\n\n"
        COLOR_GREEN
        "uppm --version\n"
        "uppm -V\n"
        COLOR_OFF
        "    show version of this command.\n\n"
        COLOR_GREEN
        "uppm env\n"
        COLOR_OFF
        "    show basic information about this software.\n\n"
        COLOR_GREEN
        "uppm buildinfo\n"
        COLOR_OFF
        "    show build information about this software.\n\n"
        COLOR_GREEN
        "uppm sysinfo\n"
        COLOR_OFF
        "    show basic information about your current running operation system.\n\n"
        COLOR_GREEN
        "uppm integrate zsh [-v] [--output-dir=<DIR>]\n"
        COLOR_OFF
        "    download a zsh completion script file to a approprivate location.\n\n"
        "    to apply this feature, you may need to run the command 'autoload -U compinit && compinit' in your terminal (your current running shell must be zsh).\n\n"
        COLOR_GREEN
        "uppm update\n"
        COLOR_OFF
        "    update all available formula repositories.\n\n"
        COLOR_GREEN
        "uppm search  <REGULAR-EXPRESSION>\n"
        COLOR_OFF
        "    search all available packages whose name matches the given regular expression pattern.\n\n"
        COLOR_GREEN
        "uppm info <PACKAGE-NAME|@all>\n"
        COLOR_OFF
        "    show information of the given package.\n\n"
        COLOR_GREEN
        "uppm tree <PACKAGE-NAME> [--dirsfirst | -L N]\n"
        COLOR_OFF
        "    list files of the given installed package in a tree-like format.\n\n"
        COLOR_GREEN
        "uppm depends <PACKAGE-NAME> [-t <OUTPUT-TYPE>] [-o <OUTPUT-PATH>]\n"
        COLOR_OFF
        "    show packages that are depended by the given package.\n\n"
        "    <OUTPUT-TYPE> must be one of <dot|box|svg|png>\n\n"
        "    If -o <OUTPUT-PATH> option is given, the result will be written to file, otherwize, the result will be written to stdout.\n\n"
        "   <OUTPUT-PATH> can be either the filepath or directory. If it is an existing directory or ends with slash, then it will be treated as a directory, otherwize, it will be treated as a filepath.\n\n"
        "    If <OUTPUT-PATH> is treated as a directory, then it will be expanded to <OUTPUT-PATH>/<PACKAGE-NAME>-dependencies.<OUTPUT-TYPE>\n\n"
        "    If <OUTPUT-PATH> is treated as a filepath, and if -t <OUTPUT-TYPE> option is not given, if <OUTPUT-PATH> ends with one of .dot|.box|.svg|.png, <OUTPUT-TYPE> will be the <OUTPUT-PATH> suffix, otherwize, <OUTPUT-TYPE> will be box.\n\n"
        "    If -t <OUTPUT-TYPE> and -o <OUTPUT-PATH> options both are not given, <OUTPU-TYPE> will be box and output to stdout.\n\n"
        COLOR_GREEN
        "uppm fetch   <PACKAGE-NAME|@all>\n"
        COLOR_OFF
        "    download the prebuild binary archive file of the given package to the local cache.\n\n"
        COLOR_GREEN
        "uppm   install <PACKAGE-NAME>\n"
        COLOR_OFF
        "    install the given packages.\n\n"
        COLOR_GREEN
        "uppm   upgrade <PACKAGE-NAME>\n"
        COLOR_OFF
        "    upgrade the given packages or all outdated packages.\n\n"
        COLOR_GREEN
        "uppm reinstall <PACKAGE-NAME>\n"
        COLOR_OFF
        "    reinstall the given packages.\n\n"
        COLOR_GREEN
        "uppm uninstall <PACKAGE-NAME>\n"
        COLOR_OFF
        "    uninstall the given packages.\n\n"
        COLOR_GREEN
        "uppm ls-available\n"
        COLOR_OFF
        "    list the available packages.\n\n"
        COLOR_GREEN
        "uppm ls-installed\n"
        COLOR_OFF
        "    list the installed packages.\n\n"
        COLOR_GREEN
        "uppm ls-outdated\n"
        COLOR_OFF
        "    list the outdated  packages.\n\n"
        COLOR_GREEN
        "uppm is-available <PACKAGE-NAME>\n"
        COLOR_OFF
        "    check if the given package is available.\n\n"
        COLOR_GREEN
        "uppm is-installed <PACKAGE-NAME>\n"
        COLOR_OFF
        "    check if the given package is installed.\n\n"
        COLOR_GREEN
        "uppm is-outdated  <PACKAGE-NAME>\n"
        COLOR_OFF
        "    check if the given package is outdated.\n\n"
        COLOR_GREEN
        "uppm formula-view <PACKAGE-NAME> [--no-color]\n"
        COLOR_OFF
        "    view the formula of the given package.\n\n"
        COLOR_GREEN
        "uppm formula-edit <PACKAGE-NAME> [--editor=EDITOR]\n"
        COLOR_OFF
        "    edit the formula of the given package.\n\n"
        COLOR_GREEN
        "uppm formula-repo-list\n"
        COLOR_OFF
        "    list all available formula repositories.\n\n"
        COLOR_GREEN
        "uppm formula-repo-add  <FORMULA-REPO-NAME> <FORMULA-REPO-URL> [--branch=VALUE --pin/--unpin --enable/--disable]\n"
        COLOR_OFF
        "    create a new empty formula repository then sync with server.\n\n"
        COLOR_GREEN
        "uppm formula-repo-del  <FORMULA-REPO-NAME>\n"
        COLOR_OFF
        "    delete the given formula repository.\n\n"
        COLOR_GREEN
        "uppm formula-repo-init <FORMULA-REPO-NAME> <FORMULA-REPO-URL> [--branch=VALUE --pin/--unpin --enable/--disable]\n"
        COLOR_OFF
        "    create a new empty formula repository.\n\n"
        COLOR_GREEN
        "uppm formula-repo-conf <FORMULA-REPO-NAME>      [--url=VALUE | --branch=VALUE --pin/--unpin --enable/--disable]\n"
        COLOR_OFF
        "    change the config of the given formula repository.\n\n"
        COLOR_GREEN
        "uppm formula-repo-sync <FORMULA-REPO-NAME>\n"
        COLOR_OFF
        "    sync the given formula repository with server.\n\n"
        COLOR_GREEN
        "uppm formula-repo-info <FORMULA-REPO-NAME>\n"
        COLOR_OFF
        "    show infomation of the given formula repository.\n\n"
        COLOR_GREEN
        "uppm gen-url-transform-sample\n"
        COLOR_OFF
        "    generate url-transform sample.\n\n"
        COLOR_GREEN
        "uppm cleanup\n"
        COLOR_OFF
        "    cleanup the unused cache.\n\n\n"
        COLOR_GREEN
        "uppm util zlib-deflate -L <LEVEL> < input/file/path\n"
        COLOR_OFF
        "    compress data using zlib deflate algorithm.\n\n"
        "    LEVEL >= 1 && LEVEL <= 9\n\n"
        "    The smaller the LEVEL, the faster the speed and the lower the compression ratio.\n\n"
        COLOR_GREEN
        "uppm util zlib-inflate < input/file/path\n"
        COLOR_OFF
        "    decompress data using zlib inflate algorithm.\n\n"
        COLOR_GREEN
        "uppm util base16-encode <STR>\n"
        COLOR_OFF
        "    encode <STR> using base16 algorithm.\n\n"
        COLOR_GREEN
        "uppm util base16-encode < input/file/path\n"
        COLOR_OFF
        "    encode data using base16 algorithm.\n\n"
        COLOR_GREEN
        "uppm util base16-decode <BASE16-ENCODED-SUM>\n"
        COLOR_OFF
        "    decode <BASE16-ENCODED-SUM> using base16 algorithm.\n\n"
        COLOR_GREEN
        "uppm util base16-decode < input/file/path\n"
        COLOR_OFF
        "    decode data using base16 algorithm.\n\n"
        COLOR_GREEN
        "uppm util base64-encode <STR>\n"
        COLOR_OFF
        "    encode <STR> using base64 algorithm.\n\n"
        COLOR_GREEN
        "uppm util base64-encode < input/file/path\n"
        COLOR_OFF
        "    encode data using base64 algorithm.\n\n"
        COLOR_GREEN
        "uppm util base64-decode <BASE64-ENCODED-SUM>\n"
        COLOR_OFF
        "    decode <BASE64-ENCODED-SUM> using base64 algorithm.\n\n"
        COLOR_GREEN
        "uppm util base64-decode < input/file/path\n"
        COLOR_OFF
        "    decode data using base64 algorithm.\n\n"
        COLOR_GREEN
        "uppm util sha256sum <input/file/path>\n"
        COLOR_OFF
        "    calculate sha256sum of file.\n\n"
        COLOR_GREEN
        "uppm util sha256sum < input/file/path\n"
        COLOR_OFF
        "    calculate sha256sum of file.\n\n"
        COLOR_GREEN
        "uppm util which <COMMAND-NAME> [-a]\n"
        COLOR_OFF
        "    find <COMMAND-NAME> in PATH.\n"
        ;

        printf("%s\n", str);
    } else {
        const char * str = ""
        "uppm (Universal Prebuild Package Manager)\n\n"
        "uppm <ACTION> [ARGUMENT...]\n\n"
        "uppm --help\n"
        "uppm -h\n"
        "    show help of this command.\n\n"
        "uppm --version\n"
        "uppm -V\n"
        "    show version of this command.\n\n"
        "uppm env\n"
        "    show basic information about this software.\n\n"
        "uppm buildinfo\n"
        "    show build information about this software.\n\n"
        "uppm sysinfo\n"
        "    show basic information about your current running operation system.\n\n"
        "uppm integrate zsh [-v] [--output-dir=<DIR>]\n"
        "    download a zsh completion script file to a approprivate location.\n\n"
        "    to apply this feature, you may need to run the command 'autoload -U compinit && compinit' in your terminal (your current running shell must be zsh).\n\n"
        "uppm update\n"
        "    update all available formula repositories.\n\n"
        "uppm search  <REGULAR-EXPRESSION>\n"
        "    search all available packages whose name matches the given regular expression pattern.\n\n"
        "uppm info <PACKAGE-NAME|@all>\n"
        "    show information of the given package.\n\n"
        "uppm tree <PACKAGE-NAME> [--dirsfirst | -L N]\n"
        "    list files of the given installed package in a tree-like format.\n\n"
        "uppm depends <PACKAGE-NAME> [-t <OUTPUT-TYPE>] [-o <OUTPUT-PATH>]\n"
        "    show packages that are depended by the given package.\n\n"
        "    <OUTPUT-TYPE> must be one of <dot|box|svg|png>\n\n"
        "    If -o <OUTPUT-PATH> option is given, the result will be written to file, otherwize, the result will be written to stdout.\n\n"
        "   <OUTPUT-PATH> can be either the filepath or directory. If it is an existing directory or ends with slash, then it will be treated as a directory, otherwize, it will be treated as a filepath.\n\n"
        "    If <OUTPUT-PATH> is treated as a directory, then it will be expanded to <OUTPUT-PATH>/<PACKAGE-NAME>-dependencies.<OUTPUT-TYPE>\n\n"
        "    If <OUTPUT-PATH> is treated as a filepath, and if -t <OUTPUT-TYPE> option is not given, if <OUTPUT-PATH> ends with one of .dot|.box|.svg|.png, <OUTPUT-TYPE> will be the <OUTPUT-PATH> suffix, otherwize, <OUTPUT-TYPE> will be box.\n\n"
        "    If -t <OUTPUT-TYPE> and -o <OUTPUT-PATH> options both are not given, <OUTPU-TYPE> will be box and output to stdout.\n\n"
        "uppm fetch   <PACKAGE-NAME|@all>\n"
        "    download the prebuild binary archive file of the given package to the local cache.\n\n"
        "uppm   install <PACKAGE-NAME>\n"
        "    install the given packages.\n\n"
        "uppm   upgrade <PACKAGE-NAME>\n"
        "    upgrade the given packages or all outdated packages.\n\n"
        "uppm reinstall <PACKAGE-NAME>\n"
        "    reinstall the given packages.\n\n"
        "uppm uninstall <PACKAGE-NAME>\n"
        "    uninstall the given packages.\n\n"
        "uppm ls-available\n"
        "    list the available packages.\n\n"
        "uppm ls-installed\n"
        "    list the installed packages.\n\n"
        "uppm ls-outdated\n"
        "    list the outdated  packages.\n\n"
        "uppm is-available <PACKAGE-NAME>\n"
        "    check if the given package is available.\n\n"
        "uppm is-installed <PACKAGE-NAME>\n"
        "    check if the given package is installed.\n\n"
        "uppm is-outdated  <PACKAGE-NAME>\n"
        "    check if the given package is outdated.\n\n"
        "uppm formula-view <PACKAGE-NAME> [--no-color]\n"
        "    view the formula of the given package.\n\n"
        "uppm formula-edit <PACKAGE-NAME> [--editor=EDITOR]\n"
        "    edit the formula of the given package.\n\n"
        "uppm formula-repo-add  <FORMULA-REPO-NAME> <FORMULA-REPO-URL> [--branch=VALUE --pin/--unpin --enable/--disable]\n"
        "    create a new empty formula repository then sync with server.\n\n"
        "uppm formula-repo-del <FORMULA-REPO-NAME>\n"
        "    delete the given formula repository.\n\n"
        "uppm formula-repo-init <FORMULA-REPO-NAME> <FORMULA-REPO-URL> [--branch=VALUE --pin/--unpin --enable/--disable]\n"
        "    create a new empty formula repository.\n\n"
        "uppm formula-repo-conf <FORMULA-REPO-NAME>      [--url=VALUE | --branch=VALUE --pin/--unpin --enable/--disable]\n"
        "    change the config of the given formula repository.\n\n"
        "uppm formula-repo-sync <FORMULA-REPO-NAME>\n"
        "    sync the given formula repository with server.\n\n"
        "uppm formula-repo-info <FORMULA-REPO-NAME>\n"
        "    show infomation of the given formula repository.\n\n"
        "uppm formula-repo-list\n"
        "    list all available formula repositories.\n\n"
        "uppm gen-url-transform-sample\n"
        "    generate url-transform sample.\n\n"
        "uppm cleanup\n"
        "    cleanup the unused cache.\n"
        "uppm util zlib-deflate -L <LEVEL> < input/file/path\n"
        "    compress data using zlib deflate algorithm.\n\n"
        "    LEVEL >= 1 && LEVEL <= 9\n\n"
        "    The smaller the LEVEL, the faster the speed and the lower the compression ratio.\n\n"
        "uppm util zlib-inflate < input/file/path\n"
        "    decompress data using zlib inflate algorithm.\n\n"
        "uppm util base16-encode <STR>\n"
        "    encode <STR> using base16 algorithm.\n\n"
        "uppm util base16-encode < input/file/path\n"
        "    encode data using base16 algorithm.\n\n"
        "uppm util base16-decode <BASE16-ENCODED-SUM>\n"
        "    decode <BASE16-ENCODED-SUM> using base16 algorithm.\n\n"
        "uppm util base16-decode < input/file/path\n"
        "    decode data using base16 algorithm.\n\n"
        "uppm util base64-encode <STR>\n"
        "    encode <STR> using base64 algorithm.\n\n"
        "uppm util base64-encode < input/file/path\n"
        "    encode data using base64 algorithm.\n\n"
        "uppm util base64-decode <BASE64-ENCODED-SUM>\n"
        "    decode <BASE64-ENCODED-SUM> using base64 algorithm.\n\n"
        "uppm util base64-decode < input/file/path\n"
        "    decode data using base64 algorithm.\n\n"
        "uppm util sha256sum <input/file/path>\n"
        "    calculate sha256sum of file.\n\n"
        "uppm util sha256sum < input/file/path\n"
        "    calculate sha256sum of file.\n\n"
        "uppm util which <COMMAND-NAME> [-a]\n"
        "    find <COMMAND-NAME> in PATH.\n"
        ;

        printf("%s\n", str);
    }

    return 0;
}
