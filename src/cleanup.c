#include "uppm.h"
#include "core/log.h"

int uppm_cleanup(bool verbose) {
    if (verbose) {
        LOG_SUCCESS("Done.");
    }

    return UPPM_OK;
}
