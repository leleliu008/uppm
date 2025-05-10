#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <elf.h>

#define ERROR_STAT               100
#define ERROR_OPEN               101
#define ERROR_SEEK               102
#define ERROR_READ               103
#define ERROR_NOT_FULLY_READ     104

#define ERROR_NOT_ELF_FILE       120
#define ERROR_BROKEN_ELF_FILE    130
#define ERROR_NO_DYNAMIC_SECTION 140
#define ERROR_NOT_DETERMINED     150

#define LIBC_GNU     1
#define LIBC_MUSL    2

__attribute__((always_inline)) static inline int determine(const char * p) {
    if (p[0] == 'l' && p[1] == 'i' && p[2] == 'b' && p[3] == 'c') {
        // libc.so.6
        if (p[4] == '.' && p[5] == 's' && p[6] == 'o' && p[7] == '.' && (p[8] == '6' || p[8] == '5') && p[9] == '\0') {
            return LIBC_GNU;
        }

        // libc.musl-x86_64.so.1
        if (p[4] == '.' && p[5] == 'm' && p[6] == 'u' && p[7] == 's' && p[8] == 'l' && p[9] == '-') {
            return LIBC_MUSL;
        }
    }

    return ERROR_NOT_DETERMINED;
}

static int handle_elf32(const int fd, const char * const fp) {
    Elf32_Ehdr ehdr;

    int ret = read(fd, &ehdr, sizeof(Elf32_Ehdr));

    if (ret == -1) {
        perror(fp);
        return ERROR_READ;
    }

    if (ret != sizeof(Elf32_Ehdr)) {
        perror(fp);
        return ERROR_NOT_FULLY_READ;
    }

    ///////////////////////////////////////////////////////////

    Elf32_Phdr phdr;

    int hasPT_DYNAMIC = 0;

    for (unsigned int i = 1; i < ehdr.e_phnum; i++) {
        ret = pread(fd, &phdr, sizeof(Elf32_Phdr), ehdr.e_phoff + i * ehdr.e_phentsize);

        if (ret == -1) {
            perror(fp);
            return ERROR_READ;
        }

        if ((size_t)ret != sizeof(Elf32_Phdr)) {
            perror(fp);
            fprintf(stderr, "not fully read.\n");
            return ERROR_NOT_FULLY_READ;
        }

        if (phdr.p_type == PT_DYNAMIC) {
            hasPT_DYNAMIC = 1;
            break;
        }
    }

    if (hasPT_DYNAMIC == 0) {
        fprintf(stderr, "There is no .dynamic section in file: %s\n", fp);
        return ERROR_NO_DYNAMIC_SECTION;
    }

    ///////////////////////////////////////////////////////////

    Elf32_Shdr shdr;

    // .shstrtab section header offset in elf file, it usually is the last section header
    off_t offset = ehdr.e_shoff + ehdr.e_shstrndx * ehdr.e_shentsize;

    ret = pread(fd, &shdr, sizeof(Elf32_Shdr), offset);

    if (ret == -1) {
        perror(fp);
        return ERROR_READ;
    }

    if (ret != sizeof(Elf32_Shdr)) {
        perror(fp);
        fprintf(stderr, "not fully read.\n");
        return ERROR_NOT_FULLY_READ;
    }

    char shstrtab[shdr.sh_size];

    ret = pread(fd, shstrtab, shdr.sh_size, shdr.sh_offset);

    if (ret == -1) {
        perror(fp);
        return ERROR_READ;
    }

    if ((size_t)ret != shdr.sh_size) {
        perror(fp);
        fprintf(stderr, "not fully read.\n");
        return ERROR_NOT_FULLY_READ;
    }

    ///////////////////////////////////////////////////////////

    int hasdynstrSection = 0;

    for (unsigned int i = 1; i < ehdr.e_shnum; i++) {
        ret = pread(fd, &shdr, sizeof(Elf32_Shdr), ehdr.e_shoff + i * ehdr.e_shentsize);

        if (ret == -1) {
            perror(fp);
            return ERROR_READ;
        }

        if ((size_t)ret != sizeof(Elf32_Shdr)) {
            perror(fp);
            fprintf(stderr, "not fully read.\n");
            return ERROR_NOT_FULLY_READ;
        }

        if (shdr.sh_type == SHT_STRTAB) {
            if (strcmp(shstrtab + shdr.sh_name, ".dynstr") == 0) {
                hasdynstrSection = 1;
                break;
            }
        }
    }

    if (hasdynstrSection == 0) {
        fprintf(stderr, "There is no .dynstr section in file: %s\n", fp);
        return ERROR_NO_DYNAMIC_SECTION;
    }

    ///////////////////////////////////////////////////////////

    Elf32_Dyn dyn;

    for (int i = 0; ; i++) {
        ret = pread(fd, &dyn, sizeof(Elf32_Dyn), phdr.p_offset + i * sizeof(Elf32_Dyn));

        if (ret == -1) {
            perror(fp);
            return ERROR_READ;
        }

        if ((size_t)ret != sizeof(Elf32_Dyn)) {
            perror(fp);
            fprintf(stderr, "not fully read.\n");
            return ERROR_NOT_FULLY_READ;
        }

        if (dyn.d_tag == DT_NULL) {
            break;
        }

        if (dyn.d_tag == DT_NEEDED) {
            long xoffset = shdr.sh_offset + dyn.d_un.d_val;

            char buf[100];

            ret = pread(fd, buf, 100, xoffset);

            if (ret == -1) {
                perror(fp);
                return ERROR_READ;
            }

            //puts(buf);
            ret = determine(buf);

            if (ret != ERROR_NOT_DETERMINED) {
                return ret;
            }
        }
    }

    return ERROR_NOT_DETERMINED;
}

