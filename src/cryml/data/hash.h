// Structs
struct CryHashData {
    int     buckets;
    CryTree *trees;
};

// Function Prototypes
CryHash  CryHash_init(int buckets);
void     CryHash_add(CryHash hash, char *key, CryData *data);
void     CryHash_remove(CryHash hash, char *key);
void     CryHash_push(CryHash hash, char *key, CryData *data);
void     CryHash_pop(CryHash hash, char *key);
CryData *CryHash_find(CryHash hash, char *key);
CryData *CryHash_top(CryHash hash, char *key);
int      CryHash_hash_function(CryHash hash, char *key);
void     CryHash_free(CryHash hash);
void     CryHash_traverse(CryHash hash, CryStack stack);