#include "../cryml.h"

#define RAISE_IF_EVAL_ERROR(context) if(CryContext_error(context)) return NULL

CryExpr CryExpr_init(int opcode, CryData *data){
    CryExpr expr;

    expr                           = malloc(sizeof(CryExprData));
    expr->data                     = data;
    expr->scope                    = NULL;
    expr->opcode                   = opcode;
    expr->function                 = NULL;
    expr->tag_function             = NULL;
    expr->tag_lookup_callback      = NULL;
    expr->function_lookup_callback = NULL;

    if(data != NULL) CryData_referenced(data);

    return expr;
}

CryExpr CryExpr_clone(CryExpr expr){
    CryExpr cloned_expr;

    cloned_expr                           = malloc(sizeof(CryExprData));
    cloned_expr->data                     = expr->data;
    cloned_expr->scope                    = expr->scope;
    cloned_expr->opcode                   = expr->opcode;
    cloned_expr->tag_function             = expr->tag_function;
    cloned_expr->tag_lookup_callback      = expr->tag_lookup_callback;
    cloned_expr->function                 = expr->function;
    cloned_expr->function_lookup_callback = expr->function_lookup_callback;

    if(expr->data != NULL) CryData_referenced(expr->data);

    return cloned_expr;
}

CryData *CryExpr_eval(CryContext *context, CryExpr expr){
    CryData *return_data, *data1, *data2;

    if(expr->opcode == EXPR_EVAL) return expr->data;

    data1       = NULL;
    data2       = NULL;
    return_data = NULL;
    switch(expr->opcode & EXPR_NON_DATA_MASK){
        case EXPR_ATTR_MASK:
            if( expr->opcode == EXPR_ATTR ){
                data1 = CryContext_attribute_lookup(context, expr->data->value.string);
            }else if(expr->opcode == EXPR_DATA){
                data1 = CryContext_data_lookup(context, expr->data->value.string);
            }else{
                CryContext_raise(context, ERROR_UNKNOWN_RENDERING, "Invalid attribute expression");
                return NULL;
            }
            RAISE_IF_EVAL_ERROR(context);
            return data1;
        case EXPR_ARITH_MASK:
            if(expr->opcode == EXPR_MINUS){
                data1 = CryExpr_get_numeric(context, expr->data);
                CryData_referenced(data1);
                return_data = CryExpr_eval_minus_arithmetic(data1);
            }else{
                data1 = CryExpr_get_numeric(context, CryArray_get(expr->data->value.array, 0));
                data2 = CryExpr_get_numeric(context, CryArray_get(expr->data->value.array, 1));
                CryData_referenced(data1);
                CryData_referenced(data2);
                return_data = CryExpr_eval_arithmetic(context, data1, expr->opcode, data2);
            }
            break;
        case EXPR_BOOL_MASK:
            if(expr->opcode == EXPR_NOT){
                data1 = CryExpr_get_boolean(context, expr->data);
                CryData_referenced(data1);
            }else{
                // TODO: Optimize calls, sometimes isn't needed to get data2
                data1 = CryExpr_get_boolean(context, CryArray_get(expr->data->value.array, 0));
                data2 = CryExpr_get_boolean(context, CryArray_get(expr->data->value.array, 1));
                CryData_referenced(data1);
                CryData_referenced(data2);
            }
            return_data =  CryExpr_eval_boolean(data1, expr->opcode, data2);
            break;
        case EXPR_COMPARE_MASK:
            data1 = CryExpr_get_comparable(context, CryArray_get(expr->data->value.array, 0));
            data2 = CryExpr_get_comparable(context, CryArray_get(expr->data->value.array, 1));
            CryData_referenced(data1);
            CryData_referenced(data2);
            return_data = CryExpr_eval_comparable(data1, expr->opcode, data2);
            break;
        case EXPR_ENV_MASK:
            printf("WARNING: It's not supposed to be here!\n");
            break;
        case EXPR_FUNC_MASK:
            data1 = CryArray_get(expr->data->value.array, 0);
            data2 = CryArray_get(expr->data->value.array, 1);
            CryData_referenced(data1);
            if(data2) CryData_referenced(data2);
            return_data = CryExpr_eval_function(context, expr, data1, data2);
            break;
        // case EXPR_SCOPED_MASK:
        //     // TODO: implement scope eval
        //     data1 = CryArray_get(expr->data->value.array, 0);
        //     data2 = CryArray_get(expr->data->value.array, 1);
        //     return CryExpr_eval_scope(expr->scope, data2);
        case EXPR_CUSTOM_MASK:
            // TODO: implement index and interpolation
            printf("CUSTOM MASK, not done yet!\n");
            return_data = CryData_null();
            break;
        // case EXPR_METHOD_MASK:
        //     // TODO: implement method call
        //     printf("METHOD MASK, (I don't have any idea of how I want it to work!)\n");
        //     break;
        case EXPR_ARRAY_MASK:
            return_data = CryExpr_eval_array(context, expr);
            break;
        default:
            printf("TODO?! Invalid Expression OpCode! 0x%04x\n", expr->opcode);
    }

    if(data1 != NULL) CryData_free(data1);
    if(data2 != NULL) CryData_free(data2);
    return return_data;
}