static int handle_elf64(const int fd, const char * const fp) {
    Elf64_Ehdr ehdr;

    int ret = read(fd, &ehdr, sizeof(Elf64_Ehdr));

    if (ret == -1) {
        perror(fp);
        return ERROR_READ;
    }

    if (ret != sizeof(Elf64_Ehdr)) {
        perror(fp);
        return ERROR_NOT_FULLY_READ;
    }

    ///////////////////////////////////////////////////////////

    Elf64_Phdr phdr;

    int hasPT_DYNAMIC = 0;

    for (unsigned int i = 1; i < ehdr.e_phnum; i++) {
        ret = pread(fd, &phdr, sizeof(Elf64_Phdr), ehdr.e_phoff + i * ehdr.e_phentsize);

        if (ret == -1) {
            perror(fp);
            return ERROR_READ;
        }

        if ((size_t)ret != sizeof(Elf64_Phdr)) {
            perror(fp);
            fprintf(stderr, "not fully read.\n");
            return ERROR_NOT_FULLY_READ;
        }

        if (phdr.p_type == PT_DYNAMIC) {
            hasPT_DYNAMIC = 1;
            break;
        }
    }

    if (hasPT_DYNAMIC == 0) {
        fprintf(stderr, "There is no .dynamic section in file: %s\n", fp);
        return ERROR_NO_DYNAMIC_SECTION;
    }

    ///////////////////////////////////////////////////////////

    Elf64_Shdr shdr;

    // .shstrtab section header offset in elf file, it usually is the last section header
    off_t offset = ehdr.e_shoff + ehdr.e_shstrndx * ehdr.e_shentsize;

    ret = pread(fd, &shdr, sizeof(Elf64_Shdr), offset);

    if (ret == -1) {
        perror(fp);
        return ERROR_READ;
    }

    if (ret != sizeof(Elf64_Shdr)) {
        perror(fp);
        fprintf(stderr, "not fully read.\n");
        return ERROR_NOT_FULLY_READ;
    }

    char shstrtab[shdr.sh_size];

    ret = pread(fd, shstrtab, shdr.sh_size, shdr.sh_offset);

    if (ret == -1) {
        perror(fp);
        return ERROR_READ;
    }

    if ((size_t)ret != shdr.sh_size) {
        perror(fp);
        fprintf(stderr, "not fully read.\n");
        return ERROR_NOT_FULLY_READ;
    }

    ///////////////////////////////////////////////////////////

    int hasdynstrSection = 0;

    for (unsigned int i = 1; i < ehdr.e_shnum; i++) {
        ret = pread(fd, &shdr, sizeof(Elf64_Shdr), ehdr.e_shoff + i * ehdr.e_shentsize);

        if (ret == -1) {
            perror(fp);
            return ERROR_READ;
        }

        if ((size_t)ret != sizeof(Elf64_Shdr)) {
            perror(fp);
            fprintf(stderr, "not fully read.\n");
            return ERROR_NOT_FULLY_READ;
        }

        if (shdr.sh_type == SHT_STRTAB) {
            if (strcmp(shstrtab + shdr.sh_name, ".dynstr") == 0) {
                hasdynstrSection = 1;
                break;
            }
        }
    }

    if (hasdynstrSection == 0) {
        fprintf(stderr, "There is no .dynstr section in file: %s\n", fp);
        return ERROR_NO_DYNAMIC_SECTION;
    }

    ///////////////////////////////////////////////////////////

    Elf64_Dyn dyn;

    for (int i = 0; ; i++) {
        ret = pread(fd, &dyn, sizeof(Elf64_Dyn), phdr.p_offset + i * sizeof(Elf64_Dyn));

        if (ret == -1) {
            perror(fp);
            return ERROR_READ;
        }

        if ((size_t)ret != sizeof(Elf64_Dyn)) {
            perror(fp);
            fprintf(stderr, "not fully read.\n");
            return ERROR_NOT_FULLY_READ;
        }

        if (dyn.d_tag == DT_NULL) {
            break;
        }

        if (dyn.d_tag == DT_NEEDED) {
            long xoffset = shdr.sh_offset + dyn.d_un.d_val;

            char buf[100];

            ret = pread(fd, buf, 100, xoffset);

            if (ret == -1) {
                perror(fp);
                return ERROR_READ;
            }

            //puts(buf);
            ret = determine(buf);

            if (ret != ERROR_NOT_DETERMINED) {
                return ret;
            }
        }
    }

    return ERROR_NOT_DETERMINED;
}

