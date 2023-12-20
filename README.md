# uppm

Universal Prebuild Package Manager for Unix-like systems.

## Caveats

- This software is being actively developed. It's in beta stage and may not be stable. Some features are subject to change without notice.

- Please do NOT place your own files under `~/.uppm` directory, as `uppm` will change files under `~/.uppm` directory without notice.

## Build from source dependencies

|dependency|required?|purpose|
|----|---------|-------|
|[cmake](https://cmake.org/)|required |for generating `build.ninja`|
|[ninja](https://ninja-build.org/)|required |for doing jobs that read from `build.ninja`|
|[pkg-config>=0.18](https://www.freedesktop.org/wiki/Software/pkg-config/)|required|for finding libraries|
||||
|[jansson](https://github.com/akheron/jansson)|required|for parsing and creating JSON.|
|[libyaml](https://github.com/yaml/libyaml/)|required|for parsing formula files whose format is YAML.|
|[libgit2](https://libgit2.org/)|required|for updating formula repositories.|
|[libcurl](https://curl.se/)|required|for http requesting support.|
|[openssl](https://www.openssl.org/)|required|for https requesting support and SHA-256 sum checking support.|
|[libarchive](https://www.libarchive.org/)|required|for uncompressing .zip and .tar.* files.|
|[zlib](https://www.zlib.net/)|required|for compress and uncompress data.|
|[pcre2](https://www.pcre.org/)||for Regular Expressions support. only required on OpenBSD.|

### Build from source via [ppkg](https://github.com/leleliu008/ppkg)

```bash
ppkg install uppm
```

### Build from source via [xcpkg](https://github.com/leleliu008/xcpkg)

```bash
xcpkg install uppm
```

### Build from source using [vcpkg](https://github.com/microsoft/vcpkg)

```bash
# install g++ curl zip unzip tar git

git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh
export VCPKG_ROOT="$PWD/vcpkg"
export PATH="$VCPKG_ROOT:$PATH"

vcpkg install curl openssl libgit2 libarchive libyaml jansson

git clone --depth=1 --branch=c https://github.com/leleliu008/uppm
cd uppm

cmake -S . -B   build.d -G Ninja -DCMAKE_INSTALL_PREFIX=/usr/local -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake
cmake --build   build.d
cmake --install build.d
```

## Build from source using your system's default package manager

**[Ubuntu](https://ubuntu.com/)**

```bash
apt -y update
apt -y install git cmake ninja-build pkg-config gcc libcurl4 libcurl4-openssl-dev libgit2-dev libarchive-dev libyaml-dev libjansson-dev

git clone --depth=1 https://github.com/leleliu008/uppm
cd uppm

cmake -S . -B   build.d -G Ninja -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build   build.d
cmake --install build.d
```

**[Fedora](https://getfedora.org/)**

```bash
dnf -y update
dnf -y install git cmake ninja-build pkg-config gcc libcurl-devel libgit2-devel libarchive-devel libyaml-devel jansson-devel

git clone --depth=1 https://github.com/leleliu008/uppm
cd uppm

cmake -S . -B   build.d -G Ninja -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build   build.d
cmake --install build.d
```

**[ArchLinux](https://archlinux.org/)**

```bash
pacman -Syyuu --noconfirm
pacman -S     --noconfirm git cmake ninja pkg-config gcc curl openssl libgit2 libarchive libyaml jansson

git clone --depth=1 https://github.com/leleliu008/uppm
cd uppm

cmake -S . -B   build.d -G Ninja -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build   build.d
cmake --install build.d
```

**[AlpineLinux](https://www.alpinelinux.org/)**

```bash
apk add git cmake ninja pkgconf gcc libc-dev curl-dev openssl-dev libgit2-dev libarchive-dev yaml-dev jansson-dev

git clone --depth=1 https://github.com/leleliu008/uppm
cd uppm

cmake -S . -B   build.d -G Ninja -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build   build.d
cmake --install build.d
```

**[VoidLinux](https://voidlinux.org/)**

```bash
xbps-install -Suy xbps
xbps-install -Suy cmake ninja gcc pkg-config libcurl-devel libgit2-devel libarchive-devel libyaml-devel jansson-devel

git clone --depth=1 https://github.com/leleliu008/uppm
cd uppm

cmake -S . -B   build.d -G Ninja -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build   build.d
cmake --install build.d
```

**[Gentoo Linux](https://www.gentoo.org/)**

```bash
emerge dev-vcs/git cmake dev-util/ninja gcc pkg-config net-misc/curl dev-libs/libgit2 libarchive dev-libs/libyaml dev-libs/jansson

git clone --depth=1 https://github.com/leleliu008/uppm
cd uppm

cmake -S . -B   build.d -G Ninja -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build   build.d
cmake --install build.d
```

**[openSUSE](https://www.opensuse.org/)**

```bash
zypper update  -y  
zypper install -y git cmake ninja gcc pkg-config libcurl-devel libgit2-devel libarchive-devel libyaml-devel libjansson-devel

git clone --depth=1 https://github.com/leleliu008/uppm
cd uppm

cmake -S . -B   build.d -G Ninja -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build   build.d
cmake --install build.d
```

**[macOS](https://www.apple.com/macos/)**

```bash
brew update
brew install git cmake pkg-config ninja curl jansson libyaml libgit2 libarchive

git clone --depth=1 https://github.com/leleliu008/uppm
cd uppm

export PKG_CONFIG_PATH="/usr/local/lib/pkgconfig:/usr/local/opt/openssl@1.1/lib/pkgconfig:/usr/local/opt/curl/lib/pkgconfig:/usr/local/opt/libarchive/lib/pkgconfig"

CMAKE_EXE_LINKER_FLAGS='-L/usr/local/lib -L/usr/local/opt/openssl@1.1/lib -lssl -liconv -framework CoreFoundation -framework Security'
CMAKE_FIND_ROOT_PATH="$(brew --prefix openssl@1.1);$(brew --prefix curl);$(brew --prefix libarchive)"

cmake \
    -S . \
    -B build.d \
    -G Ninja \
    -DCMAKE_INSTALL_PREFIX=./output \
    -DCMAKE_EXE_LINKER_FLAGS="$CMAKE_EXE_LINKER_FLAGS" \
    -DCMAKE_FIND_ROOT_PATH="$CMAKE_FIND_ROOT_PATH"

cmake --build   build.d
cmake --install build.d
```

**[FreeBSD](https://www.freebsd.org/)** and **[DragonFlyBSD](https://www.dragonflybsd.org/)**

```bash
pkg install -y git cmake ninja pkgconf gcc curl openssl libgit2 libarchive libyaml jansson

git clone --depth=1 https://github.com/leleliu008/uppm
cd uppm

cmake -S . -B   build.d -G Ninja -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build   build.d
cmake --install build.d
```

**[OpenBSD](https://www.openbsd.org/)**

```bash
pkg_add git cmake ninja pkgconf llvm curl libgit2 libarchive libyaml jansson

git clone --depth=1 https://github.com/leleliu008/uppm
cd uppm

cmake -S . -B   build.d -G Ninja -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build   build.d
cmake --install build.d
```

**[NetBSD](https://www.netbsd.org/)**

```bash
pkgin -y install git mozilla-rootcerts cmake ninja-build pkg-config clang curl openssl libgit2 libarchive libyaml jansson

mozilla-rootcerts install

git clone --depth=1 https://github.com/leleliu008/uppm
cd uppm

cmake -S . -B   build.d -G Ninja -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build   build.d
cmake --install build.d
```

## uppm command usage

- **show help of this command**

    ```bash
    uppm -h
    uppm --help
    ```

- **show version of this command**

    ```bash
    uppm -V
    uppm --version
    ```

- **show basic information about this software**

    ```bash
    uppm env
    ```

- **show build information about this software**

    ```bash
    uppm buildinfo
    ```

- **show basic information about your current running operation system**

    ```bash
    uppm sysinfo
    ```

- **integrate `zsh-completion` script**

    ```bash
    uppm integrate zsh
    uppm integrate zsh --output-dir=/usr/local/share/zsh/site-functions
    uppm integrate zsh -v
    ```

    I provide a zsh-completion script for `uppm`. when you've typed `uppm` then type `TAB` key, the rest of the arguments will be automatically complete for you.

    **Note**: to apply this feature, you may need to run the command `autoload -U compinit && compinit` in your terminal (your current running shell must be zsh).

- **update all available formula repositories**

    ```bash
    uppm update
    ```

- **search all available packages whose name matches the given regular expression pattern**

    ```bash
    uppm search curl
    uppm search lib
    ```

- **show information of the given package**

    ```bash
    uppm info curl
    uppm info curl version
    uppm info curl summary
    uppm info curl webpage
    uppm info curl bin-url
    uppm info curl bin-sha
    uppm info curl bin-ft  # filetype of bin-url
    uppm info curl bin-fp  # filepath of bin-url
    uppm info curl install
    uppm info curl formula

    uppm info curl installed-dir
    uppm info curl installed-files
    uppm info curl installed-version
    uppm info curl installed-receipt-path
    uppm info curl installed-receipt-json
    uppm info curl installed-receipt-yaml
    uppm info curl installed-timestamp-unix
    uppm info curl installed-timestamp-iso-8601
    uppm info curl installed-timestamp-rfc-3339

    uppm info curl --json
    uppm info curl --json | jq .

    uppm info curl --yaml
    uppm info curl --yaml | yq .

    uppm info curl --shell

    uppm info @all

    uppm info @all --shell

    uppm info @all --json
    uppm info @all --json | jq .

    uppm info @all --yaml
    uppm info @all --yaml | yq .
    ```

- **show packages that are depended by the given package**

    ```bash
    uppm depends curl

    uppm depends curl -t dot
    uppm depends curl -t box
    uppm depends curl -t png
    uppm depends curl -t svg

    uppm depends curl -t dot -o .
    uppm depends curl -t box -o .
    uppm depends curl -t png -o .
    uppm depends curl -t svg -o .

    uppm depends curl -t dot -o a/
    uppm depends curl -t box -o a/
    uppm depends curl -t png -o a/
    uppm depends curl -t svg -o a/

    uppm depends curl -o xx.dot
    uppm depends curl -o xx.box
    uppm depends curl -o xx.png
    uppm depends curl -o xx.svg
    ```

- **download resources of the given package to the local cache**

    ```bash
    uppm fetch curl
    uppm fetch @all

    uppm fetch curl -v
    uppm fetch @all -v
    ```

- **install packages**

    ```bash
    uppm install curl
    uppm install curl bzip2 -v
    ```

- **reinstall packages**

    ```bash
    uppm reinstall curl
    uppm reinstall curl bzip2 -v
    ```

- **uninstall packages**

    ```bash
    uppm uninstall curl
    uppm uninstall curl bzip2 -v
    ```

- **upgrade the outdated packages**

    ```bash
    uppm upgrade
    uppm upgrade curl
    uppm upgrade curl bzip2 -v
    ```

- **upgrade this software**

    ```bash
    uppm upgrade-self
    uppm upgrade-self -v
    ```

- **view the formula of the given package**

    ```bash
    uppm formula-view curl
    uppm formula-view curl --no-color
    ```

- **edit the formula of the given package**

    ```bash
    uppm formula-edit curl
    uppm formula-edit curl --editor=/usr/local/bin/vim
    ```

- **list all avaliable formula repositories**

    ```bash
    uppm formula-repo-list
    ```

- **create a new empty formula repository**

    ```bash
    uppm formula-repo-init my_repo https://github.com/leleliu008/uppm-formula-repository-my_repo
    uppm formula-repo-init my_repo https://github.com/leleliu008/uppm-formula-repository-my_repo --branch=master
    uppm formula-repo-init my_repo https://github.com/leleliu008/uppm-formula-repository-my_repo --branch=main --pin
    uppm formula-repo-init my_repo https://github.com/leleliu008/uppm-formula-repository-my_repo --unpin --disable
    uppm formula-repo-init my_repo https://github.com/leleliu008/uppm-formula-repository-my_repo --enable
    ```

- **create a new empty formula repository then sync with server**

    ```bash
    uppm formula-repo-add my_repo https://github.com/leleliu008/uppm-formula-repository-my_repo
    uppm formula-repo-add my_repo https://github.com/leleliu008/uppm-formula-repository-my_repo --branch=master
    uppm formula-repo-add my_repo https://github.com/leleliu008/uppm-formula-repository-my_repo --branch=main --pin
    uppm formula-repo-add my_repo https://github.com/leleliu008/uppm-formula-repository-my_repo --unpin --disable
    uppm formula-repo-add my_repo https://github.com/leleliu008/uppm-formula-repository-my_repo --enable
    ```

- **delete the given formula repository**

    ```bash
    uppm formula-repo-del my_repo
    ```

- **sync the given formula repository with server**

    ```bash
    uppm formula-repo-sync my_repo
    ```

- **show information of the given formula repository**

    ```bash
    uppm formula-repo-info my_repo
    ```

- **change the config of the given formula repository**

    ```bash
    uppm formula-repo-conf my_repo --url=https://github.com/leleliu008/uppm-formula-repository-my_repo
    uppm formula-repo-conf my_repo --branch=main
    uppm formula-repo-conf my_repo --pin
    uppm formula-repo-conf my_repo --unpin
    uppm formula-repo-conf my_repo --enable
    uppm formula-repo-conf my_repo --disable
    ```

- **list all available packages**

    ```bash
    uppm ls-available
    ```

- **list all installed packages**

    ```bash
    uppm ls-installed
    ```

- **list all outdated packages**

    ```bash
    uppm ls-outdated
    ```

- **check if the given package is available ?**

    ```bash
    uppm is-available curl
    ```

- **check if the given package is installed ?**

    ```bash
    uppm is-installed curl
    ```

- **check if the given package is outdated ?**

    ```bash
    uppm is-outdated  curl
    ```

- **list the installed files of the given installed package in a tree-like format**

    ```bash
    uppm tree curl
    uppm tree curl -L 3
    ```

- **generate url-transform sample**

    ```bash
    uppm gen-url-transform-sample
    ```

- **extra common used utilities**

    ```bash
    uppm util zlib-deflate -L 6 < input/file/path
    uppm util zlib-inflate      < input/file/path

    uppm util base16-encode "string to be encoded with base16 algorithm"
    uppm util base16-encode < input/file/path

    uppm util base16-decode ABCD
    uppm util base16-decode ABCD > output/file/path

    uppm util base64-encode "string to be encoded with base64 algorithm"
    uppm util base64-encode < input/file/path

    uppm util base64-decode YQ==
    uppm util base64-decode YQ== > output/file/path

    uppm util sha256sum   input/file/path
    uppm util sha256sum < input/file/path

    uppm util which tree
    uppm util which tree -a
    ```

- **delete the unused cached files**

    ```bash
    uppm cleanup
    ```

## environment variables

- **HOME**

    This environment variable already have been set on most systems, if not set or set a empty string, you will receive an error message.

- **PATH**

    This environment variable already have been set on most systems, if not set or set a empty string, you will receive an error message.

- **SSL_CERT_FILE**

    ```bash
    curl -LO https://curl.se/ca/cacert.pem
    export SSL_CERT_FILE="$PWD/cacert.pem"
    ```

    In general, you don't need to set this environment variable, but, if you encounter the reporting `the SSL certificate is invalid`, trying to run above commands in your terminal will do the trick.

- **UPPM_HOME**

    If this environment variable is not set or set a empty string, `$HOME/.uppm` will be used as the default value.

    ```bash
    export UPPM_HOME=$HOME/uppm-home
    ```

- **UPPM_URL_TRANSFORM**

    ```bash
    export UPPM_URL_TRANSFORM=/path/of/url-transform
    ```

    `/path/of/url-transform` command would be invoked as `/path/of/url-transform <URL>`

    `/path/of/url-transform` command must output a `<URL>`

    you can generate a url-transform sample via `uppm gen-url-transform-sample`

    If you want to change the request url, you can set this environment variable. It is very useful for chinese users.

## uppm formula scheme

a uppm formula is a [YAML](https://yaml.org/spec/1.2.2/) format file which is used to config a uppm package's meta-information including one sentence description, package version, installation instructions, etc.

a uppm formula's filename suffix must be `.yml`

a uppm formula'a filename prefix would be treated as the package name.

a uppm formula'a filename prefix must match regular expression pattern `^[A-Za-z0-9+-._@]{1,50}$`

a uppm formula's file content only has one level mapping and shall has following KEY:

|KEY|required?|overview|
|-|-|-|
|`summary`|required|Describe this package in one sentence.|
|`webpage`|required|the home webpage url of this package.|
|`license`|optional|a space-separated list of [SPDX license short identifiers](https://spdx.github.io/spdx-spec/v2.3/SPDX-license-list/#a1-licenses-with-short-identifiers)|
|`version`|optional|the version of this package.<br>If this mapping is not present, it will be calculated from `bin-url`|
|`bin-url`|required|the prebuild binary file download url of this package.|
|`bin-sha`|required|the `sha256sum` of the prebuild binary file.|
|`dep-pkg`|optional|a space-separated list of package names. these packages will be used when installing or runtime.|
|`unpackd`|optional|relative to `$PKG_INSTALL_DIR`. the directory where shall be unpacked to. default is empty, which means that the artifact will be unpacked to `$PKG_INSTALL_DIR`|
|`install`|optional|POSIX shell code to be run when installing.<br>If this mapping is not present, and if `bin-url` mapping's value ends with one of `.zip` `.tar.xz` `.tar.gz` `.tar.lz` `.tar.bz2` `.tgz` `.txz` `.tlz` `.tbz2`, `uppm` will uncompress it to `$PKG_INSTALL_DIR` for you. otherwise, just copy it to `$PKG_INSTALL_DIR`|

**shell variables that can be used in install block :**

|variable|overview|
|-|-|
|`NATIVE_OS_ARCH`|current machine os arch.|
|`NATIVE_OS_KIND`|current machine os kind.|
|`NATIVE_OS_TYPE`|current machine os type.|
|`NATIVE_OS_NAME`|current machine os name.|
|`NATIVE_OS_VERS`|current machine os version.|
|`NATIVE_OS_NCPU`|current machine os has how many cpu cores.|
|||
|`UPPM_VERSION`|the version of `uppm`.|
|`UPPM_VERSION_MAJOR`|the major part of `$UPPM_VERSION`.|
|`UPPM_VERSION_MINOR`|the minor part of `$UPPM_VERSION`.|
|`UPPM_VERSION_PATCH`|the patch part of `$UPPM_VERSION`.|
|`UPPM_HOME`|the home directory of `uppm`.|
|`UPPM`|the executbale path of `uppm`.|
|||
|`PKG_SUMMARY`|the value of `summary`|
|`PKG_VERSION`|the value of `version`|
|`PKG_LICENSE`|the value of `license`|
|`PKG_WEBPAGE`|the value of `webpage`|
|`PKG_BIN_URL`|the value of `bin-url`|
|`PKG_BIN_SHA`|the value of `bin-sha`|
|`PKG_BIN_FILEPATH`|the local file path of `bin-url`|
|`PKG_BIN_FILENAME`|the local file name of `bin-url`|
|`PKG_BIN_FILETYPE`|the file type of `bin-url`|
|`PKG_DEP_PKG`|the value of `dep-pkg`|
|`PKG_INSTALL_DIR`|the directory where the current package will be installed to.|

## uppm formula repository

a uppm formula repository is a git repository.

a uppm formula repository's root directory should have a `formula` named sub directory, this repository's formulas all should be located in this directory.

a uppm formula repository's local path is `~/.uppm/repos.d/${UPPMFormulaRepoName}`

**Note:** uppm supports multiple formula repositories.

## uppm formula repository's config

After a uppm formula repository is successfully fetched from server to local, a config file for this repository would be created at `~/.uppm/repos.d/${UPPMFormulaRepoName}/.uppm-formula-repo.yml`

a typical uppm formula repository's config as following:

```yml
url: https://github.com/leleliu008/uppm-formula-repository-linux-x86_64
branch: master
pinned: 0
enabled: 1
timestamp-created: 1673684639
timestamp-updated: 1673684767
```

If a uppm formula repository is `pinned`, which means it would not be updated.

If a uppm formula repository is `disabled`, which means uppm would not search formulas in this formula repository.

## uppm offical formula repository

uppm offical formula repository's url:

- <https://github.com/leleliu008/uppm-formula-repository-android-aarch64>
- <https://github.com/leleliu008/uppm-formula-repository-linux-x86_64>
- <https://github.com/leleliu008/uppm-formula-repository-linux-aarch64>
- <https://github.com/leleliu008/uppm-formula-repository-macos-x86_64>
- <https://github.com/leleliu008/uppm-formula-repository-macos-arm64>
- <https://github.com/leleliu008/uppm-formula-repository-freebsd-amd64>
- <https://github.com/leleliu008/uppm-formula-repository-openbsd-amd64>
- <https://github.com/leleliu008/uppm-formula-repository-netbsd-amd64>

uppm offical formula repository would be automatically fetched to local cache as name `offical-core` when you run `uppm update` command.

**Note:** If you find that a package is not in uppm offical formula repository yet, PR is welcomed.
