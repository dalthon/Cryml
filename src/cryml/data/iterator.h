#pragma once

#include "../cryml.h"

// Structs
struct CryIteratorData {
    CryIteratorNext  next;
    CryIteratorFree  free;
};

// Function Prototypes
CryData  *CryIterator_next(CryIterator);
void      CryIterator_free(CryIterator);
