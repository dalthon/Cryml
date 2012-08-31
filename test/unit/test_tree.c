#include "../../src/cryml/cryml.h"
#include "unit.h"

int before_all_test_tree(void){ return 0; }
int after_all_test_tree(void){  return 0; }

CryTree init_sample_tree(){
    CryTree root;

    root = CryTree_add(NULL, "Orange",  CryData_integer(1));
    CryTree_add(       root, "Banana",  CryData_integer(2));
    CryTree_add(       root, "Turnip",  CryData_integer(3));
    CryTree_add(       root, "Onion",   CryData_integer(4));
    CryTree_add(       root, "Apple",   CryData_integer(5));

    return root;
}

void test_add_to_tree(void){
    CryTree root;

    root = init_sample_tree();

    CU_ASSERT_STRING_EQUAL(root->key, "Orange");
    CU_ASSERT_EQUAL(root->data->value.integer, 1);

    CU_ASSERT_STRING_EQUAL(root->left->key, "Banana");
    CU_ASSERT_EQUAL(root->left->data->value.integer, 2);

    CU_ASSERT_STRING_EQUAL(root->right->key, "Turnip");
    CU_ASSERT_EQUAL(root->right->data->value.integer, 3);

    CU_ASSERT_STRING_EQUAL(root->left->right->key, "Onion");
    CU_ASSERT_EQUAL(root->left->right->data->value.integer, 4);

    CU_ASSERT_STRING_EQUAL(root->left->left->key, "Apple");
    CU_ASSERT_EQUAL(root->left->left->data->value.integer, 5);

    CryTree_add(root, "Onion", CryData_integer(10));
    CU_ASSERT_STRING_EQUAL(root->left->right->key, "Onion");
    CU_ASSERT_EQUAL(root->left->right->data->value.integer, 10);

    CryTree_free(root);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_remove_from_tree(void){
    CryTree root;

    root = init_sample_tree();

    root = CryTree_remove(root, "Banana");
    CU_ASSERT_STRING_EQUAL(root->left->key, "Onion");
    CU_ASSERT_EQUAL(root->left->data->value.integer, 4);
    CU_ASSERT_EQUAL(root->left->right, NULL);
    CU_ASSERT_NOT_EQUAL(root->left->left, NULL);
    CU_ASSERT_EQUAL(CryTree_find(root, "Banana"), NULL);

    root = CryTree_remove(root, "Onion");
    CU_ASSERT_STRING_EQUAL(root->left->key, "Apple");
    CU_ASSERT_EQUAL(root->left->data->value.integer, 5);
    CU_ASSERT_EQUAL(root->left->right, NULL);
    CU_ASSERT_EQUAL(root->left->left, NULL);
    CU_ASSERT_EQUAL(CryTree_find(root, "Onion"), NULL);

    root = CryTree_remove(root, "Apple");
    CU_ASSERT_EQUAL(root->left, NULL);
    CU_ASSERT_EQUAL(CryTree_find(root, "Apple"), NULL);

    root = CryTree_remove(root, "Orange");
    CU_ASSERT_STRING_EQUAL(root->key, "Turnip");
    CU_ASSERT_EQUAL(root->data->value.integer, 3);
    CU_ASSERT_EQUAL(root->right, NULL);
    CU_ASSERT_EQUAL(root->left, NULL);
    CU_ASSERT_EQUAL(CryTree_find(root, "Orange"), NULL);

    root = CryTree_remove(root, "Turnip");
    CU_ASSERT_EQUAL(root, NULL);
    CU_ASSERT_EQUAL(CryTree_find(root, "Turnip"), NULL);

    CryTree_free(root);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_search_on_tree(void){
    CryTree root, node;

    root = init_sample_tree();

    node = CryTree_find(root, "Orange");
    CU_ASSERT_STRING_EQUAL(node->key, "Orange");
    CU_ASSERT_EQUAL(node->data->value.integer, 1);
    CU_ASSERT_EQUAL(CryTree_find_data(root, "Orange")->value.integer, 1);

    node = CryTree_find(root, "Banana");
    CU_ASSERT_STRING_EQUAL(node->key, "Banana");
    CU_ASSERT_EQUAL(node->data->value.integer, 2);
    CU_ASSERT_EQUAL(CryTree_find_data(root, "Banana")->value.integer, 2);

    node = CryTree_find(root, "Turnip");
    CU_ASSERT_STRING_EQUAL(node->key, "Turnip");
    CU_ASSERT_EQUAL(node->data->value.integer, 3);
    CU_ASSERT_EQUAL(CryTree_find_data(root, "Turnip")->value.integer, 3);

    node = CryTree_find(root, "Onion");
    CU_ASSERT_STRING_EQUAL(node->key, "Onion");
    CU_ASSERT_EQUAL(node->data->value.integer, 4);
    CU_ASSERT_EQUAL(CryTree_find_data(root, "Onion")->value.integer, 4);

    node = CryTree_find(root, "Apple");
    CU_ASSERT_STRING_EQUAL(node->key, "Apple");
    CU_ASSERT_EQUAL(node->data->value.integer, 5);
    CU_ASSERT_EQUAL(CryTree_find_data(root, "Apple")->value.integer, 5);

    node = CryTree_find(root, "Cherry");
    CU_ASSERT_EQUAL(node, NULL);
    CU_ASSERT_EQUAL(CryTree_find_data(root, "Cherry"), NULL);

    node = CryTree_find(CryTree_find(root, "Banana"), "Turnip");
    CU_ASSERT_EQUAL(node, NULL);
    CU_ASSERT_EQUAL(CryTree_find_data(CryTree_find(root, "Banana"), "Turnip"), NULL);

    CryTree_free(root);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_first_element_of_tree(void){
    CryTree root, node;

    root = init_sample_tree();

    node = CryTree_first(root);
    CU_ASSERT_STRING_EQUAL(node->key, "Apple");
    CU_ASSERT_EQUAL(node->data->value.integer, 5);

    node = CryTree_first(CryTree_find(root, "Banana"));
    CU_ASSERT_STRING_EQUAL(node->key, "Apple");
    CU_ASSERT_EQUAL(node->data->value.integer, 5);

    node = CryTree_first(CryTree_find(root, "Turnip"));
    CU_ASSERT_STRING_EQUAL(node->key, "Turnip");
    CU_ASSERT_EQUAL(node->data->value.integer, 3);

    CryTree_free(root);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_last_element_of_tree(void){
    CryTree root, node;

    root = init_sample_tree();

    node = CryTree_last(root);
    CU_ASSERT_STRING_EQUAL(node->key, "Turnip");
    CU_ASSERT_EQUAL(node->data->value.integer, 3);

    node = CryTree_last(CryTree_find(root, "Banana"));
    CU_ASSERT_STRING_EQUAL(node->key, "Onion");
    CU_ASSERT_EQUAL(node->data->value.integer, 4);

    node = CryTree_last(CryTree_find(root, "Turnip"));
    CU_ASSERT_STRING_EQUAL(node->key, "Turnip");
    CU_ASSERT_EQUAL(node->data->value.integer, 3);

    node = CryTree_last(CryTree_find(root, "Apple"));
    CU_ASSERT_STRING_EQUAL(node->key, "Apple");
    CU_ASSERT_EQUAL(node->data->value.integer, 5);

    CryTree_free(root);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_push_elements_to_tree(void){
    CryTree root;

    root = CryTree_push(NULL, "Orange", CryData_integer(1));
    CU_ASSERT_EQUAL(CryTree_find_top(root, "Orange")->value.integer, 1);
    CU_ASSERT_EQUAL(CryStack_top(CryTree_find(root, "Orange")->data->value.stack)->value.integer, 1);

    CryTree_push(root, "Banana", CryData_integer(2));
    CU_ASSERT_EQUAL(CryTree_find_top(root, "Banana")->value.integer, 2);
    CU_ASSERT_EQUAL(CryStack_top(CryTree_find(root, "Banana")->data->value.stack)->value.integer, 2);

    CryTree_push(root, "Orange", CryData_integer(3));
    CU_ASSERT_EQUAL(CryTree_find_top(root, "Orange")->value.integer, 3);
    CU_ASSERT_EQUAL(CryStack_top(CryTree_find(root, "Orange")->data->value.stack)->value.integer, 3);

    CryTree_push(root, "Onion", CryData_integer(4));
    CU_ASSERT_EQUAL(CryTree_find_top(root, "Onion")->value.integer, 4);
    CU_ASSERT_EQUAL(CryStack_top(CryTree_find(root, "Onion")->data->value.stack)->value.integer, 4);

    CryTree_push(root, "Banana", CryData_integer(5));
    CU_ASSERT_EQUAL(CryTree_find_top(root, "Banana")->value.integer, 5);
    CU_ASSERT_EQUAL(CryStack_top(CryTree_find(root, "Banana")->data->value.stack)->value.integer, 5);

    CryTree_free(root);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_pop_elements_to_tree(void){
    CryTree root;

    root = CryTree_push(NULL, "Orange", CryData_integer(1));
    CryTree_push(       root, "Banana", CryData_integer(2));
    CryTree_push(       root, "Orange", CryData_integer(3));
    CryTree_push(       root, "Onion",  CryData_integer(4));
    CryTree_push(       root, "Banana", CryData_integer(5));

    CU_ASSERT_EQUAL(CryTree_find_top(root, "Banana")->value.integer, 5);
    root = CryTree_pop(root, "Banana");
    CU_ASSERT_EQUAL(CryTree_find_top(root, "Banana")->value.integer, 2);

    CU_ASSERT_EQUAL(CryTree_find_top(root, "Onion")->value.integer, 4);
    root = CryTree_pop(root, "Onion");
    CU_ASSERT_EQUAL(CryTree_find_top(root, "Onion"), NULL);

    CU_ASSERT_EQUAL(CryTree_find_top(root, "Orange")->value.integer, 3);
    root = CryTree_pop(root, "Orange");
    CU_ASSERT_EQUAL(CryTree_find_top(root, "Orange")->value.integer, 1);
    root = CryTree_pop(root, "Orange");
    CU_ASSERT_EQUAL(CryTree_find_top(root, "Orange"), NULL);

    CryTree_free(root);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

CU_ErrorCode test_tree(CU_pSuite suite){
    suite = CU_add_suite("CryTree Tests (test_tree.c)", before_all_test_tree, after_all_test_tree);
    if(suite == NULL){
       CU_cleanup_registry();
       return CU_get_error();
    }

    if( (CU_add_test(suite, "Add elements to cryml tree     ", test_add_to_tree           ) == NULL ) ||
        (CU_add_test(suite, "Remove elements from cryml tree", test_remove_from_tree      ) == NULL ) ||
        (CU_add_test(suite, "Find first elements on tree    ", test_first_element_of_tree ) == NULL ) ||
        (CU_add_test(suite, "Find last elements on tree     ", test_last_element_of_tree  ) == NULL ) ||
        (CU_add_test(suite, "Search elements on cryml tree  ", test_search_on_tree        ) == NULL ) ||
        (CU_add_test(suite, "Push elements to cryml tree    ", test_push_elements_to_tree ) == NULL ) ||
        (CU_add_test(suite, "Pop elements to cryml tree     ", test_pop_elements_to_tree  ) == NULL ) ){
       CU_cleanup_registry();
       return CU_get_error();
    }

    return CUE_SUCCESS;
}