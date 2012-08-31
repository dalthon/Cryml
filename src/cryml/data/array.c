#include "../cryml.h"

CryArray CryArray_init(int bucket_size){
    int      i;
    CryArray array;

    array              = malloc(sizeof(CryArrayData));
    array->iterable.start = &CryArray_start;
    array->data        = malloc(bucket_size*sizeof(CryData));
    array->bucket_size = bucket_size;
    array->next_index  = 0;
    array->size        = 0;
    array->this_size   = 0;
    array->next        = NULL;

    for(i = 0; i < array->bucket_size; i++){
        array->data[i] = NULL;
    }

    return array;
}

int CryArray_normalize_index(CryArray array, int index){
    if(index < 0){
        return index + array->next_index;
    }else{
        return index;
    }
}

void CryArray_add(CryArray array, CryData *data){
    CryArray_insert_into(array, data, array->next_index);
}

int CryArray_insert_into(CryArray array, CryData *data, int index){
    if(data == NULL) return FALSE;

    index = CryArray_normalize_index(array, index);
    if(index < 0) return FALSE;

    if(index >= array->next_index) array->next_index = index+1;

    if(index < array->bucket_size){
        if(array->data[index] != NULL){
            CryData_free(array->data[index]);
            array->data[index] = data;
            CryData_referenced(data);
            return FALSE; // TODO: This is right?!
        }else{
            array->size++;
            array->this_size++;
            array->data[index] = data;
            CryData_referenced(data);
            return TRUE;
        }
    }else{
        if(array->next == NULL) array->next = CryArray_init(array->bucket_size);
        if(CryArray_insert_into(array->next, data, index - array->bucket_size)){
            array->size++;
            return TRUE;
        }
        return FALSE;
    }
}

int CryArray_remove(CryArray array, int index){
    index = CryArray_normalize_index(array, index);
    if(index < 0) return FALSE;

    if(index < array->bucket_size){
        if(array->data[index] == NULL){
            return FALSE;
        }else{
            if(index + 1 == array->next_index) array->next_index = index;
            CryData_free(array->data[index]);
            array->data[index] = NULL;
            array->size--;
            array->this_size--;
            return TRUE;
        }
    }else if(array->next != NULL){
        if( CryArray_remove(array->next, index - array->bucket_size) ){
            array->size--;
            if(index + 1 == array->next_index ) array->next_index = index;
            if(array->size == array->this_size){
                CryArray_free(array->next);
                array->next = NULL;
            }
            return TRUE;
        }
    }

    return FALSE;
}

CryData *CryArray_get(CryArray array, int index){
    index = CryArray_normalize_index(array, index);
    if(index < 0) return NULL;

    if(index < array->bucket_size){
        return array->data[index];
    }else if(array->next != NULL){
        return CryArray_get(array->next, index - array->bucket_size);
    }
    return NULL;
}

int CryArray_size(CryArray array){
    return array->size;
}

void CryArray_free(CryArray array){
    int i, removed;

    if(array->next != NULL){
        CryArray_free(array->next);
        array->next = NULL;
    }

    removed = 0;
    for(i = 0; removed < array->this_size && i < array->bucket_size; i++){
        if(array->data[i] != NULL){
            removed++;
            CryData_free(array->data[i]);
            array->data[i] = NULL;
        }
    }

    free(array->data);
    free(array);
}

CryIterator CryArray_start(CryIterable iter) {
    CryArray array = (CryArray) iter;
    CryArrayIterator iterator = malloc(sizeof(CryArrayIteratorData));
    iterator->iterator.next = &CryArrayIterator_next;
    iterator->iterator.free = &CryArrayIterator_free;
    iterator->array = array;
    iterator->cursor = 0;
    return (CryIterator)iterator;
}

CryData  *CryArrayIterator_next(CryIterator iter) {
    CryArrayIterator array_iter = (CryArrayIterator) iter;
    int len = CryArray_size(array_iter->array);

    if (array_iter->cursor >= len) {
        return NULL;
    } else {
        return CryArray_get(array_iter->array, array_iter->cursor++);
    }
}

void CryArrayIterator_free(CryIterator iter) {
    // Nothing to do, since nothing was dinamically alocated inside CryArrayIterator
}
