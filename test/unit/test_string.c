#include "../../src/cryml/cryml.h"
#include "unit.h"

int before_all_test_string(void){ return 0; }
int after_all_test_string(void){  return 0; }

// TODO: Strings Tests!
void test_missing_strings_tests(void){
    CU_FAIL("String's tests are not done yet!");
}

CU_ErrorCode test_string(CU_pSuite suite){
    suite = CU_add_suite("CryString Tests (test_string.c)", before_all_test_string, after_all_test_string);
    if(suite == NULL){
       CU_cleanup_registry();
       return CU_get_error();
    }

    if( (CU_add_test(suite, "String's tests are not done yet!", test_missing_strings_tests ) == NULL ) ){
       CU_cleanup_registry();
       return CU_get_error();
    }

    return CUE_SUCCESS;
}