CryData *CryExpr_get_numeric(CryContext *context, CryData *data){
    if(data == NULL) return NULL;

    if(data->flags & DATA_NUMERIC){
        return data;
    }else if(data->flags == DATA_EXPR){
        return CryExpr_get_numeric(context, CryExpr_eval(context, data->value.expression));
    } else if(data->flags & DATA_EXTERNAL){
        // TODO: get numeric on external objects
        return NULL;
    }

    return NULL;
}

CryData *CryExpr_get_boolean(CryContext *context, CryData *data){
    if(data == NULL) return NULL;

    if(data->flags == DATA_BOOLEAN){
        return data;
    }else if(data->flags == DATA_EXPR){
        return CryExpr_get_boolean(context, CryExpr_eval(context, data->value.expression));
    } else if(data->flags & DATA_EXTERNAL){
        // TODO: get boolean on external objects
        return NULL;
    }

    return NULL;
}

CryData *CryExpr_get_comparable(CryContext *context, CryData *data){
    if(data == NULL) return NULL;

    if(data->flags & DATA_IMEDIATE || data->flags == DATA_STRING){
        return data;
    }else if(data->flags == DATA_EXPR){
        return CryExpr_get_comparable(context, CryExpr_eval(context, data->value.expression));
    } else if(data->flags & DATA_EXTERNAL){
        // TODO: get string, numeric or boolean on external objects
        return NULL;
    }

    return NULL;
}

CryData *CryExpr_eval_minus_arithmetic(CryData *data){
    if(data->flags == DATA_INTEGER){
        return CryData_integer(-data->value.integer);
    }else{
        return CryData_double(-data->value.cdouble);
    }
}

CryData *CryExpr_eval_arithmetic(CryContext *context, CryData *data1, int opcode, CryData *data2){
    long double double_value_1, double_value_2;

    if(data1->flags == DATA_INTEGER && data2->flags == DATA_INTEGER){
        switch(opcode){
            case EXPR_ADD:
                return CryData_integer(data1->value.integer + data2->value.integer);
            case EXPR_SUB:
                return CryData_integer(data1->value.integer - data2->value.integer);
            case EXPR_MUL:
                return CryData_integer(data1->value.integer * data2->value.integer);
            case EXPR_DIV:
                if(data2->value.integer == 0){
                    CryContext_raise(context, ERROR_DIVISION_BY_ZERO, "Invalid division %ld/0", data1->value.integer);
                    return NULL;
                }
                return CryData_integer(data1->value.integer / data2->value.integer);
            case EXPR_MOD:
                return CryData_integer(data1->value.integer % data2->value.integer);
        }
    }else{
        if(data1->flags == DATA_DOUBLE){
            double_value_1 = data1->value.cdouble;
        }else{
            double_value_1 = 0.0 + data1->value.integer;
        }

        if(data2->flags == DATA_DOUBLE){
            double_value_2 = data2->value.cdouble;
        }else{
            double_value_2 = 0.0 + data2->value.integer;
        }

        switch(opcode){
            case EXPR_ADD:
                return CryData_double(double_value_1 + double_value_2);
            case EXPR_SUB:
                return CryData_double(double_value_1 - double_value_2);
            case EXPR_MUL:
                return CryData_double(double_value_1 * double_value_2);
            case EXPR_DIV:
                if(double_value_2 < 0.000001 && double_value_2 > -0.000001){
                    CryContext_raise(context, ERROR_DIVISION_BY_ZERO, "Invalid division %Lf/0", double_value_1);
                    return NULL;
                }
                return CryData_double(double_value_1 / double_value_2);
            case EXPR_MOD:
                return NULL;
        }
    }

    return NULL;
}

