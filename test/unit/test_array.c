#include "../../src/cryml/cryml.h"
#include "unit.h"

int before_all_test_array(void){ return 0; }
int after_all_test_array(void){  return 0; }

CryArray init_sample_array(int bucket_size){
    CryArray array;

    array = CryArray_init(bucket_size);

    CryArray_add(array, CryData_integer(1));
    CryArray_add(array, CryData_integer(3));
    CryArray_add(array, CryData_integer(5));
    CryArray_add(array, CryData_integer(7));
    CryArray_add(array, CryData_integer(9));

    return array;
}

void test_add_to_array(void){
    CryArray array;
    array = init_sample_array(3);

    CU_ASSERT_EQUAL(CryArray_size(array),                5   );
    CU_ASSERT_EQUAL(array->data[0]->value.integer,       1   );
    CU_ASSERT_EQUAL(array->data[1]->value.integer,       3   );
    CU_ASSERT_EQUAL(array->data[2]->value.integer,       5   );
    CU_ASSERT_EQUAL(array->next->data[0]->value.integer, 7   );
    CU_ASSERT_EQUAL(array->next->data[1]->value.integer, 9   );
    CU_ASSERT_EQUAL(array->next->data[2],                NULL);

    CryArray_free(array);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_insert_array(void){
    CryArray array;

    array = init_sample_array(3);
    CU_ASSERT_EQUAL(array->next_index, 5);

    CryArray_insert_into(array, CryData_integer(-10), 2);
    CU_ASSERT_EQUAL(CryArray_size(array),           5 );
    CU_ASSERT_EQUAL(array->next_index,              5 );
    CU_ASSERT_EQUAL(array->data[2]->value.integer, -10);

    CryArray_insert_into(array, CryData_integer(99), 6);
    CU_ASSERT_EQUAL(CryArray_size(array),                      6   );
    CU_ASSERT_EQUAL(array->next->next->data[0]->value.integer, 99  );
    CU_ASSERT_EQUAL(array->next->data[2],                      NULL);
    CU_ASSERT_EQUAL(array->next->next->data[1],                NULL);
    CU_ASSERT_EQUAL(array->next->next->data[2],                NULL);
    CU_ASSERT_EQUAL(array->next_index,                         7   );

    CryArray_insert_into(array, CryData_integer(88), -1);
    CU_ASSERT_EQUAL(CryArray_size(array),                      6   );
    CU_ASSERT_EQUAL(array->next_index,                         7   );
    CU_ASSERT_EQUAL(array->next->next->data[0]->value.integer, 88  );
    CU_ASSERT_EQUAL(array->next->data[2],                      NULL);
    CU_ASSERT_EQUAL(array->next->next->data[1],                NULL);
    CU_ASSERT_EQUAL(array->next->next->data[2],                NULL);

    CryArray_insert_into(array, CryData_integer(88), -1);
    CU_ASSERT_EQUAL(CryArray_size(array),                      6   );
    CU_ASSERT_EQUAL(array->next_index,                         7   );
    CU_ASSERT_EQUAL(array->next->next->data[0]->value.integer, 88  );
    CU_ASSERT_EQUAL(array->next->data[2],                      NULL);
    CU_ASSERT_EQUAL(array->next->next->data[1],                NULL);
    CU_ASSERT_EQUAL(array->next->next->data[2],                NULL);

    CryArray_free(array);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_remove_array(void){
    CryArray array;
    array = init_sample_array(3);

    CU_ASSERT(CryArray_remove(array, 2));
    CU_ASSERT_EQUAL(CryArray_size(array),                      4   );
    CU_ASSERT_EQUAL(array->data[0]->value.integer,             1   );
    CU_ASSERT_EQUAL(array->data[1]->value.integer,             3   );
    CU_ASSERT_EQUAL(array->data[2],                            NULL);
    CU_ASSERT_EQUAL(array->next->data[0]->value.integer,       7   );
    CU_ASSERT_EQUAL(array->next->data[1]->value.integer,       9   );
    CU_ASSERT_EQUAL(array->next->data[2],                      NULL);

    CU_ASSERT(CryArray_remove(array, 4));
    CU_ASSERT_EQUAL(CryArray_size(array),                      3   );
    CU_ASSERT_EQUAL(array->data[0]->value.integer,             1   );
    CU_ASSERT_EQUAL(array->data[1]->value.integer,             3   );
    CU_ASSERT_EQUAL(array->data[2],                            NULL);
    CU_ASSERT_EQUAL(array->next->data[0]->value.integer,       7   );
    CU_ASSERT_EQUAL(array->next->data[1],                      NULL);
    CU_ASSERT_EQUAL(array->next->data[2],                      NULL);

    CU_ASSERT(CryArray_remove(array, 3));
    CU_ASSERT_EQUAL(CryArray_size(array),                      2   );
    CU_ASSERT_EQUAL(array->data[0]->value.integer,             1   );
    CU_ASSERT_EQUAL(array->data[1]->value.integer,             3   );
    CU_ASSERT_EQUAL(array->data[2],                            NULL);
    CU_ASSERT_EQUAL(array->next,                               NULL);

    CryArray_insert_into(array, CryData_integer(99), 8);
    CU_ASSERT_EQUAL(CryArray_size(array),                      3   );
    CU_ASSERT_EQUAL(array->data[0]->value.integer,             1   );
    CU_ASSERT_EQUAL(array->data[1]->value.integer,             3   );
    CU_ASSERT_EQUAL(array->data[2],                            NULL);
    CU_ASSERT_EQUAL(array->next->data[0],                      NULL);
    CU_ASSERT_EQUAL(array->next->data[1],                      NULL);
    CU_ASSERT_EQUAL(array->next->data[2],                      NULL);
    CU_ASSERT_EQUAL(array->next->next->data[0],                NULL);
    CU_ASSERT_EQUAL(array->next->next->data[1],                NULL);
    CU_ASSERT_EQUAL(array->next->next->data[2]->value.integer, 99  );

    CryArray_free(array);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_access_array(void){
    CryArray array;
    array = init_sample_array(3);

    CryArray_insert_into(array, CryData_integer(99), 9);
    CU_ASSERT_EQUAL(CryArray_get(array, 0)->value.integer, 1   );
    CU_ASSERT_EQUAL(CryArray_get(array, 1)->value.integer, 3   );
    CU_ASSERT_EQUAL(CryArray_get(array, 2)->value.integer, 5   );
    CU_ASSERT_EQUAL(CryArray_get(array, 3)->value.integer, 7   );
    CU_ASSERT_EQUAL(CryArray_get(array, 4)->value.integer, 9   );
    CU_ASSERT_EQUAL(CryArray_get(array, 5),                NULL);
    CU_ASSERT_EQUAL(CryArray_get(array, 6),                NULL);
    CU_ASSERT_EQUAL(CryArray_get(array, 7),                NULL);
    CU_ASSERT_EQUAL(CryArray_get(array, 8),                NULL);
    CU_ASSERT_EQUAL(CryArray_get(array, 9)->value.integer, 99  );

    CryArray_free(array);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

CU_ErrorCode test_array(CU_pSuite suite){
    suite = CU_add_suite("CryArray Tests (test_array.c)", before_all_test_array, after_all_test_array);
    if(suite == NULL){
       CU_cleanup_registry();
       return CU_get_error();
    }

    if( (CU_add_test(suite, "Add elements on cryml array     ", test_add_to_array ) == NULL ) ||
        (CU_add_test(suite, "Insert elements from cryml array", test_insert_array ) == NULL ) ||
        (CU_add_test(suite, "Remove elements from cryml array", test_remove_array ) == NULL ) ||
        (CU_add_test(suite, "Access elements on array        ", test_access_array ) == NULL ) ){
       CU_cleanup_registry();
       return CU_get_error();
    }

    return CUE_SUCCESS;
}