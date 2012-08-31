// Structs
struct CryTreeNode {
    char    *key;
    CryData *data;
    CryTree left;
    CryTree right;
};

// Function Prototypes
CryTree  CryTree_find(CryTree tree, char *query_key);
CryData *CryTree_find_data(CryTree tree, char *query_key);
CryData *CryTree_find_top(CryTree tree, char *query_key);
CryTree  CryTree_first(CryTree tree);
CryTree  CryTree_last(CryTree tree);
void     CryTree_free(CryTree tree);
CryTree  CryTree_add(CryTree tree, char *key, CryData *data);
CryTree  CryTree_remove(CryTree tree, char *query_key);
CryTree  CryTree_push(CryTree tree, char *key, CryData *data);
CryTree  CryTree_pop(CryTree tree, char *query_key);
void     CryTree_traverse(CryTree tree, CryStack stack);