static int determine_by_inspect_elf_file(const char * fp) {
    struct stat st;

    if (stat(fp, &st) == -1) {
        perror(fp);
        return ERROR_STAT;
    }

    if (st.st_size < 52) {
        fprintf(stderr, "NOT an ELF file: %s\n", fp);
        return ERROR_NOT_ELF_FILE;
    }

    int fd = open(fp, O_RDONLY);

    if (fd == -1) {
        perror(fp);
        return ERROR_OPEN;
    }

    ///////////////////////////////////////////////////////////

    unsigned char a[5];

    ssize_t readBytes = read(fd, a, 5);

    if (readBytes == -1) {
        perror(fp);
        close(fd);
        return ERROR_READ;
    }

    if (readBytes != 5) {
        perror(fp);
        close(fd);
        fprintf(stderr, "not fully read.\n");
        return ERROR_NOT_FULLY_READ;
    }

    ///////////////////////////////////////////////////////////

    // https://www.sco.com/developers/gabi/latest/ch4.eheader.html
    if ((a[0] != 0x7F) || (a[1] != 0x45) || (a[2] != 0x4C) || (a[3] != 0x46)) {
        fprintf(stderr, "NOT an ELF file: %s\n", fp);
        close(fd);
        return ERROR_NOT_ELF_FILE;
    }

    ///////////////////////////////////////////////////////////

    off_t offset = lseek(fd, 0, SEEK_SET);

    if (offset == -1) {
        perror(fp);
        close(fd);
        return ERROR_SEEK;
    }

    ///////////////////////////////////////////////////////////

    int ret;

    switch (a[4]) {
        case 1: ret = handle_elf32(fd, fp); break;
        case 2: ret = handle_elf64(fd, fp); break;
        default: 
            fprintf(stderr, "Invalid ELF file: %s\n", fp);
            ret = ERROR_BROKEN_ELF_FILE;
    }

    close(fd);

    return ret;
}

int determine_by_inspect_elf_files() {
    const char* list[2] = {"/sbin/init", "/bin/sh"};

    int ret;

    for (int i = 0; i < 2; i++) {
        ret = determine_by_inspect_elf_file(list[i]);

        if (ret == LIBC_GNU || ret == LIBC_MUSL) {
            return ret;
        }
    }

    return ret;
}

static int xx() {
    int ret = determine_by_inspect_elf_files();

    switch (ret) {
        case LIBC_GNU: puts("glibc"); return 0;
        case LIBC_MUSL: puts("musl"); return 0;
        default: return ret;
    }
}
