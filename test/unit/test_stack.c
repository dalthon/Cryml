#include "../../src/cryml/cryml.h"
#include "unit.h"

int before_all_test_stack(void){ return 0; }
int after_all_test_stack(void){  return 0; }

CryStack init_sample_stack(){
    CryStack stack;

    stack = CryStack_init();

    CryStack_push(stack, CryData_integer(1));
    CryStack_push(stack, CryData_integer(2));
    CryStack_push(stack, CryData_integer(3));
    CryStack_push(stack, CryData_integer(4));
    CryStack_push(stack, CryData_integer(5));

    return stack;
}

void test_push_to_stack(void){
    CryStack stack;
    stack = init_sample_stack();

    CU_ASSERT_EQUAL(stack->next->data->value.integer,                         5   );
    CU_ASSERT_EQUAL(stack->next->next->data->value.integer,                   4   );
    CU_ASSERT_EQUAL(stack->next->next->next->data->value.integer,             3   );
    CU_ASSERT_EQUAL(stack->next->next->next->next->data->value.integer,       2   );
    CU_ASSERT_EQUAL(stack->next->next->next->next->next->data->value.integer, 1   );
    CU_ASSERT_EQUAL(stack->next->next->next->next->next->next,                NULL);

    CryStack_free(stack);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_push_strings_to_stack(void){
    CryStack stack;

    stack = CryStack_init();

    CryStack_push(stack, CryData_string("value1"));
    CryStack_push(stack, CryData_string("value2"));
    CryStack_push(stack, CryData_string("value3"));
    CryStack_push(stack, CryData_string("value4"));
    CryStack_push(stack, CryData_string("value5"));

    CU_ASSERT_STRING_EQUAL(stack->next->data->value.string,                         "value5");
    CU_ASSERT_STRING_EQUAL(stack->next->next->data->value.string,                   "value4");
    CU_ASSERT_STRING_EQUAL(stack->next->next->next->data->value.string,             "value3");
    CU_ASSERT_STRING_EQUAL(stack->next->next->next->next->data->value.string,       "value2");
    CU_ASSERT_STRING_EQUAL(stack->next->next->next->next->next->data->value.string, "value1");
    CU_ASSERT_EQUAL(stack->next->next->next->next->next->next,                      NULL    );

    CryStack_free(stack);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_pop_stack(void){
    CryStack stack;
    CryData  *data;

    stack = init_sample_stack();

    data = CryStack_pop(stack);
    CU_ASSERT_EQUAL(data->value.integer, 5);
    CryData_free(data);

    data = CryStack_pop(stack);
    CU_ASSERT_EQUAL(data->value.integer, 4);
    CryData_free(data);

    data = CryStack_pop(stack);
    CU_ASSERT_EQUAL(data->value.integer, 3);
    CryData_free(data);

    data = CryStack_pop(stack);
    CU_ASSERT_EQUAL(data->value.integer, 2);
    CryData_free(data);

    data = CryStack_pop(stack);
    CU_ASSERT_EQUAL(data->value.integer, 1);
    CryData_free(data);

    data = CryStack_pop(stack);
    CU_ASSERT_EQUAL(data, NULL);

    CryStack_free(stack);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_stack_top_value(void){
    CryStack stack;
    CryData  *data;

    stack = init_sample_stack();

    CU_ASSERT_EQUAL(CryStack_top(stack)->value.integer, 5);
    data = CryStack_pop(stack);
    CU_ASSERT_EQUAL(data->value.integer, 5);
    CryData_free(data);

    CU_ASSERT_EQUAL(CryStack_top(stack)->value.integer, 4);
    data = CryStack_pop(stack);
    CU_ASSERT_EQUAL(data->value.integer, 4);
    CryData_free(data);

    CU_ASSERT_EQUAL(CryStack_top(stack)->value.integer, 3);
    data = CryStack_pop(stack);
    CU_ASSERT_EQUAL(data->value.integer, 3);
    CryData_free(data);

    CU_ASSERT_EQUAL(CryStack_top(stack)->value.integer, 2);
    data = CryStack_pop(stack);
    CU_ASSERT_EQUAL(data->value.integer, 2);
    CryData_free(data);

    CU_ASSERT_EQUAL(CryStack_top(stack)->value.integer, 1);
    data = CryStack_pop(stack);
    CU_ASSERT_EQUAL(data->value.integer, 1);
    CryData_free(data);

    CU_ASSERT_EQUAL(CryStack_top(stack), NULL);

    CryStack_free(stack);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_next_stack(void){
    CryStack stack, next_stack;

    stack = init_sample_stack();

    next_stack = CryStack_next(stack);
    CU_ASSERT_EQUAL(CryStack_top(next_stack)->value.integer, 4);

    next_stack = CryStack_next(next_stack);
    CU_ASSERT_EQUAL(CryStack_top(next_stack)->value.integer, 3);

    next_stack = CryStack_next(next_stack);
    CU_ASSERT_EQUAL(CryStack_top(next_stack)->value.integer, 2);

    next_stack = CryStack_next(next_stack);
    CU_ASSERT_EQUAL(CryStack_top(next_stack)->value.integer, 1);

    next_stack = CryStack_next(next_stack);
    CU_ASSERT_EQUAL(CryStack_top(next_stack), NULL);

    CryStack_free(stack);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

CU_ErrorCode test_stack(CU_pSuite suite){
    suite = CU_add_suite("CryStack Tests (test_stack.c)", before_all_test_stack, after_all_test_stack);
    if(suite == NULL){
       CU_cleanup_registry();
       return CU_get_error();
    }

    if( (CU_add_test(suite, "Push elements on cryml stack ", test_push_to_stack         ) == NULL ) ||
        (CU_add_test(suite, "Ppush strings to stack       ", test_push_strings_to_stack ) == NULL ) ||
        (CU_add_test(suite, "Pop elements from cryml stack", test_pop_stack             ) == NULL ) ||
        (CU_add_test(suite, "Access top value on stack    ", test_stack_top_value       ) == NULL ) ||
        (CU_add_test(suite, "Access next stack            ", test_next_stack            ) == NULL ) ){
       CU_cleanup_registry();
       return CU_get_error();
    }

    return CUE_SUCCESS;
}