#include "../../src/cryml/cryml.h"
#include "unit.h"

int before_all_test_text(void){ return 0; }
int after_all_test_text(void){  return 0; }

// TODO: Texts Tests!
void test_missing_texts_tests(void){
    CU_FAIL("Text's tests are not done yet!");
}

CU_ErrorCode test_text(CU_pSuite suite){
    suite = CU_add_suite("CryText Tests (test_text.c)", before_all_test_text, after_all_test_text);
    if(suite == NULL){
       CU_cleanup_registry();
       return CU_get_error();
    }

    if( (CU_add_test(suite, "Text's tests are not done yet!", test_missing_texts_tests ) == NULL ) ){
       CU_cleanup_registry();
       return CU_get_error();
    }

    return CUE_SUCCESS;
}