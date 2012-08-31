#include "cryml.h"

CryEnv *CryEnv_init(){
    CryEnv *env;

    env = malloc(sizeof(CryEnv));

    env->tags       = NULL;
    env->functions  = NULL;
    env->status     = ENV_CLEAN_STATUS;
    env->scopes     = NULL;
    env->parent     = NULL;
    env->root_path  = NULL;

    env->lex_states                 = NULL;
    env->root_tag                   = NULL;
    env->current_tag                = NULL;
    env->current_text               = NULL;
    env->current_attribute          = NULL;
    env->closing_unparsed_full_name = NULL;

    env->flags                                  = 0;
    env->rendering_data                         = CryHash_init(ENV_DATA_BUCKET_SIZE);
    env->extra                                  = CryHash_init(ENV_EXTRA_BUCKET_SIZE);
    env->rendering_context                      = CryContext_init(env, env->rendering_data, env->extra);

    env->default_eval                           = CryExpr_init(EXPR_ENV, NULL);
    env->default_eval->tag_lookup               = &CryEnv_default_tag_lookup;
    env->default_eval->function_lookup          = &CryEnv_default_function_lookup;
    env->default_eval->tag_function             = &CryRender_tag;
    env->default_eval->tag_lookup_callback      = &CryTag_default_tag_lookup_callback;
    env->default_eval->function_lookup_callback = NULL;

    return env;
}

void CrymlSetup(){
    if(CryFunctionLibraries == NULL) CryFunctionLibraries = CryHash_init(ENV_FUNC_BUCKET_SIZE);
}

void CrymlCleanup(){
    if(CryFunctionLibraries != NULL) CryHash_free(CryFunctionLibraries);
}

CryEnv *CryEnv_fork(CryEnv *parent_env){
    CryEnv *env;

    env         = CryEnv_init();
    env->parent = parent_env;

    return env;
}

int CryEnv_default_tag_lookup(CryEnv *env, CryTag tag){
    int      length;
    char    *full_name;
    CryData *proto_tag;

    if(tag->flags & TAG_ATTR_TAG){
        tag->eval               = CryExpr_init(EXPR_ENV, NULL);
        tag->eval->tag_function = &CryRender_attr_tag;
        return TRUE;
    }

    if(tag->flags & TAG_DATA_TAG){
        tag->eval               = CryExpr_init(EXPR_ENV, NULL);
        tag->eval->tag_function = &CryRender_data_tag;
        return TRUE;
    }

    if(env->tags != NULL){
        full_name = CryTag_full_name(tag);
        proto_tag = CryHash_top(env->tags, full_name);
        free(full_name);

        if(proto_tag != NULL){
            (proto_tag->value.expression->tag_lookup_callback)(tag, env, proto_tag->value.expression);
            return TRUE;
        }

        if(tag->namespace != NULL){
            length    = strlen(tag->namespace) + 3;
            full_name = malloc(length*sizeof(char));
            sprintf(full_name, "%s:*", tag->namespace);
            proto_tag = CryHash_top(env->tags, full_name);
            free(full_name);
            if(proto_tag != NULL){
                (proto_tag->value.expression->tag_lookup_callback)(tag, env, proto_tag->value.expression);
                return TRUE;
            }
        }
    }

    if(env->parent != NULL){
        return (env->parent->default_eval->tag_lookup)(env->parent, tag);
    }

    return FALSE;
}

int CryEnv_default_function_lookup(CryEnv *env, CryExpr expr){
    int      succeed = FALSE;
    char    *function_name;
    CryData *proto_data;
    CryExpr  proto_expr;

    if(env->functions != NULL){
        function_name = CryArray_get(expr->data->value.array, 0)->value.string;
        proto_data    = CryHash_top(env->functions, function_name);

        if(proto_data != NULL){
            proto_expr = proto_data->value.expression;
            succeed = (proto_expr->function_lookup_callback)(expr, env, proto_expr);
            if(succeed) return TRUE;
        }
    }

    if(env->parent != NULL){
        return (env->parent->default_eval->function_lookup)(env->parent, expr);
    }

    return FALSE;
}

void CryEnv_tag_lookup(CryEnv *env, CryTag tag){
    if( (env->default_eval->tag_lookup)(env, tag) ) return;
    (env->default_eval->tag_lookup_callback)(tag, env, env->default_eval);
}

int CryEnv_function_lookup(CryEnv *env, CryExpr expr){
    int succeed;

    if( (env->default_eval->function_lookup)(env, expr) ) return TRUE;

    if(env->default_eval->function_lookup_callback){
        succeed = (env->default_eval->function_lookup_callback)(expr, env, env->default_eval);
        if(succeed) return TRUE;
    }

    // printf("TODO?! Function Lookup Failed!\n");
    // TODO: Handle function not found error
    return FALSE;
}

// TODO: Do it properly!
CryScope *CryEnv_scope_lookup(CryEnv *env, char *scope_name){
    CryScope *scope;

    for(scope = env->scopes; scope != NULL; scope = scope->next){
        if(strcmp(scope->name, scope_name) == 0) break;
    }

    return scope;
}

void CryEnv_set_flag(CryEnv *env, int flag){
    CryTag_init(env->current_tag, flag);
}

void CryEnv_cleanup(CryEnv *env){
    if(env->root_tag != NULL) CryTag_free(env->root_tag);
    env->lex_states                 = NULL;
    env->root_tag                   = NULL;
    env->current_tag                = NULL;
    env->current_text               = NULL;
    env->current_attribute          = NULL;
    env->closing_unparsed_full_name = NULL;

    if(env->tags != NULL) CryHash_free(env->tags);
    env->tags = NULL;

    if(env->functions != NULL) CryHash_free(env->functions);
    env->functions = NULL;

    env->status = ENV_CLEAN_STATUS;
}

void CryEnv_lock(CryEnv *env){
    pthread_mutex_lock(&(env->render_lock));
}

void CryEnv_unlock(CryEnv *env){
    pthread_mutex_unlock(&(env->render_lock));
}

void CryEnv_free(CryEnv *env){
    if(env->tags              != NULL) CryHash_free(env->tags);
    if(env->functions         != NULL) CryHash_free(env->functions);
    if(env->rendering_data    != NULL) CryHash_free(env->rendering_data);
    if(env->extra             != NULL) CryHash_free(env->extra);
    if(env->rendering_context != NULL) CryContext_free(env->rendering_context);
    if(env->root_tag          != NULL) CryTag_free(env->root_tag);
    if(env->default_eval      != NULL) CryExpr_free(env->default_eval);
    CryLexState_free(env->lex_states);
    free(env);
}

void CryEnv_include_tag(CryEnv *env, char *namespace, char *name, CryData *proto_tag, int global_include){
    int   size;
    char *full_name;

    if(namespace != NULL){
        size      = 2 + strlen(namespace) + strlen(name);
        full_name = malloc(size*sizeof(char));
        sprintf(full_name, "%s:%s", namespace, name);
        CryHash_push(env->tags, full_name, proto_tag);
        free(full_name);
    }

    if(global_include) CryHash_push(env->tags, name, proto_tag);
}

void CryLexState_free(CryLexState state){
    if(state != NULL){
        CryLexState_free(state->next);
        free(state);
    }
}







// TODO: REMOVE IT ASAP!
CryData *debug_function(CryContext *context, CryData *function_name, CryData *args){
    char    *string;
    CryData *result;

    return CryData_integer(123);

    string = malloc(256*sizeof(char));
    sprintf(string, "function: %s %d arguments", function_name->value.string, CryArray_size(args->value.array));
    result = CryData_string(string);
    free(string);

    return result;
}