#include "../cryml.h"

CryData *CryIterator_next(CryIterator iter) {
    return iter->next(iter);
}

void CryIterator_free(CryIterator iter) {
    iter->free(iter);
    free(iter);
}
