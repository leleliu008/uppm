#include <stdio.h>
#include "core/sysinfo.h"
#include "uppm.h"

int uppm_sysinfo() {
    SysInfo sysinfo = {0};

    if (sysinfo_make(&sysinfo) != 0) {
        perror(NULL);
        return UPPM_ERROR;
    }

    sysinfo_dump(sysinfo);
    sysinfo_free(sysinfo);
   
    return UPPM_OK;
}
