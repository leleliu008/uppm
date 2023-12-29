#include "uppm.h"

int uppm_formula_view(const char * packageName, const bool raw) {
    if (raw) {
        return uppm_formula_cat(packageName);
    } else {
        return uppm_formula_bat(packageName);
    }
}
