# uppm [Work In Progress]
Universal Prebuild Package Manager for Unix-like systems.

## Install uppm via [HomeBrew](https://brew.sh/)
```bash
brew tap leleliu008/fpliu
brew install uppm
```

## Install uppm via [uppm](https://github.com/leleliu008/uppm)
```bash
uppm install uppm
```

## Build from source
|dependency|required?|purpose|
|----|---------|-------|
|[cmake](https://cmake.org/)|required |for generating `build.ninja`|
|[ninja](https://ninja-build.org/)|required |for doing jobs that read from `build.ninja`|
|[pkg-config>=0.18](https://www.freedesktop.org/wiki/Software/pkg-config/)|required|for finding libraries|
||||
|[jansson](https://github.com/akheron/jansson)|required|for parsing and creating JSON.|
|[libyaml](https://github.com/yaml/libyaml/)|required|for parsing formulas whose format is YAML.|
|[libgit2](https://libgit2.org/)|required|for updating formula repositories.|
|[libcurl](https://curl.se/)|required|for http requesting support.|
|[openssl](https://www.openssl.org/)|required|for https requesting support and SHA-256 sum checking support.|
|[libarchive](https://www.libarchive.org/)|required|for uncompressing .zip and .tar.* files.|
|[zlib](https://www.zlib.net/)|required|for compress and uncompress data.|
|[pcre2](https://www.pcre.org/)||for Regular Expressions support. only required on OpenBSD.|


**[vcpkg](https://github.com/microsoft/vcpkg)**
```bash
# install g++ curl zip unzip tar git

git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh
./vcpkg install curl openssl libgit2 libarchive libyaml jansson zlib

cd -

git clone https://github.com/leleliu008/uppm
cd uppm

cmake -S . -B   build.d -G Ninja -DCMAKE_INSTALL_PREFIX=/usr/local -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake
cmake --build   build.d
cmake --install build.d
```

**[Ubuntu](https://ubuntu.com/)**

```bash
apt -y update
apt -y install git cmake ninja-build pkg-config gcc libcurl4 libcurl4-openssl-dev libgit2-dev libarchive-dev libyaml-dev libjansson-dev

git clone https://github.com/leleliu008/uppm
cd uppm

cmake -S . -B   build.d -G Ninja -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build   build.d
cmake --install build.d
```

**[Fedora](https://getfedora.org/)**

```bash
dnf -y update
dnf -y install git cmake ninja-build pkg-config gcc libcurl-devel libgit2-devel libarchive-devel libyaml-devel jansson-devel

git clone https://github.com/leleliu008/uppm
cd uppm

cmake -S . -B   build.d -G Ninja -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build   build.d
cmake --install build.d
```

**[ArchLinux](https://archlinux.org/)**

```bash
pacman -Syyuu --noconfirm
pacman -S     --noconfirm git cmake ninja pkg-config gcc curl openssl libgit2 libarchive libyaml jansson

git clone https://github.com/leleliu008/uppm
cd uppm

cmake -S . -B   build.d -G Ninja -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build   build.d
cmake --install build.d
```

**[AlpineLinux](https://www.alpinelinux.org/)**

```bash
apk add git cmake ninja pkgconf gcc libc-dev curl-dev openssl-dev libgit2-dev libarchive-dev yaml-dev jansson-dev

git clone https://github.com/leleliu008/uppm
cd uppm

cmake -S . -B   build.d -G Ninja -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build   build.d
cmake --install build.d
```

**[VoidLinux](https://voidlinux.org/)**

```bash
xbps-install -Suy xbps
xbps-install -Suy cmake ninja gcc pkg-config libcurl-devel libgit2-devel libarchive-devel libyaml-devel jansson-devel

git clone https://github.com/leleliu008/uppm
cd uppm

cmake -S . -B   build.d -G Ninja -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build   build.d
cmake --install build.d
```

**[Gentoo Linux](https://www.gentoo.org/)**

```bash
emerge dev-vcs/git cmake dev-util/ninja gcc pkg-config net-misc/curl dev-libs/libgit2 libarchive dev-libs/libyaml dev-libs/jansson

git clone https://github.com/leleliu008/uppm
cd uppm

cmake -S . -B   build.d -G Ninja -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build   build.d
cmake --install build.d
```

**[openSUSE](https://www.opensuse.org/)**

```bash
zypper update  -y  
zypper install -y git cmake ninja gcc pkg-config libcurl-devel libgit2-devel libarchive-devel libyaml-devel libjansson-devel

git clone https://github.com/leleliu008/uppm
cd uppm

cmake -S . -B   build.d -G Ninja -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build   build.d
cmake --install build.d
```

**[macOS](https://www.apple.com/macos/)**

```bash
brew update
brew install git cmake pkg-config ninja curl jansson libyaml libgit2 libarchive

git clone https://github.com/leleliu008/uppm
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

**[FreeBSD](https://www.freebsd.org/)**

```bash
pkg install -y git cmake ninja pkgconf gcc curl openssl libgit2 libarchive libyaml jansson

git clone https://github.com/leleliu008/uppm
cd uppm

cmake -S . -B   build.d -G Ninja -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build   build.d
cmake --install build.d
```

**[OpenBSD](https://www.openbsd.org/)**

```bash
pkg_add git cmake ninja pkgconf llvm curl libgit2 libarchive libyaml jansson

git clone https://github.com/leleliu008/uppm
cd uppm

cmake -S . -B   build.d -G Ninja -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build   build.d
cmake --install build.d
```

**[NetBSD](https://www.netbsd.org/)**

```bash
pkgin -y install git mozilla-rootcerts cmake ninja-build pkg-config clang curl openssl libgit2 libarchive libyaml jansson

mozilla-rootcerts install

git clone https://github.com/leleliu008/uppm
cd uppm

cmake -S . -B   build.d -G Ninja -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build   build.d
cmake --install build.d
```


## ~/.uppm
all relevant dirs and files are located in `~/.uppm` directory.


## uppm command usage
*   **show help of this command**
        
        uppm -h
        uppm --help
        
*   **show version of this command**

        uppm -V
        uppm --version
        
*   **show current machine os info**

        uppm env
        
*   **integrate `zsh-completion` script**

        uppm integrate zsh
        uppm integrate zsh --output-dir=/usr/local/share/zsh/site-functions
        uppm integrate zsh -v
        
    I have provide a zsh-completion script for `uppm`. when you've typed `uppm` then type `TAB` key, it will auto complete the rest for you.

    **Note**: to apply this feature, you may need to run the command `autoload -U compinit && compinit`

*   **update formula repositories**

        uppm update
        
*   **search packages**
        
        uppm search curl
        uppm search lib
        
*   **show infomation of the given package**
        
        uppm info curl
        uppm info curl version
        uppm info curl summary
        uppm info curl webpage
        uppm info curl bin-url
        uppm info curl bin-sha
        uppm info curl install
        uppm info curl formula

        uppm info curl installed-dir
        uppm info curl installed-files
        uppm info curl installed-receipt-path
        uppm info curl installed-receipt-json
        uppm info curl installed-receipt-yaml
        uppm info curl installed-timestamp-unix
        uppm info curl installed-timestamp-iso-8601
        uppm info curl installed-timestamp-rfc-3339
        uppm info curl installed-version

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
        
*   **show packages that are depended by the given package**
        
        uppm depends curl

        uppm depends curl --format=dot
        uppm depends curl --format=box
        uppm depends curl --format=png
        uppm depends curl --format=svg

        uppm depends curl --format=dot > xx.dot
        uppm depends curl --format=box > xx.txt
        uppm depends curl --format=png > xx.png
        uppm depends curl --format=svg > xx.svg
        
*   **download formula resources of the given package to the local cache**
        
        uppm fetch curl
        uppm fetch @all

        uppm fetch curl -v
        uppm fetch @all -v

*   **install packages**
        
        uppm install curl
        uppm install curl bzip2 -v
        
*   **reinstall packages**
        
        uppm reinstall curl
        uppm reinstall curl bzip2 -v
        
*   **uninstall packages**

        uppm uninstall curl
        uppm uninstall curl bzip2 -v
        
*   **upgrade the outdated packages**

        uppm upgrade
        uppm upgrade curl
        uppm upgrade curl bzip2 -v
        
*   **upgrade this software**

        uppm upgrade-self
        uppm upgrade-self -v
        
*   **list the avaliable formula repositories**

        uppm formula-repo-list

*   **add a new formula repository**

        uppm formula-repo-add my_repo https://github.com/leleliu008/uppm-formula-repository-my_repo
        uppm formula-repo-add my_repo https://github.com/leleliu008/uppm-formula-repository-my_repo master
        uppm formula-repo-add my_repo https://github.com/leleliu008/uppm-formula-repository-my_repo main
        
    **Note:** this software supports multiple formula repositories.

*   **delete a existing formula repository**

        uppm formula-repo-del my_repo

*   **list the available packages**
        
        uppm ls-available
        
*   **list the installed packages**
        
        uppm ls-installed
        
*   **list the outdated packages**
        
        uppm ls-outdated
        
*   **is the given package available ?**
        
        uppm is-available curl
        
*   **is the given package installed ?**
        
        uppm is-installed curl
        
*   **is the given package outdated ?**
        
        uppm is-outdated  curl
        
*   **list files of the given installed package in a tree-like format**
        
        uppm tree curl
        uppm tree curl -L 3
        
*   **cleanup the unused cached files**
        
        uppm cleanup
        

## influential environment variables

*   **HOME**

    this environment variable already have been set on most systems, if not set or set a empty string, you will receive an error message.

*   **PATH**

    this environment variable already have been set on most systems, if not set or set a empty string, you will receive an error message.

*   **SSL_CERT_FILE**

    ```bash
    curl -LO https://curl.se/ca/cacert.pem
    export SSL_CERT_FILE="$PWD/cacert.pem"
    ```

    In general, you don't need to set this environment variable, but, if you encounter the reporting `the SSL certificate is invalid`, trying to run above commands in your terminal will do the trick.

*   **UPPM_URL_TRANSFORM**

    ```bash
    export UPPM_URL_TRANSFORM=/path/of/url-transform
    ```

    `/path/of/url-transform` command would be invoked as `/path/of/url-transform <URL>`

    `/path/of/url-transform` command must output a `<URL>`

    following is a example of `/path/of/url-transform` command implementation:

    ```bash
    #!/bin/sh

    case $1 in
        *githubusercontent.com/*)
            printf 'https://ghproxy.com/%s\n' "$1"
            ;;
        https://github.com/*)
            printf 'https://ghproxy.com/%s\n' "$1"
            ;;
        '') printf '%s\n' 'url-transform <URL>, <URL> is not given.' >&2 ;;
        *)  printf '%s\n' "$1"
    esac
    ```

    If you want to change the request url, you can set this environment variable. It is very useful for chinese users.

## uppm formula

a uppm formula is a [YAML](https://yaml.org/spec/1.2.2/) format file which is used to config a uppm package's meta-infomation including one sentence description, package version, installation instructions, etc.

a uppm formula's filename suffix must be `.yml`

a uppm formula'a filename prefix would be treated as the package name.

a uppm formula'a filename prefix must match regular expression partten `^[A-Za-z0-9+-._]{1,50}$`

a uppm formula's file content must follow [the uppm formula scheme](https://github.com/leleliu008/uppm-formula-repository-linux-x86_64)

## uppm formula repository
a uppm formula repository is a git repository.

a uppm formula repository's root dir should have a `formula` named sub dir, this repository's formulas all should be located in this dir.

a uppm formula repository's local path is `~/.uppm/repos.d/${UPPMFormulaRepoName}`

After a uppm formula repository is successfully fetched from server to local, a config file for this repository would be create, this config file path is `~/.uppm/repos.d/${UPPMFormulaRepoName}/.uppm-formula-repo.dat`, this config file is zlib deflated, and you're able to uncompress it via `zlib-flate -uncompress < ~/.uppm/repos.d/${UPPMFormulaRepoName}/.uppm-formula-repo.dat`.

a typical uppm formula repository's uncompressed config as following:

```
url: https://github.com/leleliu008/uppm-formula-repository-linux-x86_64
branch: master
pinned: no
enabled: yes
timestamp-added: 1673684639
timestamp-last-updated: 1673684767
```

If a uppm formula repository is pinned, which means it would not be updated.

If a uppm formula repository is disabled, which means uppm would not search formula in this formula repository.

**Note:**
 - please do NOT directly modify the formulas since your changes may be lost after the formula repository is updated!
 - uppm supports multiple formula repositories.


## uppm offical formula repository

There is a special uppm formula repository whose name is `offical-core`.

`offical-core` formula repository's url:
- https://github.com/leleliu008/uppm-formula-repository-android-aarch64
- https://github.com/leleliu008/uppm-formula-repository-linux-x86_64
- https://github.com/leleliu008/uppm-formula-repository-linux-aarch64
- https://github.com/leleliu008/uppm-formula-repository-macos-x86_64
- https://github.com/leleliu008/uppm-formula-repository-macos-arm64
- https://github.com/leleliu008/uppm-formula-repository-freebsd-amd64
- https://github.com/leleliu008/uppm-formula-repository-openbsd-amd64
- https://github.com/leleliu008/uppm-formula-repository-netbsd-amd64

If you find that a package is not in `offical-core` formula repository yet, PR is welcomed.

