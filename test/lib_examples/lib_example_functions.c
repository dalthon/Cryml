#include "../../src/cryml/cryml.h"

CryData *function_one(CryContext *context, CryData *function_name, CryData *args){
    char    *string;
    CryData *result;

    string = malloc(256*sizeof(char));
    if(args == NULL){
        sprintf(string, "Any Argument here!");
    }else{
        sprintf(string, "function: %s %d arguments", function_name->value.string, CryArray_size(args->value.array));
    }
    result = CryData_string(string);
    free(string);

    return result;
}

CryData *function_two(CryContext *context, CryData *function_name, CryData *args){
    return CryData_integer(42);
}

int Cryml_function_library_init(CryFunctionLib *library){
    CryFunctionLib_prepare(library);

    CryFunctionLib_add_function(library, "function_two", "without_args",  1, NULL);
    CryFunctionLib_add_function(library, "function_one", "count_args",   -1, NULL);

    CryFunctionLib_release(library);
    return TRUE;
}
