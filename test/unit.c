#include "../src/cryml/cryml.h"
#include "unit/unit.h"

#define ADD_TESTS(statement) status_code = statement(suite); if(status_code != CUE_SUCCESS) return status_code;

int main(){
    char         system_call[80];
    CU_pSuite    suite = NULL;
    CU_ErrorCode status_code;

    data_allocated = 0;

    /* initialize the CUnit test registry */
    if(CU_initialize_registry() != CUE_SUCCESS) return CU_get_error();

    ADD_TESTS( test_array   );
    ADD_TESTS( test_expr    );
    ADD_TESTS( test_hash    );
    ADD_TESTS( test_stack   );
    ADD_TESTS( test_tree    );
    ADD_TESTS( test_tag     );
    ADD_TESTS( test_parser  );
    ADD_TESTS( test_context );

    // // TODO: String's and Text's tests not done yet!
    // ADD_TESTS( test_string );
    // ADD_TESTS( test_text   );

    /* Run all tests using the basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    printf("\n");
    CU_basic_show_failures(CU_get_failure_list());
    printf("\n\n");

    /* Run all tests using the automated interface */
    /* CU_automated_run_tests();                   */
    /* CU_list_tests_to_file();                    */

    /* Run all tests using the console interface   */
    /* CU_console_run_tests();                     */

    /* Run all tests using the curses interface    */
    /* (only on systems having curses)             */
    /* CU_curses_run_tests();                      */

    CU_cleanup_registry();

    sprintf(system_call, "leaks %d > tmp/testing_leaks.txt\n", getpid());
    system(system_call);
    printf("\n\n************************************************************\nDEBUGGING MEMORY LEAKS:\n");
    system("cat tmp/testing_leaks.txt | grep leaks\\ for\n");
    printf("************************************************************\n");

    return CU_get_error();
}
