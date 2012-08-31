#pragma once

#include "../cryml.h"
#include "iterable.h"

// Structs
struct CryArrayData {
    CryIterableData iterable;

    int      size;
    int      this_size;
    int      bucket_size;
    int      next_index;
    CryData  **data;
    CryArray next;
};

struct CryArrayIteratorData {
    CryIteratorData iterator;
    CryArray array;
    int      cursor;
};

// Function Prototypes
CryArray  CryArray_init(int bucket_size);
void      CryArray_add(CryArray array, CryData *data);
int       CryArray_insert_into(CryArray array, CryData *data, int index);
int       CryArray_remove(CryArray array, int index);
CryData  *CryArray_get(CryArray array, int index);
int       CryArray_size(CryArray array);
void      CryArray_free(CryArray array);

CryIterator CryArray_start(CryIterable iter);
CryData  *CryArrayIterator_next(CryIterator iter);
void CryArrayIterator_free(CryIterator iter);
