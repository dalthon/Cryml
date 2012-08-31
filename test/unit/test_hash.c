#include "../../src/cryml/cryml.h"
#include "unit.h"

int before_all_test_hash(void){ return 0; }
int after_all_test_hash(void){  return 0; }

CryHash init_sample_hash(){
    CryHash hash;

    hash = CryHash_init(5);

    CryHash_add(hash, "Orange", CryData_integer(4));
    CryHash_add(hash, "Turnip", CryData_integer(2));
    CryHash_add(hash, "Onion",  CryData_integer(0));
    CryHash_add(hash, "Apple",  CryData_integer(3));
    CryHash_add(hash, "Banana", CryData_integer(2));

    return hash;
}

void test_add_to_hash(void){
    CryHash hash;
    hash = init_sample_hash();

    CU_ASSERT_NOT_EQUAL(hash->trees[0], NULL);
    CU_ASSERT_EQUAL(    hash->trees[1], NULL);
    CU_ASSERT_NOT_EQUAL(hash->trees[2], NULL);
    CU_ASSERT_NOT_EQUAL(hash->trees[3], NULL);
    CU_ASSERT_NOT_EQUAL(hash->trees[4], NULL);

    CryHash_free(hash);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_remove_from_hash(void){
    CryHash hash;
    hash = init_sample_hash();

    CryHash_remove(hash, "Onion");
    CU_ASSERT_EQUAL(hash->trees[0], NULL);
    CU_ASSERT_EQUAL(CryHash_find(hash, "Onion"), NULL);

    CryHash_remove(hash, "Turnip");
    CU_ASSERT_NOT_EQUAL(hash->trees[2], NULL);
    CU_ASSERT_EQUAL(CryHash_find(hash, "Turnip"), NULL);

    CryHash_remove(hash, "Banana");
    CU_ASSERT_EQUAL(hash->trees[2], NULL);
    CU_ASSERT_EQUAL(CryHash_find(hash, "Banana"), NULL);

    CryHash_remove(hash, "Apple");
    CU_ASSERT_EQUAL(hash->trees[3], NULL);
    CU_ASSERT_EQUAL(CryHash_find(hash, "Apple"), NULL);

    CryHash_remove(hash, "Orange");
    CU_ASSERT_EQUAL(hash->trees[4], NULL);
    CU_ASSERT_EQUAL(CryHash_find(hash, "Orange"), NULL);

    CryHash_free(hash);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_push_to_hash(void){
    CryHash hash;

    hash = CryHash_init(5);
    CU_ASSERT_EQUAL(hash->trees[4], NULL);

    CryHash_push(hash, "Orange", CryData_integer(0));
    CU_ASSERT_EQUAL(CryHash_top(hash, "Orange")->value.integer,                                             0);
    CU_ASSERT_EQUAL(CryStack_top(CryTree_find(hash->trees[4], "Orange")->data->value.stack)->value.integer, 0);

    CryHash_push(hash, "Orange", CryData_integer(1));
    CU_ASSERT_EQUAL(CryHash_top(hash, "Orange")->value.integer,                                             1);
    CU_ASSERT_EQUAL(CryStack_top(CryTree_find(hash->trees[4], "Orange")->data->value.stack)->value.integer, 1);

    CryHash_push(hash, "Banana", CryData_integer(2));
    CU_ASSERT_EQUAL(CryHash_top(hash, "Banana")->value.integer,                                             2);
    CU_ASSERT_EQUAL(CryStack_top(CryTree_find(hash->trees[2], "Banana")->data->value.stack)->value.integer, 2);

    CryHash_push(hash, "Turnip", CryData_integer(3));
    CU_ASSERT_EQUAL(CryHash_top(hash, "Turnip")->value.integer,                                             3);
    CU_ASSERT_EQUAL(CryStack_top(CryTree_find(hash->trees[2], "Turnip")->data->value.stack)->value.integer, 3);

    CryHash_push(hash, "Apple", CryData_integer(4));
    CU_ASSERT_EQUAL(CryHash_top(hash, "Apple")->value.integer,                                              4);
    CU_ASSERT_EQUAL(CryStack_top(CryTree_find(hash->trees[3], "Apple")->data->value.stack)->value.integer,  4);

    CU_ASSERT_EQUAL(CryStack_top(CryTree_find(hash->trees[4], "Orange")->data->value.stack)->value.integer, 1);
    CU_ASSERT_EQUAL(CryStack_top(CryTree_find(hash->trees[2], "Banana")->data->value.stack)->value.integer, 2);
    CU_ASSERT_EQUAL(CryStack_top(CryTree_find(hash->trees[2], "Turnip")->data->value.stack)->value.integer, 3);

    CryHash_free(hash);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_push_same_element_twice(void){
    CryData *data;
    CryHash  hash1, hash2;

    hash1 = CryHash_init(5);
    CU_ASSERT_EQUAL(hash1->trees[4], NULL);
    hash2 = CryHash_init(5);
    CU_ASSERT_EQUAL(hash2->trees[4], NULL);

    data = CryData_string("SOMETHING");
    CryHash_push(hash1, "something", data);
    CryHash_push(hash2, "something", data);

    CU_ASSERT_STRING_EQUAL(CryHash_top(hash1, "something")->value.string, "SOMETHING");
    CU_ASSERT_STRING_EQUAL(CryHash_top(hash2, "something")->value.string, "SOMETHING");

    CU_ASSERT_EQUAL(data->references, 2);
    CryHash_pop(hash1, "something");

    CU_ASSERT_EQUAL(data->references, 1);
    CryHash_pop(hash2, "something");

    CryHash_free(hash1);
    CryHash_free(hash2);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_pop_from_hash(void){
    CryHash hash;

    hash = CryHash_init(5);
    CU_ASSERT_EQUAL(hash->trees[4], NULL);

    CryHash_push(hash, "Orange", CryData_integer(0));
    CryHash_push(hash, "Orange", CryData_integer(1));
    CryHash_push(hash, "Banana", CryData_integer(2));
    CryHash_push(hash, "Turnip", CryData_integer(3));
    CryHash_push(hash, "Apple",  CryData_integer(4));

    CU_ASSERT_EQUAL(CryHash_top(hash, "Orange")->value.integer, 1);
    CU_ASSERT_EQUAL(CryHash_top(hash, "Banana")->value.integer, 2);
    CU_ASSERT_EQUAL(CryHash_top(hash, "Turnip")->value.integer, 3);
    CU_ASSERT_EQUAL(CryHash_top(hash, "Apple")->value.integer,  4);

    CryHash_pop(hash, "Banana");
    CU_ASSERT_EQUAL(CryHash_top(hash, "Orange")->value.integer, 1   );
    CU_ASSERT_EQUAL(CryHash_top(hash, "Banana"),                NULL);
    CU_ASSERT_EQUAL(CryHash_top(hash, "Turnip")->value.integer, 3   );
    CU_ASSERT_EQUAL(CryHash_top(hash, "Apple")->value.integer,  4   );

    CryHash_pop(hash, "Orange");
    CU_ASSERT_EQUAL(CryHash_top(hash, "Orange")->value.integer, 0   );
    CU_ASSERT_EQUAL(CryHash_top(hash, "Banana"),                NULL);
    CU_ASSERT_EQUAL(CryHash_top(hash, "Turnip")->value.integer, 3   );
    CU_ASSERT_EQUAL(CryHash_top(hash, "Apple")->value.integer,  4   );

    CryHash_pop(hash, "Orange");
    CU_ASSERT_EQUAL(CryHash_top(hash, "Orange"),                NULL);
    CU_ASSERT_EQUAL(CryHash_top(hash, "Banana"),                NULL);
    CU_ASSERT_EQUAL(CryHash_top(hash, "Turnip")->value.integer, 3   );
    CU_ASSERT_EQUAL(CryHash_top(hash, "Apple")->value.integer,  4   );

    CryHash_pop(hash, "Turnip");
    CU_ASSERT_EQUAL(CryHash_top(hash, "Orange"),                NULL);
    CU_ASSERT_EQUAL(CryHash_top(hash, "Banana"),                NULL);
    CU_ASSERT_EQUAL(CryHash_top(hash, "Turnip"),                NULL);
    CU_ASSERT_EQUAL(CryHash_top(hash, "Apple")->value.integer,  4   );

    CryHash_pop(hash, "Apple");
    CU_ASSERT_EQUAL(CryHash_top(hash, "Orange"), NULL);
    CU_ASSERT_EQUAL(CryHash_top(hash, "Banana"), NULL);
    CU_ASSERT_EQUAL(CryHash_top(hash, "Turnip"), NULL);
    CU_ASSERT_EQUAL(CryHash_top(hash, "Apple"),  NULL);

    CryHash_free(hash);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_search_on_hash(void){
    CryHash hash;
    hash = init_sample_hash();

    CU_ASSERT_EQUAL(CryHash_find(hash, "Orange")->value.integer, 4);
    CU_ASSERT_EQUAL(CryHash_find(hash, "Turnip")->value.integer, 2);
    CU_ASSERT_EQUAL(CryHash_find(hash, "Onion" )->value.integer, 0);
    CU_ASSERT_EQUAL(CryHash_find(hash, "Apple" )->value.integer, 3);
    CU_ASSERT_EQUAL(CryHash_find(hash, "Banana")->value.integer, 2);

    CU_ASSERT_EQUAL(CryHash_find(hash, "Cherry"), NULL);

    CryHash_free(hash);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_hash_function(void){
    CryHash hash;

    hash = CryHash_init(10);

    CU_ASSERT_EQUAL(CryHash_hash_function(hash, "Orange"), 4);
    CU_ASSERT_EQUAL(CryHash_hash_function(hash, "Turnip"), 2);
    CU_ASSERT_EQUAL(CryHash_hash_function(hash, "Onion"),  5);
    CU_ASSERT_EQUAL(CryHash_hash_function(hash, "Apple"),  8);
    CU_ASSERT_EQUAL(CryHash_hash_function(hash, "Banana"), 7);

    CryHash_free(hash);
    CU_ASSERT_EQUAL(data_allocated, 0);
}


CU_ErrorCode test_hash(CU_pSuite suite){
    suite = CU_add_suite("CryHash Tests (test_hash.c)", before_all_test_hash, after_all_test_hash);
    if(suite == NULL){
       CU_cleanup_registry();
       return CU_get_error();
    }

    if( (CU_add_test(suite, "Add elements to cryml hash             ", test_add_to_hash             ) == NULL ) ||
        (CU_add_test(suite, "Remove elements from cryml hash        ", test_remove_from_hash        ) == NULL ) ||
        (CU_add_test(suite, "Push elements to cryml hash            ", test_push_to_hash            ) == NULL ) ||
        (CU_add_test(suite, "Push same element twice to cryml hash  ", test_push_same_element_twice ) == NULL ) ||
        (CU_add_test(suite, "Pop elements from cryml hash           ", test_pop_from_hash           ) == NULL ) ||
        (CU_add_test(suite, "Search elements on cryml hash          ", test_search_on_hash          ) == NULL ) ||
        (CU_add_test(suite, "Hash function generates correct values ", test_hash_function           ) == NULL ) ){
       CU_cleanup_registry();
       return CU_get_error();
    }

    return CUE_SUCCESS;
}