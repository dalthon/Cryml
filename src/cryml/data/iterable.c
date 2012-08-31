#include "../cryml.h"

CryIterator CryIterable_start(CryIterable iter) {
    return iter->start(iter);
}
