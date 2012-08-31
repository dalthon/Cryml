#include "../cryml.h"

CryHash CryHash_init(int buckets){
    int i;
    CryHash hash;

    hash          = malloc(sizeof(CryHashData));
    hash->buckets = buckets;
    hash->trees   = malloc(buckets*sizeof(CryTree));

    for(i = 0; i < buckets; i++){
        hash->trees[i] = NULL;
    }

    return hash;
}

int CryHash_hash_function(CryHash hash, char *key){
    int i, result = 0;

    for(i = strlen(key) - 1; i >= 0; i--){
        result += (int) key[i];
    }

    return result % hash->buckets;
}

void CryHash_add(CryHash hash, char *key, CryData *data){
    int hash_func;
    hash_func = CryHash_hash_function(hash, key);

    if(hash->trees[hash_func] == NULL){
        hash->trees[hash_func] = CryTree_add(NULL, key, data);
    }else{
        CryTree_add(hash->trees[hash_func], key, data);
    }
}

void CryHash_remove(CryHash hash, char *key){
    int hash_func;
    hash_func = CryHash_hash_function(hash, key);
    hash->trees[hash_func] = CryTree_remove(hash->trees[hash_func], key);
}

void CryHash_push(CryHash hash, char *key, CryData *data){
    int hash_func;
    hash_func = CryHash_hash_function(hash, key);

    if(hash->trees[hash_func] == NULL){
        hash->trees[hash_func] = CryTree_push(NULL, key, data);
    }else{
        CryTree_push(hash->trees[hash_func], key, data);
    }
}

void CryHash_pop(CryHash hash, char *key){
    int hash_func;
    hash_func = CryHash_hash_function(hash, key);
    hash->trees[hash_func] = CryTree_pop(hash->trees[hash_func], key);
}

CryData *CryHash_find(CryHash hash, char *key){
    int     hash_func;
    CryTree node;

    hash_func = CryHash_hash_function(hash, key);
    node      = CryTree_find(hash->trees[hash_func], key);

    if(node == NULL){
        return NULL;
    }else{
        return node->data;
    }
}

CryData *CryHash_top(CryHash hash, char *key){
    CryData *data;

    data = CryHash_find(hash, key);

    if(data == NULL){
        return NULL;
    }else{
        return CryStack_top(data->value.stack);
    }
}

void CryHash_free(CryHash hash){
    int i;

    for(i = 0; i < hash->buckets; i++){
        if(hash->trees[i] != NULL){
            CryTree_free(hash->trees[i]);
        }
    }

    free(hash->trees);
    free(hash);
}

void CryHash_traverse(CryHash hash, CryStack stack){
    int i;

    for(i = 0; i < hash->buckets; i++){
        if(hash->trees[i] != NULL){
            CryTree_traverse(hash->trees[i], stack);
        }
    }
}