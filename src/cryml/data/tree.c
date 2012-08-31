#include "../cryml.h"

// TODO: Think about replace this structure by a Red-Black tree,
//       but maybe it's better to use a simple binary search tree
//       because with this we can always remove leaf nodes if we
//       do it in the reverse order.

CryTree CryTree_find(CryTree tree, char *query_key){
    if(tree == NULL) return NULL;

    if( strcmp(query_key, tree->key) < 0 ){
        return CryTree_find(tree->left, query_key);
    }else if( strcmp(query_key, tree->key) > 0 ){
        return CryTree_find(tree->right, query_key);
    }else {
        return tree;
    }
}

CryData *CryTree_find_data(CryTree tree, char *query_key){
    CryTree node;

    node = CryTree_find(tree, query_key);
    if(node == NULL) return NULL;

    return node->data;
}

CryTree CryTree_first(CryTree tree){
    if(tree == NULL) return NULL;
    for(; tree->left != NULL; tree = tree->left);
    return tree;
}

CryTree CryTree_last(CryTree tree){
    if(tree == NULL) return NULL;
    for(; tree->right != NULL; tree = tree->right);
    return tree;
}

CryTree CryTree_add(CryTree tree, char *key, CryData *data){
    if(tree == NULL){
        tree        = malloc(sizeof(CryTreeNode));
        tree->key   = strdup(key);
        tree->data  = data;
        tree->left  = NULL;
        tree->right = NULL;
        CryData_referenced(data);
    }else{
        if( strcmp(key, tree->key) < 0 ){
            tree->left = CryTree_add(tree->left, key, data);
        }else if( strcmp(key, tree->key) > 0 ){
            tree->right = CryTree_add(tree->right, key, data);
        }else{
            CryData_free(tree->data);
            tree->data = data;
        }
    }
    return tree;
}

CryTree CryTree_remove(CryTree tree, char *query_key){
    CryTree moving_cell;

    if(tree == NULL) return NULL;

    if( strcmp(query_key, tree->key) < 0 ){
        tree->left = CryTree_remove(tree->left, query_key);
    }else if( strcmp(query_key, tree->key) > 0 ){
        tree->right = CryTree_remove(tree->right, query_key);
    }else{
        if( tree->left == NULL ){
            if(tree->right == NULL){
                CryTree_free(tree);
                return NULL;
            }else{
                moving_cell = tree->right;
                tree->right   = NULL;
                CryTree_free(tree);
                return moving_cell;
            }
        }else{
            if( tree->right == NULL ){
                moving_cell = tree->left;
                tree->left    = NULL;
                CryTree_free(tree);
                return moving_cell;
            }else{
                moving_cell = CryTree_first(tree->right);
                CryData_free(tree->data);
                free(tree->key);
                tree->key   = strdup(moving_cell->key);
                tree->data  = moving_cell->data;
                CryData_referenced(tree->data);
                tree->right = CryTree_remove(tree->right, tree->key);
            }
        }
    }
    return tree;
}

CryTree CryTree_push(CryTree tree, char *key, CryData *data){
    if(tree == NULL){
        tree        = malloc(sizeof(CryTreeNode));
        tree->key   = strdup(key);
        tree->data  = CryData_stack();
        tree->left  = NULL;
        tree->right = NULL;
        CryStack_push(tree->data->value.stack, data);
    }else{
        if( strcmp(key, tree->key) < 0 ){
            tree->left = CryTree_push(tree->left, key, data);
        }else if( strcmp(key, tree->key) > 0 ){
            tree->right = CryTree_push(tree->right, key, data);
        }else{
            CryStack_push(tree->data->value.stack, data);
        }
    }
    return tree;
}

CryTree CryTree_pop(CryTree tree, char *query_key){
    CryData *data;
    CryTree moving_cell;

    if(tree == NULL) return NULL;

    if( strcmp(query_key, tree->key) < 0 ){
        tree->left = CryTree_pop(tree->left, query_key);
    }else if( strcmp(query_key, tree->key) > 0 ){
        tree->right = CryTree_pop(tree->right, query_key);
    }else{
        data = CryStack_clean_pop(tree->data->value.stack);
        CryData_free(data);
        if(CryStack_top(tree->data->value.stack) == NULL){
            if( tree->left == NULL ){
                if(tree->right == NULL){
                    CryTree_free(tree);
                    return NULL;
                }else{
                    moving_cell = tree->right;
                    tree->right = NULL;
                    CryTree_free(tree);
                    return moving_cell;
                }
            }else{
                if( tree->right == NULL ){
                    moving_cell = tree->left;
                    tree->left  = NULL;
                    CryTree_free(tree);
                    return moving_cell;
                }else{
                    moving_cell = CryTree_first(tree->right);
                    CryData_free(tree->data);
                    free(tree->key);
                    tree->key   = strdup(moving_cell->key);
                    tree->data  = moving_cell->data;
                    CryData_referenced(tree->data);
                    tree->right = CryTree_remove(tree->right, tree->key);
                }
            }
        }
    }
    return tree;
}

CryData *CryTree_find_top(CryTree tree, char *query_key){
    CryTree node;

    node = CryTree_find(tree, query_key);
    if(node == NULL) return NULL;

    return CryStack_top(node->data->value.stack);
}

void CryTree_free(CryTree tree){
    if(tree != NULL){
        CryTree_free(tree->left);
        CryTree_free(tree->right);
        if(tree->key != NULL)  free(tree->key);
        if(tree->data != NULL) CryData_free(tree->data);
        free(tree);
    }
}

void CryTree_traverse(CryTree tree, CryStack stack){
    if(tree != NULL){
        CryTree_traverse(tree->left, stack);
        CryTree_traverse(tree->right, stack);
        if(tree->key != NULL){
            CryStack_push(stack, CryData_string(tree->key));
        }
    }
}