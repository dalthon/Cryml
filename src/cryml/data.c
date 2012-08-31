#include "cryml.h"

CryData *CryData_null(){
    CryData *data;

    data                = malloc(sizeof(CryData));
    data->flags         = DATA_NULL;
    data->value.unknown = NULL;
    data->references    = 0;
    data_allocated++;

    return data;
}

CryData *CryData_boolean(int boolean_value){
    CryData *data;

    data                = malloc(sizeof(CryData));
    data->flags         = DATA_BOOLEAN;
    data->references    = 0;
    data->value.boolean = boolean_value;
    data_allocated++;

    return data;
}

CryData *CryData_integer(long int integer_value){
    CryData *data;

    data                = malloc(sizeof(CryData));
    data->flags         = DATA_INTEGER;
    data->references    = 0;
    data->value.integer = integer_value;
    data_allocated++;

    return data;
}

CryData *CryData_double(long double double_value){
    CryData *data;

    data                = malloc(sizeof(CryData));
    data->flags         = DATA_DOUBLE;
    data->references    = 0;
    data->value.cdouble = double_value;
    data_allocated++;

    return data;
}

CryData *CryData_string(char *string_value){
    CryData *data;

    data               = malloc(sizeof(CryData));
    data->flags        = DATA_STRING;
    data->references   = 0;
    data->value.string = CryString_init(string_value);
    data_allocated++;

    return data;
}


CryData *CryData_tag(CryTag tag){
    CryData *data;

    data             = malloc(sizeof(CryData));
    data->flags      = DATA_TAG;
    data->references = 0;
    data->value.tag  = tag;
    data_allocated++;

    return data;
}

CryData *CryData_stack(){
    CryData *data;

    data              = malloc(sizeof(CryData));
    data->flags       = DATA_STACK;
    data->references  = 0;
    data->value.stack = CryStack_init();
    data_allocated++;

    return data;
}

CryData *CryData_array(int bucket_size){
    CryData *data;

    data              = malloc(sizeof(CryData));
    data->flags       = DATA_ARRAY;
    data->references  = 0;
    data->value.array = CryArray_init(bucket_size);
    data_allocated++;

    return data;
}

CryData *CryData_expression(CryExpr expression){
    CryData *data;

    data                   = malloc(sizeof(CryData));
    data->flags            = DATA_EXPR;
    data->references       = 0;
    data->value.expression = expression;
    data_allocated++;

    return data;
}

CryData *CryData_funclib(CryFunctionLib *library){
    CryData *data;

    data                = malloc(sizeof(CryData));
    data->flags         = DATA_FUNC_LIB;
    data->references    = 0;
    data->value.library = library;
    data_allocated++;

    return data;
}

void CryData_referenced(CryData *data){
    data->references++;
}

int CryData_is_true(CryData *data){
    switch(data->flags){
        case DATA_NULL:
            return FALSE;
        case DATA_BOOLEAN:
            return data->value.boolean;
        default:
            if(data->flags & DATA_EXTERNAL){
                // TODO: Handle print custom object
                return FALSE;
            }
    }

    return TRUE;
}

CryData *CryData_to_numeric(CryData *data){
    if(data->flags & DATA_NATIVE){
        if(data->flags & DATA_NUMERIC){
            return data;
        }else{
            return NULL;
        }
    }else{
        // TODO: Call to numeric on external object
        return NULL;
    }
}

CryIterable CryData_to_iterable(CryData *data) {
    return data->value.iterable;
}

int CryData_printable(CryData *data){

    if(data->flags & DATA_PRINTABLE){
        return TRUE;
    }else{
        if(data->flags & DATA_EXTERNAL){
            // TODO: Handle print custom object
            return FALSE;
        }
    }

    return FALSE;
}

int CryData_print(CryData *data, FILE *output){
    if(output == NULL) return 0;

    if(data->flags & DATA_PRINTABLE){
        switch(data->flags){
        case DATA_NULL:
            fprintf(output, "null");
            break;
        case DATA_BOOLEAN:
            if(data->value.boolean){
                fprintf(output, "true");
            }else{
                fprintf(output, "false");
            }
            break;
        case DATA_INTEGER:
            fprintf(output, "%ld", data->value.integer);
            break;
        case DATA_DOUBLE:
            fprintf(output, "%Lf", data->value.cdouble);
            break;
        case DATA_STRING:
            return CryString_print(data->value.string, output);
        case DATA_TEXT:
            return CryText_print(data->value.text, output, TEXT_NON_BLANK);
        case DATA_HASH:
            // TODO: CryHash_print(data->value.hash, output);
            break;
        case DATA_STACK:
            // TODO: CryStack_print(data->value.stack, output);
            break;
        case DATA_ARRAY:
            // TODO: CryArray_print(data->value.array, output);
            break;
        default:
            // TODO: Review this print, thinking if External object can respond to DATA_PRINTABLE mask!
            printf("TODO?! Unknown data type!");
            return ERROR_UNPRINTABLE;
        }
    }else{
        // TODO: Handle print custom object
    }

    return 0;
}

int CryData_escaped_print(CryData *data, FILE *output){
    char  character;
    int   descriptors[2], error_code;
    FILE *escaping_input;
    FILE *escaping_output;

    pipe(descriptors);
    escaping_input = fdopen(descriptors[1], "w");
    error_code = CryData_print(data, escaping_input);
    fclose(escaping_input);
    close(descriptors[1]);

    if(error_code){
        fclose(escaping_output);
        close(descriptors[0]);
        return error_code;
    }else{
        escaping_output = fdopen(descriptors[0], "r");
        while( (character = fgetc(escaping_output)) != EOF ){
            switch(character){
                case '\"':
                    fprintf(output, "\\\"");
                    break;
                case '>':
                    fprintf(output, "&gt;");
                    break;
                case '<':
                    fprintf(output, "&lt;");
                    break;
                default:
                    fputc(character, output);
            }
        }
        return 0;
    }
}

void CryData_free(CryData *data){
    if(data == NULL) return;

    data->references--;
    if(data->references <= 0){
        data_allocated--;
        if(data->flags & DATA_IMEDIATE){
            free(data);
        }else if(data->flags & DATA_NATIVE){
            switch(data->flags){
            case DATA_STRING:
                CryString_free(data->value.string);
                break;
            case DATA_TEXT:
                CryText_free(data->value.text);
                break;
            case DATA_EXPR:
                CryExpr_free(data->value.expression);
                break;
            case DATA_TAG:
                CryTag_free(data->value.tag);
                break;
            case DATA_ATTR:
                CryAttr_free(data->value.attribute);
                break;
            case DATA_HASH:
                CryHash_free(data->value.hash);
                break;
            case DATA_TREE:
                CryTree_free(data->value.tree);
                break;
            case DATA_STACK:
                CryStack_free(data->value.stack);
                break;
            case DATA_ARRAY:
                CryArray_free(data->value.array);
                break;
            case DATA_FUNC_LIB:
                CryFunctionLib_free(data->value.library);
                break;
            default:
                printf("TODO?! Unrecognized native data type! 0x%04x\n", data->flags);
            }
            free(data);
        }else if(data->flags & DATA_EXTERNAL){
            CryObject_free(data->value.object);
        }else{
            printf("TODO?! Unrecognized data type! 0x%04x\n", data->flags);
        }
    }
}
