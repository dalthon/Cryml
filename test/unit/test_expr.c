#include "../../src/cryml/cryml.h"
#include "unit.h"

#define EXECUTE_TWICE(x) {{x} {x}}
/* I'm not as stupid as it sounds...          */
/* I'm evaluating twice to ensure that each   */
/* expression can be evaluated more than once */

int before_all_test_expr(void){ return 0; }
int after_all_test_expr(void){  return 0; }

CryData *sum_function(CryContext *context, CryData *function_name, CryData *args){
    char    *string;
    CryData *data1, *data2, *result;

    data1 = CryArray_get(args->value.array, 0);
    data2 = CryArray_get(args->value.array, 1);

    string = malloc(100*sizeof(char));
    sprintf(string, "function: %s %d + %d = %d", function_name->value.string, (int)(data1->value.integer), (int)(data2->value.integer), (int)(data1->value.integer + data2->value.integer));
    result = CryData_string(string);
    free(string);

    return result;
}

CryExpr expr_function(CryFunction function, char *function_name, CryData *args){
    CryData *data;
    CryExpr  expr;

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_string(function_name));
    CryArray_add(data->value.array, args);

    expr           = CryExpr_init(EXPR_FUNC, data);
    expr->function = function;

    return expr;
}

void test_eval_data_expr(void){
    CryExpr expr;

    expr = CryExpr_init(EXPR_EVAL, CryData_integer(1));
    EXECUTE_TWICE( CU_ASSERT_EQUAL(CryExpr_eval(NULL, expr)->value.integer, 1); );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_eval_attr_expr(void){
    CryEnv     *env;
    CryTag      tag;
    CryExpr     expr;
    CryContext *context;
    CryHash     ctx_data, ctx_extra;

    env       = CryEnv_init();
    ctx_data  = CryHash_init(3);
    ctx_extra = CryHash_init(3);
    context   = CryContext_init(env, ctx_data, ctx_extra);
    tag       = CryTag_init();
    CryContext_init_stack(context, tag);

    expr = CryExpr_init(EXPR_ATTR, CryData_string("attr_test"));
    CryContext_push_attribute(context, "attr_test", CryData_integer(42));
    EXECUTE_TWICE(
        CU_ASSERT_EQUAL(context->flags, 0);
        CU_ASSERT_EQUAL(CryExpr_eval(context, expr)->value.integer, 42);
    )
    CryExpr_free(expr);

    expr = CryExpr_init(EXPR_ATTR, CryData_string("missing_attr"));
    EXECUTE_TWICE(
        CU_ASSERT_EQUAL(CryExpr_eval(context, expr), NULL);
        CU_ASSERT_EQUAL(context->flags, ERROR_MISSING_ATTRIBUTE);
        CU_ASSERT_STRING_EQUAL(context->error_message, "Missing attribute @missing_attr");
    )
    CryExpr_free(expr);

    CryTag_free(tag);
    CryHash_free(ctx_data);
    CryHash_free(ctx_extra);
    CryContext_free(context);
    CryEnv_free(env);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_eval_attr_data_expr(void){
    CryEnv     *env;
    CryExpr     expr;
    CryContext *context;
    CryHash     ctx_data, ctx_extra;

    env       = CryEnv_init();
    ctx_data  = CryHash_init(3);
    ctx_extra = CryHash_init(3);

    CryHash_add(ctx_data, "attr_test", CryData_integer(42));
    context = CryContext_init(env, ctx_data, ctx_extra);

    expr = CryExpr_init(EXPR_DATA, CryData_string("attr_test"));
    EXECUTE_TWICE(
        CU_ASSERT_EQUAL(context->flags, 0);
        CU_ASSERT_EQUAL(CryExpr_eval(context, expr)->value.integer, 42);
    );
    CryExpr_free(expr);

    expr = CryExpr_init(EXPR_DATA, CryData_string("missing_attr"));
    EXECUTE_TWICE(
        CU_ASSERT_EQUAL(CryExpr_eval(context, expr), NULL);
        CU_ASSERT_EQUAL(context->flags, ERROR_MISSING_DATA_ATTRIBUTE);
        CU_ASSERT_STRING_EQUAL(context->error_message, "Missing data attribute $missing_attr");
    );
    CryExpr_free(expr);

    CryHash_free(ctx_data);
    CryHash_free(ctx_extra);
    CryContext_free(context);
    CryEnv_free(env);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_eval_add_expr(void){
    CryExpr  expr;
    CryData *data;

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_integer(1));
    CryArray_add(data->value.array, CryData_integer(2));

    expr = CryExpr_init(EXPR_ADD, data);
    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_EQUAL(data->value.integer, 3);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_integer(1) );
    CryArray_add(data->value.array, CryData_double(2.5));

    expr = CryExpr_init(EXPR_ADD, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_DOUBLE_EQUAL(data->value.cdouble, 3.5, CRYML_FLOAT_PRECISION);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_double(2.5));
    CryArray_add(data->value.array, CryData_integer(1) );

    expr = CryExpr_init(EXPR_ADD, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_DOUBLE_EQUAL(data->value.cdouble, 3.5, CRYML_FLOAT_PRECISION);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_double(1.0));
    CryArray_add(data->value.array, CryData_double(2.5));

    expr = CryExpr_init(EXPR_ADD, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_DOUBLE_EQUAL(data->value.cdouble, 3.5, CRYML_FLOAT_PRECISION);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_eval_sub_expr(void){
    CryExpr expr;
    CryData *data;

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_integer(1));
    CryArray_add(data->value.array, CryData_integer(2));

    expr = CryExpr_init(EXPR_SUB, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_EQUAL(data->value.integer, -1);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_integer(1) );
    CryArray_add(data->value.array, CryData_double(2.5));

    expr = CryExpr_init(EXPR_SUB, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_DOUBLE_EQUAL(data->value.cdouble, -1.5, CRYML_FLOAT_PRECISION);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_double(2.5));
    CryArray_add(data->value.array, CryData_integer(1) );

    expr = CryExpr_init(EXPR_SUB, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_DOUBLE_EQUAL(data->value.cdouble, 1.5, CRYML_FLOAT_PRECISION);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_double(1.0));
    CryArray_add(data->value.array, CryData_double(2.5));

    expr = CryExpr_init(EXPR_SUB, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_DOUBLE_EQUAL(data->value.cdouble, -1.5, CRYML_FLOAT_PRECISION);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_eval_mul_expr(void){
    CryExpr expr;
    CryData *data;

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_integer(3));
    CryArray_add(data->value.array, CryData_integer(2));

    expr = CryExpr_init(EXPR_MUL, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_EQUAL(data->value.integer, 6);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_integer(-2) );
    CryArray_add(data->value.array, CryData_double(2.5));

    expr = CryExpr_init(EXPR_MUL, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_DOUBLE_EQUAL(data->value.cdouble, -5.0, CRYML_FLOAT_PRECISION);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_double(2.5));
    CryArray_add(data->value.array, CryData_integer(1) );

    expr = CryExpr_init(EXPR_MUL, data);
    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_DOUBLE_EQUAL(data->value.cdouble, 2.5, CRYML_FLOAT_PRECISION);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_double(-1.0));
    CryArray_add(data->value.array, CryData_double(2.5));

    expr = CryExpr_init(EXPR_MUL, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_DOUBLE_EQUAL(data->value.cdouble, -2.5, CRYML_FLOAT_PRECISION);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_eval_div_expr(void){
    CryExpr expr;
    CryData *data;
    CryContext *context;
    CryEnv *env;

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_integer(3));
    CryArray_add(data->value.array, CryData_integer(2));

    env     = CryEnv_init();
    context = CryContext_init(env, NULL, NULL);

    expr = CryExpr_init(EXPR_DIV, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(context, expr);
        CU_ASSERT_EQUAL(data->value.integer, 1);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);
    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_integer(-2) );
    CryArray_add(data->value.array, CryData_double(2.5));

    expr = CryExpr_init(EXPR_DIV, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(context, expr);
        CU_ASSERT_DOUBLE_EQUAL(data->value.cdouble, -0.8, CRYML_FLOAT_PRECISION);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_double(2.5));
    CryArray_add(data->value.array, CryData_integer(2) );

    expr = CryExpr_init(EXPR_DIV, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(context, expr);
        CU_ASSERT_DOUBLE_EQUAL(data->value.cdouble, 1.25, CRYML_FLOAT_PRECISION);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_double(-1.0));
    CryArray_add(data->value.array, CryData_double(2.5));

    expr = CryExpr_init(EXPR_DIV, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(context, expr);
        CU_ASSERT_DOUBLE_EQUAL(data->value.cdouble, -0.4, CRYML_FLOAT_PRECISION);
        CryData_free(data);
    )

    CryExpr_free(expr);
    CryContext_free(context);
    CryEnv_free(env);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_eval_div_expr_error(void){
    CryExpr expr;
    CryData *data;
    CryContext *context;
    CryEnv *env;

    env     = CryEnv_init();
    context = CryContext_init(env, NULL, NULL);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_integer(3));
    CryArray_add(data->value.array, CryData_integer(0));

    expr = CryExpr_init(EXPR_DIV, data);

    EXECUTE_TWICE(
        CU_ASSERT_EQUAL(CryExpr_eval(context, expr), NULL);
        CU_ASSERT_EQUAL(context->flags, ERROR_DIVISION_BY_ZERO);
        CU_ASSERT_STRING_EQUAL(context->error_message, "Invalid division 3/0");
    );

    CryExpr_free(expr);
    CryContext_cleanup(context);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_double(3.5));
    CryArray_add(data->value.array, CryData_integer(0));

    expr = CryExpr_init(EXPR_DIV, data);

    EXECUTE_TWICE(
        CU_ASSERT_EQUAL(CryExpr_eval(context, expr), NULL);
        CU_ASSERT_EQUAL(context->flags, ERROR_DIVISION_BY_ZERO);
        CU_ASSERT_STRING_EQUAL(context->error_message, "Invalid division 3.500000/0");
    );

    CryExpr_free(expr);
    CryContext_free(context);
    CryEnv_free(env);
}

// TODO: Handle decently double mod operations
void test_eval_mod_expr(void){
    CryExpr expr;
    CryData *data;

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_integer(3));
    CryArray_add(data->value.array, CryData_integer(2));

    expr = CryExpr_init(EXPR_MOD, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_EQUAL(data->value.integer, 1);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_integer(-2) );
    CryArray_add(data->value.array, CryData_double(2.5));

    expr = CryExpr_init(EXPR_MOD, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_EQUAL(data, NULL);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_double(2.5));
    CryArray_add(data->value.array, CryData_integer(2) );

    expr = CryExpr_init(EXPR_MOD, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_EQUAL(data, NULL);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_double(-1.0));
    CryArray_add(data->value.array, CryData_double(2.5));

    expr = CryExpr_init(EXPR_MOD, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_EQUAL(data, NULL);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_eval_not_expr(void){
    CryExpr expr;
    CryData *data;

    expr = CryExpr_init(EXPR_NOT, CryData_boolean(TRUE));

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    expr = CryExpr_init(EXPR_NOT, CryData_boolean(FALSE));

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_eval_and_expr(void){
    CryExpr expr;
    CryData *data;

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_boolean(TRUE));
    CryArray_add(data->value.array, CryData_boolean(TRUE));

    expr = CryExpr_init(EXPR_AND, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_boolean(TRUE));
    CryArray_add(data->value.array, CryData_boolean(FALSE));

    expr = CryExpr_init(EXPR_AND, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_boolean(FALSE));
    CryArray_add(data->value.array, CryData_boolean(TRUE));

    expr = CryExpr_init(EXPR_AND, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_boolean(FALSE));
    CryArray_add(data->value.array, CryData_boolean(FALSE));

    expr = CryExpr_init(EXPR_AND, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_eval_or_expr(void){
    CryExpr expr;
    CryData *data;

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_boolean(TRUE));
    CryArray_add(data->value.array, CryData_boolean(TRUE));

    expr = CryExpr_init(EXPR_OR, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_boolean(TRUE));
    CryArray_add(data->value.array, CryData_boolean(FALSE));

    expr = CryExpr_init(EXPR_OR, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_boolean(FALSE));
    CryArray_add(data->value.array, CryData_boolean(TRUE));

    expr = CryExpr_init(EXPR_OR, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_boolean(FALSE));
    CryArray_add(data->value.array, CryData_boolean(FALSE));

    expr = CryExpr_init(EXPR_OR, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_eval_boolean_eq_expr(void){
    CryExpr expr;
    CryData *data;

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_boolean(FALSE));
    CryArray_add(data->value.array, CryData_boolean(FALSE));

    expr = CryExpr_init(EXPR_EQ, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_boolean(TRUE));
    CryArray_add(data->value.array, CryData_boolean(1000));

    expr = CryExpr_init(EXPR_EQ, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_boolean(FALSE));
    CryArray_add(data->value.array, CryData_boolean(TRUE));

    expr = CryExpr_init(EXPR_EQ, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_boolean(TRUE));
    CryArray_add(data->value.array, CryData_boolean(FALSE));

    expr = CryExpr_init(EXPR_EQ, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_eval_boolean_ne_expr(void){
    CryExpr expr;
    CryData *data;

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_boolean(FALSE));
    CryArray_add(data->value.array, CryData_boolean(FALSE));

    expr = CryExpr_init(EXPR_NE, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_boolean(TRUE));
    CryArray_add(data->value.array, CryData_boolean(1000));

    expr = CryExpr_init(EXPR_NE, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_boolean(FALSE));
    CryArray_add(data->value.array, CryData_boolean(TRUE));

    expr = CryExpr_init(EXPR_NE, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_boolean(TRUE));
    CryArray_add(data->value.array, CryData_boolean(FALSE));

    expr = CryExpr_init(EXPR_NE, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_eval_string_eq_expr(void){
    CryExpr expr;
    CryData *data;

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_string("EQUAL"));
    CryArray_add(data->value.array, CryData_string("EQUAL"));

    expr = CryExpr_init(EXPR_EQ, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_string("NOT equal"));
    CryArray_add(data->value.array, CryData_string("not EQUAL"));

    expr = CryExpr_init(EXPR_EQ, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_eval_string_ne_expr(void){
    CryExpr expr;
    CryData *data;

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_string("EQUAL"));
    CryArray_add(data->value.array, CryData_string("EQUAL"));

    expr = CryExpr_init(EXPR_NE, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_string("NOT equal"));
    CryArray_add(data->value.array, CryData_string("not EQUAL"));

    expr = CryExpr_init(EXPR_NE, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_eval_string_lt_expr(void){
    CryExpr expr;
    CryData *data;

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_string("ant"));
    CryArray_add(data->value.array, CryData_string("elephant"));

    expr = CryExpr_init(EXPR_LT, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_string("elephant"));
    CryArray_add(data->value.array, CryData_string("ant"));

    expr = CryExpr_init(EXPR_LT, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_string("elephant"));
    CryArray_add(data->value.array, CryData_string("elephant"));

    expr = CryExpr_init(EXPR_LT, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_eval_string_le_expr(void){
    CryExpr expr;
    CryData *data;

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_string("ant"));
    CryArray_add(data->value.array, CryData_string("elephant"));

    expr = CryExpr_init(EXPR_LE, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_string("elephant"));
    CryArray_add(data->value.array, CryData_string("ant"));

    expr = CryExpr_init(EXPR_LE, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_string("elephant"));
    CryArray_add(data->value.array, CryData_string("elephant"));

    expr = CryExpr_init(EXPR_LE, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_eval_string_gt_expr(void){
    CryExpr expr;
    CryData *data;

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_string("ant"));
    CryArray_add(data->value.array, CryData_string("elephant"));

    expr = CryExpr_init(EXPR_GT, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_string("elephant"));
    CryArray_add(data->value.array, CryData_string("ant"));

    expr = CryExpr_init(EXPR_GT, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_string("elephant"));
    CryArray_add(data->value.array, CryData_string("elephant"));

    expr = CryExpr_init(EXPR_GT, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_eval_string_ge_expr(void){
    CryExpr expr;
    CryData *data;

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_string("ant"));
    CryArray_add(data->value.array, CryData_string("elephant"));

    expr = CryExpr_init(EXPR_GE, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_string("elephant"));
    CryArray_add(data->value.array, CryData_string("ant"));

    expr = CryExpr_init(EXPR_GE, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_string("elephant"));
    CryArray_add(data->value.array, CryData_string("elephant"));

    expr = CryExpr_init(EXPR_GE, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_eval_numeric_eq_expr(void){
    CryExpr expr;
    CryData *data;

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_integer(42));
    CryArray_add(data->value.array, CryData_integer(42));

    expr = CryExpr_init(EXPR_EQ, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_integer(42));
    CryArray_add(data->value.array, CryData_integer(24));

    expr = CryExpr_init(EXPR_EQ, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_double(42.0));
    CryArray_add(data->value.array, CryData_integer(42));

    expr = CryExpr_init(EXPR_EQ, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_integer(24));
    CryArray_add(data->value.array, CryData_double(42.0));

    expr = CryExpr_init(EXPR_EQ, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_double(42.0));
    CryArray_add(data->value.array, CryData_double(42.0));

    expr = CryExpr_init(EXPR_EQ, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_double(42.0));
    CryArray_add(data->value.array, CryData_double(24.0));

    expr = CryExpr_init(EXPR_EQ, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_eval_numeric_ne_expr(void){
    CryExpr expr;
    CryData *data;

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_integer(42));
    CryArray_add(data->value.array, CryData_integer(42));

    expr = CryExpr_init(EXPR_NE, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_integer(42));
    CryArray_add(data->value.array, CryData_integer(24));

    expr = CryExpr_init(EXPR_NE, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_double(42.0));
    CryArray_add(data->value.array, CryData_integer(42));

    expr = CryExpr_init(EXPR_NE, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_integer(24));
    CryArray_add(data->value.array, CryData_double(42.0));

    expr = CryExpr_init(EXPR_NE, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_double(42.0));
    CryArray_add(data->value.array, CryData_double(42.0));

    expr = CryExpr_init(EXPR_NE, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_double(42.0));
    CryArray_add(data->value.array, CryData_double(24.0));

    expr = CryExpr_init(EXPR_NE, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_eval_numeric_lt_expr(void){
    CryExpr expr;
    CryData *data;

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_integer(24));
    CryArray_add(data->value.array, CryData_integer(42));

    expr = CryExpr_init(EXPR_LT, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_integer(42));
    CryArray_add(data->value.array, CryData_integer(24));

    expr = CryExpr_init(EXPR_LT, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_integer(42));
    CryArray_add(data->value.array, CryData_integer(42));

    expr = CryExpr_init(EXPR_LT, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_double(24.0));
    CryArray_add(data->value.array, CryData_integer(42));

    expr = CryExpr_init(EXPR_LT, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_double(42.0));
    CryArray_add(data->value.array, CryData_integer(24));

    expr = CryExpr_init(EXPR_LT, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_double(42.0));
    CryArray_add(data->value.array, CryData_integer(42));

    expr = CryExpr_init(EXPR_LT, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_integer(24));
    CryArray_add(data->value.array, CryData_double(42.0));

    expr = CryExpr_init(EXPR_LT, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_integer(42.0));
    CryArray_add(data->value.array, CryData_double(24.0));

    expr = CryExpr_init(EXPR_LT, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_integer(42));
    CryArray_add(data->value.array, CryData_double(42.0));

    expr = CryExpr_init(EXPR_LT, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_double(24.0));
    CryArray_add(data->value.array, CryData_double(42.0));

    expr = CryExpr_init(EXPR_LT, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_double(42.0));
    CryArray_add(data->value.array, CryData_double(24.0));

    expr = CryExpr_init(EXPR_LT, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_double(42.0));
    CryArray_add(data->value.array, CryData_double(42.0));

    expr = CryExpr_init(EXPR_LT, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_eval_numeric_le_expr(void){
    CryExpr expr;
    CryData *data;

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_integer(24));
    CryArray_add(data->value.array, CryData_integer(42));

    expr = CryExpr_init(EXPR_LE, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_integer(42));
    CryArray_add(data->value.array, CryData_integer(24));

    expr = CryExpr_init(EXPR_LE, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_integer(42));
    CryArray_add(data->value.array, CryData_integer(42));

    expr = CryExpr_init(EXPR_LE, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_double(24.0));
    CryArray_add(data->value.array, CryData_integer(42));

    expr = CryExpr_init(EXPR_LE, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_double(42.0));
    CryArray_add(data->value.array, CryData_integer(24));

    expr = CryExpr_init(EXPR_LE, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_double(42.0));
    CryArray_add(data->value.array, CryData_integer(42));

    expr = CryExpr_init(EXPR_LE, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_integer(24));
    CryArray_add(data->value.array, CryData_double(42.0));

    expr = CryExpr_init(EXPR_LE, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_integer(42.0));
    CryArray_add(data->value.array, CryData_double(24.0));

    expr = CryExpr_init(EXPR_LE, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_integer(42));
    CryArray_add(data->value.array, CryData_double(42.0));

    expr = CryExpr_init(EXPR_LE, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_double(24.0));
    CryArray_add(data->value.array, CryData_double(42.0));

    expr = CryExpr_init(EXPR_LE, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_double(42.0));
    CryArray_add(data->value.array, CryData_double(24.0));

    expr = CryExpr_init(EXPR_LE, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_double(42.0));
    CryArray_add(data->value.array, CryData_double(42.0));

    expr = CryExpr_init(EXPR_LE, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_eval_numeric_gt_expr(void){
    CryExpr expr;
    CryData *data;

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_integer(24));
    CryArray_add(data->value.array, CryData_integer(42));

    expr = CryExpr_init(EXPR_GT, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_integer(42));
    CryArray_add(data->value.array, CryData_integer(24));

    expr = CryExpr_init(EXPR_GT, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_integer(42));
    CryArray_add(data->value.array, CryData_integer(42));

    expr = CryExpr_init(EXPR_GT, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_double(24.0));
    CryArray_add(data->value.array, CryData_integer(42));

    expr = CryExpr_init(EXPR_GT, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_double(42.0));
    CryArray_add(data->value.array, CryData_integer(24));

    expr = CryExpr_init(EXPR_GT, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_double(42.0));
    CryArray_add(data->value.array, CryData_integer(42));

    expr = CryExpr_init(EXPR_GT, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_integer(24));
    CryArray_add(data->value.array, CryData_double(42.0));

    expr = CryExpr_init(EXPR_GT, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_integer(42.0));
    CryArray_add(data->value.array, CryData_double(24.0));

    expr = CryExpr_init(EXPR_GT, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_integer(42));
    CryArray_add(data->value.array, CryData_double(42.0));

    expr = CryExpr_init(EXPR_GT, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_double(24.0));
    CryArray_add(data->value.array, CryData_double(42.0));

    expr = CryExpr_init(EXPR_GT, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_double(42.0));
    CryArray_add(data->value.array, CryData_double(24.0));

    expr = CryExpr_init(EXPR_GT, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_double(42.0));
    CryArray_add(data->value.array, CryData_double(42.0));

    expr = CryExpr_init(EXPR_GT, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_eval_numeric_ge_expr(void){
    CryExpr expr;
    CryData *data;

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_integer(24));
    CryArray_add(data->value.array, CryData_integer(42));

    expr = CryExpr_init(EXPR_GE, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_integer(42));
    CryArray_add(data->value.array, CryData_integer(24));

    expr = CryExpr_init(EXPR_GE, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_integer(42));
    CryArray_add(data->value.array, CryData_integer(42));

    expr = CryExpr_init(EXPR_GE, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_double(24.0));
    CryArray_add(data->value.array, CryData_integer(42));

    expr = CryExpr_init(EXPR_GE, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_double(42.0));
    CryArray_add(data->value.array, CryData_integer(24));

    expr = CryExpr_init(EXPR_GE, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_double(42.0));
    CryArray_add(data->value.array, CryData_integer(42));

    expr = CryExpr_init(EXPR_GE, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_integer(24));
    CryArray_add(data->value.array, CryData_double(42.0));

    expr = CryExpr_init(EXPR_GE, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_integer(42.0));
    CryArray_add(data->value.array, CryData_double(24.0));

    expr = CryExpr_init(EXPR_GE, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_integer(42));
    CryArray_add(data->value.array, CryData_double(42.0));

    expr = CryExpr_init(EXPR_GE, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_double(24.0));
    CryArray_add(data->value.array, CryData_double(42.0));

    expr = CryExpr_init(EXPR_GE, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_double(42.0));
    CryArray_add(data->value.array, CryData_double(24.0));

    expr = CryExpr_init(EXPR_GE, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_double(42.0));
    CryArray_add(data->value.array, CryData_double(42.0));

    expr = CryExpr_init(EXPR_GE, data);

    EXECUTE_TWICE(
        data = CryExpr_eval(NULL, expr);
        CU_ASSERT(data->value.boolean);
        CryData_free(data);
    );

    CryExpr_free(expr);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_eval_function_call(void){
    CryEnv     *env;
    CryTag      tag;
    CryData    *data, *args;
    CryExpr     expr;
    CryContext *context;
    CryHash     ctx_data, ctx_extra;

    env       = CryEnv_init();
    ctx_data  = CryHash_init(3);
    ctx_extra = CryHash_init(3);
    context   = CryContext_init(env, ctx_data, ctx_extra);
    tag       = CryTag_init();
    CryContext_init_stack(context, tag);

    args = CryData_array(2);
    expr = CryExpr_init(EXPR_EVAL, CryData_integer(123));
    CryArray_add(args->value.array, CryData_expression(expr));
    expr = CryExpr_init(EXPR_EVAL, CryData_integer(321));
    CryArray_add(args->value.array, CryData_expression(expr));

    expr = expr_function(sum_function, "function_1", args);
    EXECUTE_TWICE(
        data = CryExpr_eval(context, expr);
        CU_ASSERT_STRING_EQUAL(data->value.string, "function: function_1 123 + 321 = 444");
        CryData_free(data);
    );
    CryExpr_free(expr);

    CryTag_free(tag);
    CryHash_free(ctx_data);
    CryHash_free(ctx_extra);
    CryContext_free(context);
    CryEnv_free(env);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_eval_complex_expr(void){
    CryEnv     *env;
    CryTag      tag;
    CryData    *data, *tmp_data;
    CryExpr     expr, aux_expr, expr1, expr2;
    CryContext *context;
    CryHash     ctx_data, ctx_extra;

    /*
        d1    = 21.0
        d2    = 84
        attr1 = 42
        attr2 = 21.0
        !(
          (attr1 == d1 + attr2)
          &&
          (
            (d2 < attr2)
            ||
            (attr1 > attr2)
          )
        )
    */

    env       = CryEnv_init();
    ctx_data  = CryHash_init(3);
    ctx_extra = CryHash_init(3);
    context   = CryContext_init(env, ctx_data, ctx_extra);
    tag       = CryTag_init();
    CryContext_init_stack(context, tag);

    CryContext_push_attribute(context, "attr1", CryData_integer(42));
    CryContext_push_attribute(context, "attr2", CryData_double(21.0));

    expr1 = CryExpr_init(EXPR_ATTR, CryData_string("attr1"));
    expr2 = CryExpr_init(EXPR_ATTR, CryData_string("attr2"));
    data  = CryData_array(2);
    CryArray_add(data->value.array, CryData_expression(expr1));
    CryArray_add(data->value.array, CryData_expression(expr2));
    expr = CryExpr_init(EXPR_GT, data);

    EXECUTE_TWICE(
        tmp_data = CryExpr_eval(context, expr);
        CU_ASSERT(tmp_data->value.boolean);
        CryData_free(tmp_data);
    );

    expr1 = CryExpr_init(EXPR_EVAL, CryData_integer(84));
    expr2 = CryExpr_init(EXPR_ATTR, CryData_string("attr2"));
    data  = CryData_array(2);
    CryArray_add(data->value.array, CryData_expression(expr1));
    CryArray_add(data->value.array, CryData_expression(expr2));
    aux_expr = CryExpr_init(EXPR_LT, data);

    EXECUTE_TWICE(
        tmp_data = CryExpr_eval(context, aux_expr);
        CU_ASSERT_FALSE(tmp_data->value.boolean);
        CryData_free(tmp_data);
    );

    expr1 = expr;
    expr2 = aux_expr;
    data  = CryData_array(2);
    CryArray_add(data->value.array, CryData_expression(expr1));
    CryArray_add(data->value.array, CryData_expression(expr2));
    aux_expr = CryExpr_init(EXPR_OR, data);

    EXECUTE_TWICE(
        tmp_data = CryExpr_eval(context, aux_expr);
        CU_ASSERT(tmp_data->value.boolean);
        CryData_free(tmp_data);
    );

    expr1 = CryExpr_init(EXPR_EVAL, CryData_double(21.0));
    expr2 = CryExpr_init(EXPR_ATTR, CryData_string("attr2"));
    data  = CryData_array(2);
    CryArray_add(data->value.array, CryData_expression(expr1));
    CryArray_add(data->value.array, CryData_expression(expr2));
    expr = CryExpr_init(EXPR_ADD, data);

    EXECUTE_TWICE(
        tmp_data = CryExpr_eval(context, expr);
        CU_ASSERT_DOUBLE_EQUAL(tmp_data->value.cdouble, 42.0, CRYML_FLOAT_PRECISION);
        CryData_free(tmp_data);
    );

    expr1 = CryExpr_init(EXPR_ATTR, CryData_string("attr1"));
    expr2 = expr;
    data  = CryData_array(2);
    CryArray_add(data->value.array, CryData_expression(expr1));
    CryArray_add(data->value.array, CryData_expression(expr2));
    expr = CryExpr_init(EXPR_EQ, data);

    EXECUTE_TWICE(
        tmp_data = CryExpr_eval(context, expr);
        CU_ASSERT(tmp_data->value.boolean);
        CryData_free(tmp_data);
    );

    expr1 = expr;
    expr2 = aux_expr;
    data  = CryData_array(2);
    CryArray_add(data->value.array, CryData_expression(expr1));
    CryArray_add(data->value.array, CryData_expression(expr2));
    expr = CryExpr_init(EXPR_AND, data);

    EXECUTE_TWICE(
        tmp_data = CryExpr_eval(context, expr);
        CU_ASSERT(tmp_data->value.boolean);
        CryData_free(tmp_data);
    );

    expr = CryExpr_init(EXPR_NOT, CryData_expression(expr));

    EXECUTE_TWICE(
        data = CryExpr_eval(context, expr);
        CU_ASSERT_FALSE(data->value.boolean);
        CryData_free(data);
    );

    CryTag_free(tag);
    CryHash_free(ctx_data);
    CryHash_free(ctx_extra);
    CryEnv_free(env);
    CryExpr_free(expr);
    CryContext_free(context);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

CU_ErrorCode test_expr(CU_pSuite suite){
    suite = CU_add_suite("CryExpr Tests (test_expr.c)", before_all_test_expr, after_all_test_expr);
    if(suite == NULL){
       CU_cleanup_registry();
       return CU_get_error();
    }

    if( (CU_add_test(suite, "Eval data expression                                 ", test_eval_data_expr            ) == NULL ) ||
        (CU_add_test(suite, "Eval attribute expression                            ", test_eval_attr_expr            ) == NULL ) ||
        (CU_add_test(suite, "Eval attribute data expression                       ", test_eval_attr_data_expr       ) == NULL ) ||
        (CU_add_test(suite, "Eval arithmetic add expression                       ", test_eval_add_expr             ) == NULL ) ||
        (CU_add_test(suite, "Eval arithmetic sub expression                       ", test_eval_sub_expr             ) == NULL ) ||
        (CU_add_test(suite, "Eval arithmetic mul expression                       ", test_eval_mul_expr             ) == NULL ) ||
        (CU_add_test(suite, "Eval arithmetic div expression                       ", test_eval_div_expr             ) == NULL ) ||
        (CU_add_test(suite, "Eval arithmetic div expression error                 ", test_eval_div_expr_error       ) == NULL ) ||
        (CU_add_test(suite, "Eval arithmetic mod expression                       ", test_eval_mod_expr             ) == NULL ) ||
        (CU_add_test(suite, "Eval boolean not expression                          ", test_eval_not_expr             ) == NULL ) ||
        (CU_add_test(suite, "Eval boolean and expression                          ", test_eval_and_expr             ) == NULL ) ||
        (CU_add_test(suite, "Eval boolean or expression                           ", test_eval_or_expr              ) == NULL ) ||
        (CU_add_test(suite, "Eval compare boolean equal expression                ", test_eval_boolean_eq_expr      ) == NULL ) ||
        (CU_add_test(suite, "Eval compare boolean not equal expression            ", test_eval_boolean_ne_expr      ) == NULL ) ||
        (CU_add_test(suite, "Eval compare string equal expression                 ", test_eval_string_eq_expr       ) == NULL ) ||
        (CU_add_test(suite, "Eval compare string not equal expression             ", test_eval_string_ne_expr       ) == NULL ) ||
        (CU_add_test(suite, "Eval compare string less than expression             ", test_eval_string_lt_expr       ) == NULL ) ||
        (CU_add_test(suite, "Eval compare string less or equal than expression    ", test_eval_string_le_expr       ) == NULL ) ||
        (CU_add_test(suite, "Eval compare string greater than expression          ", test_eval_string_gt_expr       ) == NULL ) ||
        (CU_add_test(suite, "Eval compare string greater or equal than expression ", test_eval_string_ge_expr       ) == NULL ) ||
        (CU_add_test(suite, "Eval compare numeric equal expression                ", test_eval_numeric_eq_expr      ) == NULL ) ||
        (CU_add_test(suite, "Eval compare numeric not equal expression            ", test_eval_numeric_ne_expr      ) == NULL ) ||
        (CU_add_test(suite, "Eval compare numeric less than expression            ", test_eval_numeric_lt_expr      ) == NULL ) ||
        (CU_add_test(suite, "Eval compare numeric less or equal than expression   ", test_eval_numeric_le_expr      ) == NULL ) ||
        (CU_add_test(suite, "Eval compare numeric greater than expression         ", test_eval_numeric_gt_expr      ) == NULL ) ||
        (CU_add_test(suite, "Eval compare numeric greater or equal than expression", test_eval_numeric_ge_expr      ) == NULL ) ||
        (CU_add_test(suite, "Eval function call                                   ", test_eval_function_call        ) == NULL ) ||
        (CU_add_test(suite, "Eval complex expressions                             ", test_eval_complex_expr         ) == NULL ) ){
       CU_cleanup_registry();
       return CU_get_error();
    }

    return CUE_SUCCESS;
}
