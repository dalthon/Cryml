#include "cryml.h"

CryContext *CryContext_init(CryEnv *env, CryHash data, CryHash extra){
    CryContext *context;

    context                = malloc(sizeof(CryContext));
    context->flags         = 0;
    context->environment   = env;
    context->data          = data;
    context->attributes    = CryHash_init(ENV_ATTR_BUCKET_SIZE);
    context->extra         = extra;
    context->tag_stack     = NULL;

    return context;
}

void CryContext_init_stack(CryContext *context, CryTag tag){
    CryTagStack_init(context, tag);
}

CryTagStack *CryContext_stack(CryContext *context){
    return context->tag_stack;
}

CryData *CryContext_current_attr(CryContext *context, char *attr_name){
    return CryHash_top(CryContext_stack(context)->attributes, attr_name);
}

int CryContext_error(CryContext *context){
    return context->flags;
}

void CryContext_raise(CryContext *context, int error_flag, char *buff, ...){
    va_list arglist;
    va_start(arglist, buff);
    context->flags = error_flag;
    vsprintf(context->error_message, buff, arglist);
    va_end(arglist);
}

void CryContext_push_attribute(CryContext *context, char *attribute_key, CryData *data){
    CryHash_push( context->attributes,            attribute_key, data);
    CryHash_push( context->tag_stack->attributes, attribute_key, data);
    CryStack_push(context->tag_stack->attribute_names, CryData_string(attribute_key));
}

void CryContext_pop_attribute(CryContext *context, char *attribute_key){
    CryHash_pop(context->attributes, attribute_key);
}

void CryContext_add_visible_attr(CryContext *context, char *attribute_key, CryData *data){
    CryTagStack_add_visible_attr(context->tag_stack, attribute_key, data);
}

void CryContext_pop_stack(CryContext *context){
    CryStack  attribute_names;
    CryData  *data;

    attribute_names = context->tag_stack->attribute_names;
    while(CryStack_next(attribute_names) != NULL){
        data = CryStack_pop(attribute_names);
        CryHash_pop(context->attributes,            data->value.string);
        CryHash_pop(context->tag_stack->attributes, data->value.string);
        CryData_free(data);
    }
    CryTagStack_pop(context->tag_stack, context);
}

CryData *CryContext_data_lookup(CryContext *context, char *attribute_key){
    CryData *data;

    data = CryHash_find(context->data, attribute_key);
    if(data == NULL) CryContext_raise(context, ERROR_MISSING_DATA_ATTRIBUTE, "Missing data attribute $%s", attribute_key);

    return data;
}

CryData *CryContext_attribute_lookup(CryContext *context, char *attribute_key){
    CryData *data;

    data = CryHash_top(context->attributes, attribute_key);
    if(data == NULL) CryContext_raise(context, ERROR_MISSING_ATTRIBUTE, "Missing attribute @%s", attribute_key);

    return data;
}

CryData *CryContext_tag_attribute_lookup(CryContext *context, char *attribute_key){
    return CryHash_top(context->tag_stack->attributes, attribute_key);
}

void CryContext_free(CryContext *context){
    while(context->tag_stack != NULL) CryTagStack_pop(context->tag_stack, context);
    CryHash_free(context->attributes);
    free(context);
}

void CryContext_cleanup(CryContext *context){
    context->flags = 0;
}
