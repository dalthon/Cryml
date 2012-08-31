#pragma once

#include "../cryml.h"
#include "iterator.h"

struct CryIterableData {
    CryIterableStart start;
};

// Function Prototypes
CryIterator CryIterable_start(CryIterable);
