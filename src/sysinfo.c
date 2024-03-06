#include "core/sysinfo.h"
#include "uppm.h"

int uppm_sysinfo() {
    SysInfo sysinfo = {0};

    int ret = sysinfo_make(&sysinfo);

    if (ret == UPPM_OK) {
        sysinfo_dump(&sysinfo);
        sysinfo_free(&sysinfo);
    }

    return ret;
}
