#include "../cryml.h"

// TODO: Needs to implement interpolation

char *CryString_init(char *string_value){
    return strdup(string_value);
}

int CryString_print(char *string, FILE *output){
    fprintf(output, "%s", string);
    return 0;
}

void CryString_free(char *string_value){
    free(string_value);
}