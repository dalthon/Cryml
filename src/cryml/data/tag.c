#include "../cryml.h"

CryTag CryTag_init(){
    CryTag tag;

    tag              = malloc(sizeof(CryTagNode));
    tag->flags       = 0;
    tag->namespace   = NULL;
    tag->name        = NULL;
    tag->attributes  = NULL;
    tag->first_child = NULL;
    tag->next        = NULL;
    tag->text        = NULL;
    tag->eval        = NULL;

    return tag;
}

void CryTag_set_flag(CryTag tag, int flag){
    tag->flags |= flag;
}

void CryTag_remove_flag(CryTag tag, int flag){
    tag->flags &= ~flag;
}

void CryTag_set_namespace(CryTag tag, char *namespace){
    if(tag->namespace) free(tag->namespace);
    tag->namespace = strdup(namespace);
}

void CryTag_set_name(CryTag tag, char *name){
    if(tag->name) free(tag->name);
    tag->name = strdup(name);
}

char *CryTag_full_name(CryTag tag){
    int   full_name_lenght;
    char *full_name;

    if(tag->namespace == NULL){
        full_name = strdup(tag->name);
    }else{
        full_name_lenght = 2+strlen(tag->name)+strlen(tag->namespace);
        full_name        = malloc(full_name_lenght*sizeof(char));
        sprintf(full_name, "%s:%s", tag->namespace, tag->name);
        full_name[full_name_lenght] = '\0';
    }

    return full_name;
}

int CryTag_eval_tag_function(CryTag parent_tag, CryTag tag, CryContext *context, FILE * output){
    return tag->eval->tag_function(parent_tag, tag, context, output);
}

// TODO: Function CryTag_merge_default_attributes_and_predicates has compexity O(n^2).
//       This could be better, but I don't believe that someone will use a very large amount of attributes.
//       So this is done now in the simplest way, but when people start to use it, we must think about it.
void CryTag_merge_default_attributes_and_predicates(CryTag tag, CryTag definee_tag){
    int      found, definee_predicate;
    char    *definee_attribute_name, *attribute_name;
    CryAttr  definee_attribute;
    CryAttr  last_attribute;
    CryAttr  attribute;

    last_attribute = NULL;
    for(definee_attribute = definee_tag->attributes; definee_attribute != NULL; definee_attribute = definee_attribute->next){
        found                  = FALSE;
        definee_predicate      = definee_attribute->flags & ATTR_PREDICATE;
        definee_attribute_name = CryAttr_full_name(definee_attribute);
        for(attribute = tag->attributes; attribute != NULL; attribute = attribute->next){
            attribute_name = CryAttr_full_name(attribute);
            last_attribute = attribute;
            if( (definee_predicate == (attribute->flags & ATTR_PREDICATE)) && strcmp(definee_attribute_name, attribute_name) == 0 ){
                found = TRUE;
                free(attribute_name);
                break;
            }
            free(attribute_name);
        }
        free(definee_attribute_name);

        if(found == FALSE){
            attribute             = CryAttr_init(definee_attribute->flags);
            attribute->expression = CryExpr_clone(definee_attribute->expression);

            CryAttr_set_name(attribute, definee_attribute->name);
            if(definee_attribute->namespace != NULL) CryAttr_set_namespace(attribute, definee_attribute->namespace);

            if(tag->attributes == NULL){
                tag->attributes = attribute;
            }else{
                last_attribute->next = attribute;
            }
        }
    }
}

void CryTag_default_tag_lookup_callback(CryTag tag, CryEnv *env, CryExpr expression){
    tag->flags |= env->flags;
    tag->eval   = CryExpr_clone(expression);
    if(expression->data != NULL){
        CryTag_merge_default_attributes_and_predicates(tag, expression->data->value.tag);
    }
}

void CryTag_free(CryTag tag){
    if(tag->namespace            != NULL) free(tag->namespace);
    if(tag->name                 != NULL) free(tag->name);
    if(tag->attributes           != NULL) CryAttr_free(tag->attributes);
    if(tag->first_child          != NULL) CryTag_free(tag->first_child);
    if(tag->next                 != NULL) CryTag_free(tag->next);
    if(tag->text                 != NULL) CryText_free(tag->text);
    if(tag->eval                 != NULL) CryExpr_free(tag->eval);
    free(tag);
}