CryData *CryExpr_eval_boolean(CryData *data1, int opcode, CryData *data2){
    switch(opcode){
        case EXPR_NOT:
            return CryData_boolean(!data1->value.boolean);
        case EXPR_AND:
            return CryData_boolean(data1->value.boolean && data2->value.boolean);
        case EXPR_OR:
            return CryData_boolean(data1->value.boolean || data2->value.boolean);
    }

    return NULL;
}

CryData *CryExpr_eval_comparable(CryData *data1, int opcode, CryData *data2){
    if((data1->flags & DATA_NUMERIC) && (data2->flags & DATA_NUMERIC)){
        switch(opcode){
            case EXPR_EQ:
                return CryData_boolean(Util_numeric_comparison(data1, data2) == 0);
            case EXPR_NE:
                return CryData_boolean(Util_numeric_comparison(data1, data2) != 0);
            case EXPR_LT:
                return CryData_boolean(Util_numeric_comparison(data1, data2) < 0);
            case EXPR_LE:
                return CryData_boolean(Util_numeric_comparison(data1, data2) <= 0);
            case EXPR_GT:
                return CryData_boolean(Util_numeric_comparison(data1, data2) > 0);
            case EXPR_GE:
                return CryData_boolean(Util_numeric_comparison(data1, data2) >= 0);
        }
    }else if(data1->flags == DATA_STRING && data2->flags == DATA_STRING){
        switch(opcode){
            case EXPR_EQ:
                return CryData_boolean(strcmp(data1->value.string, data2->value.string) == 0);
            case EXPR_NE:
                return CryData_boolean(strcmp(data1->value.string, data2->value.string) != 0);
            case EXPR_LT:
                return CryData_boolean(strcmp(data1->value.string, data2->value.string) < 0);
            case EXPR_LE:
                return CryData_boolean(strcmp(data1->value.string, data2->value.string) <= 0);
            case EXPR_GT:
                return CryData_boolean(strcmp(data1->value.string, data2->value.string) > 0);
            case EXPR_GE:
                return CryData_boolean(strcmp(data1->value.string, data2->value.string) >= 0);
        }
    }else if(data1->flags == DATA_BOOLEAN && data2->flags == DATA_BOOLEAN){
        switch(opcode){
            case EXPR_EQ:
                return CryData_boolean((data1->value.boolean && data2->value.boolean) || (!(data1->value.boolean) && !(data2->value.boolean)));
            case EXPR_NE:
                return CryData_boolean((!(data1->value.boolean) && data2->value.boolean) || (data1->value.boolean && !(data2->value.boolean)));
        }
    }

    return NULL;
}

CryData *CryExpr_eval_function(CryContext *context, CryExpr expr, CryData *function_name, CryData *data){
    int       i, arg_count;
    CryData  *result, *args;

    if(data == NULL) return (expr->function)(context, function_name, args);

    arg_count = CryArray_size(data->value.array);
    args      = CryData_array(arg_count);
    for(i = 0; i < arg_count; i++){
        CryArray_add(args->value.array, CryExpr_eval(context, CryArray_get(data->value.array, i)->value.expression));
    }
    result = (expr->function)(context, function_name, args);
    CryData_free(args);

    return result;
}


CryData *CryExpr_eval_array(CryContext *context, CryExpr expr) {
    int len;
    int i;
    CryData *result, *current;
    CryArray expr_array, result_array;

    expr_array = expr->data->value.array;
    len = CryArray_size(expr_array);
    result = CryData_array(len);
    result_array = result->value.array;

    for (i=0; i<len; i++) {
        current = CryArray_get(expr_array, i);
        CryArray_add(result_array, CryExpr_eval(context, current->value.expression));
        CryData_free(current);
    }

    return result;
}


void CryExpr_free(CryExpr expr){
    CryData_free(expr->data);
    free(expr);
}
