#include "../cryml.h"

#define RAISE_IF_ERROR(context) if(CryContext_error(context)) return CryContext_error(context)

int CryLibCore_if_tag_function(CryTag parent_tag, CryTag tag, CryContext *context, FILE *output){
    int       condition, check_count;
    CryData  *data, *is_data;

    data = CryContext_current_attr(context, "condition");
    if(data != NULL){
        condition = CryData_is_true(data);
    }else{
        is_data = CryContext_current_attr(context, "is");
        if(is_data == NULL){
            CryContext_raise(context, ERROR_ARGUMMENT_ERROR, "Missing attribute \"is\" or \"condition\" on <if>");
            CryContext_pop_stack(context);
            return ERROR_ARGUMMENT_ERROR;
        }else{
            check_count = 0;
            condition = CryLibCore_if_compare(context, is_data, "greater_than",     EXPR_GT, &check_count)
                     && CryLibCore_if_compare(context, is_data, "lower_than",       EXPR_LT, &check_count)
                     && CryLibCore_if_compare(context, is_data, "equal_than",       EXPR_EQ, &check_count)
                     && CryLibCore_if_compare(context, is_data, "not_greater_than", EXPR_LE, &check_count)
                     && CryLibCore_if_compare(context, is_data, "not_lower_than",   EXPR_GE, &check_count)
                     && CryLibCore_if_compare(context, is_data, "not_equal_than",   EXPR_NE, &check_count);

            if(check_count == 0){
                condition = CryData_is_true(is_data);
            }

            CryContext_push_attribute(context, "condition", CryData_boolean(condition));
        }
    }

    data = CryContext_current_attr(context, "then");
    if(data){
        if(condition) CryRender_eval_tags_childs_function(tag, context, output);
     }else{
         CryContext_push_attribute(context, ".cryml.condition", CryData_boolean(condition));
         CryRender_eval_tags_childs_function(tag, context, output);
     }

    return 0;
}

int CryLibCore_if_compare(CryContext *context, CryData *is_data, char *attr_name, int opcode, int *check_count){
    int condition;
    CryData *data, *cmp_data;

    data = CryContext_current_attr(context, attr_name);
    if(data){
        (*check_count)++;
        CryData_referenced(is_data);
        CryData_referenced(data);
        cmp_data  = CryExpr_eval_comparable(is_data, opcode, data);
        condition = CryData_is_true(cmp_data);
        CryData_free(cmp_data);
        CryData_free(is_data);
        CryData_free(data);
        return condition;
    }else{
        return TRUE;
    }
}

int CryLibCore_then_tag_function(CryTag parent_tag, CryTag tag, CryContext *context, FILE *output){
    int      result;
    CryData *data;

    data = CryContext_attribute_lookup(context, ".cryml.condition");
    if(data == NULL){
        CryContext_pop_attribute(context, ".cryml.condition");
        result = CryRender_eval_tags_childs_function(tag, context, output);
        CryHash_push(context->attributes, ".cryml.condition", data);
        return result;
    }

    if(CryData_is_true(data)){
        CryData_referenced(data);
        CryContext_pop_attribute(context, ".cryml.condition");
        result = CryRender_eval_tags_childs_function(tag, context, output);
        CryHash_push(context->attributes, ".cryml.condition", data);
        CryData_free(data);
        return result;
    }

    return TRUE;
}

int CryLibCore_else_tag_function(CryTag parent_tag, CryTag tag, CryContext *context, FILE *output){
    int      result;
    CryData *data;

    data = CryContext_attribute_lookup(context, ".cryml.condition");
    if(data == NULL) return TRUE;

    if(!CryData_is_true(data)){
        CryData_referenced(data);
        CryContext_pop_attribute(context, ".cryml.condition");
        result = CryRender_eval_tags_childs_function(tag, context, output);
        CryHash_push(context->attributes, ".cryml.condition", data);
        CryData_free(data);
        return result;
    }

    return TRUE;
}

int CryLibCore_each_tag_function(CryTag parent_tag, CryTag tag, CryContext *context, FILE *output){
    CryData *attr_data, *in_data, *current;
    char *attr_name;
    CryIterable in_iterable;
    CryIterator in_iterator;

    attr_data = CryContext_current_attr(context, "attr");
    in_data   = CryContext_current_attr(context, "in");

    if (attr_data == NULL || in_data == NULL) {
        CryContext_raise(context, ERROR_ARGUMMENT_ERROR, "Missing attribute \"in\" or \"attr\" on <each>");
        CryContext_pop_stack(context);
        return ERROR_ARGUMMENT_ERROR;
    }

    // TODO: check passed attributes has correct types

    attr_name = attr_data->value.string;
    in_iterable = CryData_to_iterable(in_data);
    in_iterator = CryIterable_start(in_iterable);
    while ((current = CryIterator_next(in_iterator))) {
        CryContext_push_attribute(context, attr_name, current);
        CryRender_eval_tags_childs_function(tag, context, output);
        CryContext_pop_attribute(context, attr_name);
    }
    CryIterator_free(in_iterator);

    return 0;
}

int CryLibCore_attributes_tag_function(CryTag parent_tag, CryTag tag, CryContext *context, FILE *output){
    return CryRender_eval_tags_childs_function(tag, context, output);
}

