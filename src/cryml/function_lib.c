#include "cryml.h"

CryFunctionLib *CryFunctionLib_init(char *path){
    CryFunctionLib *library;

    library                 = malloc(sizeof(CryFunctionLib));
    library->path           = strdup(path);
    library->handler        = dlopen(library->path, RTLD_LAZY);
    library->functions      = CryStack_init();

    if( !(library->handler) ){
        free(library->path);
        // TODO: Handle this error properly
        fprintf(stderr, "%s\n", dlerror());
        return NULL;
    }

    if( !CryFunctionLib_init_library(library) ) return NULL;

    if(CryFunctionLibraries == NULL){
        CryFunctionLibraries = CryHash_init(ENV_FUNC_BUCKET_SIZE);
    }

    CryHash_add(CryFunctionLibraries, path, CryData_funclib(library));

    return library;
}

void CryFunctionLib_prepare(CryFunctionLib *library){
    data_allocated          = 0;
    library->error          = 0;
    library->data_allocated = 0;
}

void CryFunctionLib_release(CryFunctionLib *library){
    library->data_allocated = data_allocated;
}

void CryFunctionLib_finish(CryFunctionLib *library){
    data_allocated          += library->data_allocated;
    library->data_allocated = 0;
}

CryFunctionLib *CryFunctionLib_find(char *path){
    CryData *data;

    if(CryFunctionLibraries == NULL){
        return NULL;
    }else{
        data = CryHash_find(CryFunctionLibraries, path);
        if(data == NULL){
            return NULL;
        }else{
            return data->value.library;
        }
    }
}

int CryFunctionLib_init_library(CryFunctionLib *library){
    CryFunctionInitCallback *init_callback;

    init_callback = dlsym(library->handler, "Cryml_function_library_init");
    if(dlerror() != NULL) return FALSE;

    init_callback(library);
    CryFunctionLib_finish(library);

    return !(library->error);
}

int CryFunctionLib_add_function(CryFunctionLib *library, char *function_name, char *calling_name, int arity, CryFunctionLookupCallback function_lookup_callback){
    CryData *data;
    CryExpr  expression;

    data = CryData_array(2);
    CryArray_add(data->value.array, CryData_string(calling_name));
    CryArray_add(data->value.array, CryData_integer(arity));

    expression           = CryExpr_init(EXPR_FUNC, data);
    expression->function = dlsym(library->handler, function_name);
    if(dlerror() != NULL) return FALSE;

    if(function_lookup_callback == NULL){
        expression->function_lookup_callback = CryFunction_default_function_lookup_callback;
    }else{
        expression->function_lookup_callback = function_lookup_callback;
    }

    CryStack_push(library->functions, CryData_expression(expression));
    return TRUE;
}

void CryFunctionLib_free(CryFunctionLib *library){
    free(library->path);
    dlclose(library->handler);
    CryStack_free(library->functions);
    free(library);
}

void CryFunctionLib_lock(){
    pthread_mutex_lock(&CryGlobalFuncLock);
}

void CryFunctionLib_unlock(){
    pthread_mutex_unlock(&CryGlobalFuncLock);
}

void CryFunctionLib_apply(CryFunctionLib *library, CryEnv *env){
    CryStack stack;
    CryArray array;

    for(stack = library->functions->next; stack != NULL; stack = stack->next){
        array = stack->data->value.expression->data->value.array;
        if(env->functions == NULL) env->functions = CryHash_init(ENV_FUNC_BUCKET_SIZE);
        CryHash_push(
            env->functions,
            CryArray_get(array, 0)->value.string,
            stack->data
        );
    }
}

int CryFunction_default_function_lookup_callback(CryExpr expr, CryEnv *env, CryExpr proto_expr){
    int arity, args_count;

    arity      = (int)( CryArray_get(proto_expr->data->value.array, 1)->value.integer);
    args_count = CryArray_size(CryArray_get(expr->data->value.array, 1)->value.array);

    if(arity >= 0 && args_count == arity){
        expr->function = proto_expr->function;
        return TRUE;
    }else if(arity < 0 && args_count >= 1 - arity){
        expr->function = proto_expr->function;
        return TRUE;
    }

    return FALSE;
}