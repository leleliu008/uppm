# uppm [Work In Progress]
Universal Prebuild Package Manager for Unix-like systems.

## Install uppm via [HomeBrew](https://brew.sh/)
```bash
brew tap leleliu008/fpliu
brew install uppm
```

## Install uppm via [ppkg](https://github.com/leleliu008/ppkg)
```bash
ppkg install uppm
```

## Build from source
|dependency|required?|purpose|
|----|---------|-------|
|[cmake](https://cmake.org/)|required |for generating `build.ninja`|
|[ninja](https://ninja-build.org/)|required |for doing jobs that read from `build.ninja`|
|[pkg-config>=0.18](https://www.freedesktop.org/wiki/Software/pkg-config/)|recommended|for finding libraries|
||||
|[libyaml](https://github.com/yaml/libyaml/)|required|for parsing formula files.|
|[libgit2](https://libgit2.org/)|required|for updating formula repositories.|
|[libcurl](https://curl.se/)|required|for http requesting support.|
|[openssl](https://www.openssl.org/)|required|for https requesting support and SHA-256 sum checking support.|
|[libarchive](https://www.libarchive.org/)|required|for uncompressing .zip and .tar.* files.|
|[liblzma](https://tukaani.org/xz/)|required|for uncompressing .tar.xz files.|
|[bzip2](https://sourceware.org/bzip2/)|required|for uncompressing .tar.bz2 files.|
|[zlib](https://zlib.net/)|required|for uncompressing .tar.gz and .zip files.|

**[Ubuntu](https://ubuntu.com/)**

```bash
apt -y install cmake ninja-build libcurl4 libcurl4-openssl-dev libyaml-dev libgit2-dev libarchive-dev liblzma-dev libbz2-dev

git clone https://github.com/leleliu008/uppm
cd uppm

cmake -S . -B   build.d -G Ninja -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build   build.d
cmake --install build.d
```

**[AlpineLinux](https://www.alpinelinux.org/)**

```bash
apk add cmake ninja gcc libc-dev curl-dev openssl-dev libgit2-dev libarchive-dev yaml-dev xz-dev bzip2-dev zlib-dev

git clone https://github.com/leleliu008/uppm
cd uppm

cmake -S . -B   build.d -G Ninja -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build   build.d
cmake --install build.d
```

**[ArchLinux](https://archlinux.org/)**

```bash
pacman -Syyuu --noconfirm
pacman -S     --noconfirm git cmake ninja gcc curl openssl libgit2 libarchive libyaml

git clone https://github.com/leleliu008/uppm
cd uppm

cmake -S . -B   build.d -G Ninja -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build   build.d
cmake --install build.d
```

**[FreeBSD](https://www.freebsd.org/)**

```bash
pkg install -y cmake ninja gcc curl openssl libgit2 libarchive libyaml

git clone https://github.com/leleliu008/uppm
cd uppm

cmake -S . -B   build.d -G Ninja -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build   build.d
cmake --install build.d
```

**[vcpkg](https://github.com/microsoft/vcpkg)**
```bash
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh
export VCPKG_ROOT="$PWD/vcpkg"
export PATH="$VCPKG_ROOT:$PATH"

vcpkg install libyaml libgit2 libarchive curl openssl

git clone https://github.com/leleliu008/uppm
cd uppm

cmake -S . -B   build.d -G Ninja -DCMAKE_INSTALL_PREFIX=/usr/local -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake
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
        uppm integrate zsh -d
        uppm integrate zsh --china
        uppm integrate zsh --china -d
        
    I have provide a zsh-completion script for `uppm`. when you've typed `uppm` then type `TAB` key, it will auto complete the rest for you.

    **Note**: to apply this feature, you may need to run the command `autoload -U compinit && compinit`

*   **update formula repositories**

        uppm update
        
    **Note:** this software supports multi formula repositories. Offical formula repository is [uppm-formula-repository](https://github.com/leleliu008/uppm-formula-repository)

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

        uppm info curl installed-dir-path
        uppm info curl installed-dir-tree
        uppm info curl installed-metadata-path
        uppm info curl installed-metadata-json
        uppm info curl installed-metadata-yaml
        uppm info curl installed-datetime-unix
        uppm info curl installed-datetime-formatted
        uppm info curl installed-package-version
        uppm info curl installed-files

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
        uppm depends curl --format=list
        uppm depends curl --format=json
        uppm depends curl --format=dot
        uppm depends curl --format=txt
        uppm depends curl --format=png -o curl-dependencies.png
        
*   **download formula resources of the given package to the local cache**
        
        uppm fetch curl
        uppm fetch @all

*   **install packages**
        
        uppm install curl
        uppm install curl bzip2  -v
        uppm install curl bzip2  -v -d
        
*   **reinstall packages**
        
        uppm reinstall curl
        uppm reinstall curl bzip2 -v
        
*   **uninstall packages**

        uppm uninstall curl
        uppm uninstall curl bzip2
        
*   **upgrade the outdated packages**

        uppm upgrade
        uppm upgrade curl
        uppm upgrade curl bzip2
        
*   **upgrade this software**

        uppm upgrade @self
        uppm upgrade @self -d
        uppm upgrade @self --china
        uppm upgrade @self --china -d
        

*   **list the avaliable formula repositories**

        uppm formula-repo list

*   **add a new formula repository**

        uppm formula-repo add my_repo https://github.com/leleliu008/uppm-formula-repository-linux-x86_64.git

*   **delete a existing formula repository**

        uppm formula-repo del my_repo

*   **view the formula of the given package**
        
        uppm formula view curl
        
*   **edit the formula of the given package**
        
        uppm formula edit curl
        
*   **create the formula of the given package**
        
        uppm formula create curl
        
*   **delete the formula of the given package**
        
        uppm formula delete curl
        
*   **rename the formula of the given package to new name**
        
        uppm formula rename curl curl7
        
*   **list the available packages**
        
        uppm ls-available-packages
        
*   **list the installed packages**
        
        uppm ls-installed-packages
        
*   **list the outdated packages**
        
        uppm ls-outdated-packages
        
*   **is the given package available ?**
        
        uppm is-package-available curl
        
*   **is the given package installed ?**
        
        uppm is-package-installed curl
        
*   **is the given package outdated ?**
        
        uppm is-package-outdated  curl
        
*   **list files of the given installed package in a tree-like format**
        
        uppm tree curl
        
*   **cleanup the unused cached files**
        
        uppm cleanup
        