int CryLibCore_taglib_tag_function(CryTag parent_tag, CryTag tag, CryContext *context, FILE *output){
    CryTagInitCallback *init_callback;

    void *handler;
    char *path;

    // CryData *tags;

    if(context->environment->status & ENV_PARSING_MASK){
        // CryHash_push(context->extra, ".cryml.taglib", CryData_stack());
        if(tag->flags & TAG_SINGLETON){
            path          = CryContext_current_attr(context, "src")->value.string;
            handler       = dlopen(path, RTLD_LAZY);
            init_callback = dlsym(handler, "Cryml_taglib_init");

            init_callback(context);
        }else{

        }
        // tags = CryHash_top(context->extra, ".cryml.taglib");
        // CryHash_pop(context->extra, ".cryml.taglib");
    }else{
        if( !(tag->flags & TAG_PRE_RENDER) ) fprintf(stderr, "WARNING: <taglib> should be marked as pre evaluated!\n");
    }
    return TRUE;
}

void CryLibCore_taglib_tag_lookup_callback(CryTag tag, CryEnv *env, CryExpr expression){
    CryTag_default_tag_lookup_callback(tag, env, expression);
    // TODO: Ignore tag lookups from here...
}

int CryLibCore_define_tag_function(CryTag parent_tag, CryTag tag, CryContext *context, FILE *output){
    return TRUE;
}

void CryLibCore_define_tag_lookup_callback(CryTag tag, CryEnv *env, CryExpr expression){
    CryHash_push(env->extra, ".cryml.taglib", CryData_tag(tag));
}

int CryLibCore_funclib_tag_function(CryTag parent_tag, CryTag tag, CryContext *context, FILE *output){
    char           *path;
    CryData        *data_path;
    CryFunctionLib *library;

    if(context->environment->status & ENV_PARSING_MASK){
        data_path = CryContext_current_attr(context, "src");

        if(data_path){
            path = data_path->value.string;

            CryEnv_lock(context->environment);
            CryFunctionLib_lock();

            library = CryFunctionLib_find(path);
            if(library == NULL) library = CryFunctionLib_init(path);

            CryFunctionLib_unlock();

            CryFunctionLib_apply(library, context->environment);

            CryEnv_unlock(context->environment);
            return TRUE;
        }else{
            // TODO: Handle path not found
            return FALSE;
        }
    }else{
        // TODO: Think about flags, it should be context flags!
        if( !(tag->flags & TAG_PRE_RENDER) ) fprintf(stderr, "WARNING: <funclib> should be marked as pre evaluated!\n");
    }

    return TRUE;
}

CryData *CryLibCore_build_tag(CryTagFunction tag_function, CryTagLookupCallback tag_lookup_callback){
    CryTag   tag;
    CryData *data_tag;
    CryExpr  expr;

    tag        = CryTag_init();
    tag->flags = TAG_CLEAN;
    data_tag   = CryData_tag(tag);
    expr       = CryExpr_init(EXPR_ENV, data_tag);

    expr->tag_function        = tag_function;
    expr->tag_lookup_callback = tag_lookup_callback;

    return CryData_expression(expr);
}

CryData *CryLibCore_if(){
    return CryLibCore_build_tag(CryLibCore_if_tag_function, CryTag_default_tag_lookup_callback);
}

CryData *CryLibCore_then(){
    return CryLibCore_build_tag(CryLibCore_then_tag_function, CryTag_default_tag_lookup_callback);
}

CryData *CryLibCore_else(){
    return CryLibCore_build_tag(CryLibCore_else_tag_function, CryTag_default_tag_lookup_callback);
}

CryData *CryLibCore_attributes(){
    return CryLibCore_build_tag(CryLibCore_attributes_tag_function, CryTag_default_tag_lookup_callback);
}

CryData *CryLibCore_taglib(){
    return CryLibCore_build_tag(CryLibCore_taglib_tag_function, CryLibCore_taglib_tag_lookup_callback);
}

CryData *CryLibCore_define(){
    return CryLibCore_build_tag(CryLibCore_define_tag_function, CryLibCore_define_tag_lookup_callback);
}

CryData *CryLibCore_funclib(){
    return CryLibCore_build_tag(CryLibCore_funclib_tag_function, CryTag_default_tag_lookup_callback);
}

CryData *CryLibCore_each(){
    return CryLibCore_build_tag(CryLibCore_each_tag_function, CryTag_default_tag_lookup_callback);
}

int CryLibCore_include_corelib(CryEnv *env, char *namespace, int global_include){
    CryEnv_include_tag(env, namespace, "if",         CryLibCore_if(),         global_include);
    CryEnv_include_tag(env, namespace, "then",       CryLibCore_then(),       global_include);
    CryEnv_include_tag(env, namespace, "else",       CryLibCore_else(),       global_include);
    CryEnv_include_tag(env, namespace, "attributes", CryLibCore_attributes(), global_include);
    CryEnv_include_tag(env, namespace, "taglib",     CryLibCore_taglib(),     global_include);
    CryEnv_include_tag(env, namespace, "define",     CryLibCore_define(),     global_include);
    CryEnv_include_tag(env, namespace, "funclib",    CryLibCore_funclib(),    global_include);
    CryEnv_include_tag(env, namespace, "each",       CryLibCore_each(),       global_include);

    return TRUE;
}
