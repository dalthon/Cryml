#include "cryml.h"

CryTagStack *CryTagStack_init(CryContext *context, CryTag tag){
    CryTagStack *tag_stack;

    tag_stack                  = malloc(sizeof(CryTagStack));
    tag_stack->tag             = tag;
    tag_stack->flags           = tag->flags;
    tag_stack->attributes      = CryHash_init(TAG_EVALUATED_ATTR_BUCKET_SIZE);
    tag_stack->attribute_names = CryStack_init();
    tag_stack->attr_list       = NULL;
    tag_stack->last_attr       = NULL;
    tag_stack->parent          = context->tag_stack;

    context->tag_stack = tag_stack;

    return tag_stack;
}

void CryTagStack_pop(CryTagStack *tag_stack, CryContext *context){
    context->tag_stack = tag_stack->parent;
    CryTagStack_free(tag_stack);
}

void CryTagStack_add_visible_attr(CryTagStack *tag_stack, char *attribute_key, CryData *data){
    CryTagAttrList *attr;

    attr = CryTagAttrList_init(attribute_key, data);
    if(tag_stack->attr_list == NULL){
        tag_stack->attr_list = attr;
        tag_stack->last_attr = attr;
    }else{
        tag_stack->last_attr->next = attr;
        tag_stack->last_attr       = attr;
    }
}

void CryTagStack_free(CryTagStack *tag_stack){
    if(tag_stack->attr_list != NULL) CryTagAttrList_free(tag_stack->attr_list);
    CryHash_free(tag_stack->attributes);
    CryStack_free(tag_stack->attribute_names);
    free(tag_stack);
}