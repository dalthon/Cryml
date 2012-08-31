#include "../../src/cryml/cryml.h"
#include "unit.h"

int before_all_test_context(void){ return 0; }
int after_all_test_context(void){  return 0; }

CryContext *init_sample_context(){
    return CryContext_init(CryEnv_init(), CryHash_init(3), CryHash_init(3));
}

void free_context(CryContext *context){
    CryHash_free(context->data);
    CryHash_free(context->extra);
    CryEnv_free(context->environment);
    CryContext_free(context);
}

void test_initialize_empty_context(void){
    CryTag      tag;
    CryContext *context;

    context = init_sample_context();
    free_context(context);
    CU_ASSERT_EQUAL(data_allocated, 0);

    context = init_sample_context();
    tag     = CryTag_init();
    CryContext_init_stack(context, tag);

    CryTag_free(tag);
    free_context(context);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_push_and_pop_tag_stack(void){
    CryTag      tag;
    CryData    *data;
    CryContext *context;

    context = init_sample_context();
    tag     = CryTag_init();
    CryContext_init_stack(context, tag);
    CryContext_push_attribute(context, "key", CryData_string("value"));

    data = CryContext_attribute_lookup(context, "key");
    CU_ASSERT_STRING_EQUAL(data->value.string, "value");

    data = CryContext_attribute_lookup(context, "missing_key");
    CU_ASSERT_EQUAL(data, NULL);

    data = CryContext_tag_attribute_lookup(context, "key");
    CU_ASSERT_STRING_EQUAL(data->value.string, "value");

    data = CryContext_tag_attribute_lookup(context, "missing_key");
    CU_ASSERT_EQUAL(data, NULL);

    CryContext_init_stack(context, tag);
    CryContext_push_attribute(context, "something", CryData_string("different"));

    data = CryContext_attribute_lookup(context, "key");
    CU_ASSERT_STRING_EQUAL(data->value.string, "value");

    data = CryContext_attribute_lookup(context, "something");
    CU_ASSERT_STRING_EQUAL(data->value.string, "different");

    data = CryContext_attribute_lookup(context, "missing_key");
    CU_ASSERT_EQUAL(data, NULL);

    data = CryContext_tag_attribute_lookup(context, "key");
    CU_ASSERT_EQUAL(data, NULL);

    data = CryContext_tag_attribute_lookup(context, "something");
    CU_ASSERT_STRING_EQUAL(data->value.string, "different");

    data = CryContext_tag_attribute_lookup(context, "missing_key");
    CU_ASSERT_EQUAL(data, NULL);

    CryContext_push_attribute(context, "key", CryData_string("new_content"));

    data = CryContext_attribute_lookup(context, "key");
    CU_ASSERT_STRING_EQUAL(data->value.string, "new_content");

    data = CryContext_attribute_lookup(context, "something");
    CU_ASSERT_STRING_EQUAL(data->value.string, "different");

    data = CryContext_attribute_lookup(context, "missing_key");
    CU_ASSERT_EQUAL(data, NULL);

    data = CryContext_tag_attribute_lookup(context, "key");
    CU_ASSERT_STRING_EQUAL(data->value.string, "new_content");

    data = CryContext_tag_attribute_lookup(context, "something");
    CU_ASSERT_STRING_EQUAL(data->value.string, "different");

    data = CryContext_tag_attribute_lookup(context, "missing_key");
    CU_ASSERT_EQUAL(data, NULL);

    CryContext_pop_stack(context);

    data = CryContext_attribute_lookup(context, "key");
    CU_ASSERT_STRING_EQUAL(data->value.string, "value");

    data = CryContext_attribute_lookup(context, "something");
    CU_ASSERT_EQUAL(data, NULL);

    data = CryContext_attribute_lookup(context, "missing_key");
    CU_ASSERT_EQUAL(data, NULL);

    data = CryContext_tag_attribute_lookup(context, "key");
    CU_ASSERT_STRING_EQUAL(data->value.string, "value");

    data = CryContext_tag_attribute_lookup(context, "something");
    CU_ASSERT_EQUAL(data, NULL);

    data = CryContext_tag_attribute_lookup(context, "missing_key");
    CU_ASSERT_EQUAL(data, NULL);

    CryTag_free(tag);
    free_context(context);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_data_lookup(void){
    CryHash     hash_data;
    CryData    *data;
    CryContext *context;

    hash_data = CryHash_init(3);
    CryHash_add(hash_data, "key1", CryData_string("value1"));
    CryHash_add(hash_data, "key2", CryData_string("value2"));
    CryHash_add(hash_data, "key3", CryData_string("value3"));
    context = CryContext_init(CryEnv_init(), hash_data, CryHash_init(3));

    data = CryContext_data_lookup(context, "key1");
    CU_ASSERT_STRING_EQUAL(data->value.string, "value1");

    data = CryContext_data_lookup(context, "key2");
    CU_ASSERT_STRING_EQUAL(data->value.string, "value2");

    data = CryContext_data_lookup(context, "key3");
    CU_ASSERT_STRING_EQUAL(data->value.string, "value3");

    data = CryContext_data_lookup(context, "missing_key");
    CU_ASSERT_EQUAL(data, NULL);

    free_context(context);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

CU_ErrorCode test_context(CU_pSuite suite){
    suite = CU_add_suite("CryContext Tests (test_context.c)", before_all_test_context, after_all_test_context);
    if(suite == NULL){
       CU_cleanup_registry();
       return CU_get_error();
    }

    if( (CU_add_test(suite, "Initialize Empty Context", test_initialize_empty_context ) == NULL ) ||
        (CU_add_test(suite, "Push and Pop Tag Stack  ", test_push_and_pop_tag_stack   ) == NULL ) ||
        (CU_add_test(suite, "Data Lookup             ", test_data_lookup              ) == NULL ) ){
       CU_cleanup_registry();
       return CU_get_error();
    }

    return CUE_SUCCESS;
}