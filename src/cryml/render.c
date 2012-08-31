#include "cryml.h"

#define RAISE_IF_ERROR(context) if(CryContext_error(context)) return CryContext_error(context)

int CryRender_render(CryContext *context, FILE *output){
    return CryRender_render_tag(NULL, context->environment->root_tag, context, output);
}

int CryRender_render_tag(CryTag parent_tag, CryTag tag, CryContext *context, FILE *output){
    int result;
    if(tag->flags & TAG_TEXT){
        return CryRender_text_tag(parent_tag, tag, context, output);
    }else{
        CryContext_init_stack(context, tag);
        CryRender_eval_tag_attributes(tag, context);
        RAISE_IF_ERROR(context);

        result = CryTag_eval_tag_function(parent_tag, tag, context, output);
        CryContext_pop_stack(context);
        return result;
    }
}

int CryRender_render_visible_attributes(CryContext *context, FILE *output){
    CryTagAttrList *attr;

    for(attr = CryContext_stack(context)->attr_list; attr != NULL; attr = attr->next){
        if(CryData_printable(attr->data)){
            fprintf(output, " %s=\"", attr->full_name);
            CryData_escaped_print(attr->data, output);
            fprintf(output, "\"");
        }
    }

    return 0;
}

// TODO: Handle predicates
int CryRender_eval_tag_attributes(CryTag tag, CryContext *context){
    char    *full_name;
    CryData *data;
    CryAttr  attribute;

    for(attribute = tag->attributes; attribute != NULL; attribute = attribute->next){
        data      = CryExpr_eval(context, attribute->expression);
        full_name = CryAttr_full_name(attribute);
        if(attribute->flags & ATTR_PREDICATE){
            // TODO: Not done yet
        }else{
            CryContext_push_attribute(context, full_name, data);
            if( !(attribute->flags & ATTR_INVISIBLE) ) CryContext_add_visible_attr(context, full_name, data);
        }
        free(full_name);
        RAISE_IF_ERROR(context);
    }

    return 0;
}

int CryRender_eval_tags_childs_function(CryTag tag, CryContext *context, FILE * output){
    CryTag child_tag;

    for(child_tag = tag->first_child; child_tag != NULL; child_tag = child_tag->next){
        CryRender_render_tag(tag, child_tag, context, output);
        RAISE_IF_ERROR(context);
    }

    return 0;
}


// Tag Functions

int CryRender_tag_group(CryTag parent_tag, CryTag tag, CryContext *context, FILE *output){
    CryTag child_tag;

    for(child_tag = tag->first_child; child_tag != NULL; child_tag = child_tag->next){
        CryRender_render_tag(parent_tag, child_tag, context, output);
        RAISE_IF_ERROR(context);
    }

    return 0;
}

int CryRender_tag(CryTag parent_tag, CryTag tag, CryContext *context, FILE *output){
    char *full_name;

    full_name = CryTag_full_name(tag);

    if(output){
        fprintf(output, "<%s", full_name);
        CryRender_render_visible_attributes(context, output);
    }

    if(!CryContext_error(context)){
        if(tag->flags & TAG_SINGLETON){
            if(output) fprintf(output, "/>");
        }else{
            if(output) fprintf(output, ">");
            CryRender_eval_tags_childs_function(tag, context, output);
            if(output) fprintf(output, "</%s>", full_name);
        }
     }

    free(full_name);
    RAISE_IF_ERROR(context);

    return 0;
}

int CryRender_text_tag(CryTag parent_tag, CryTag tag, CryContext *context, FILE *output){
    if(output != NULL) CryText_print(tag->text, output, tag->flags & TAG_UNCOMPRESSED_TEXT);
    return 0;
}

int CryRender_attr_tag(CryTag parent_tag, CryTag tag, CryContext *context, FILE *output){
    CryData *data;
    char    *full_name;
    int      error_code = 0;

    // TODO: tag attr must be evaluated differently!
    full_name = CryTag_full_name(tag);

    if(tag->flags & TAG_SINGLETON){
        if(output){
            data = CryContext_attribute_lookup(context, full_name);

            if(data){
                // TODO: Add method call by attributes
                if(data->flags == DATA_TAG){
                    error_code = CryRender_render_tag(parent_tag, data->value.tag, context, output);
                }else if(CryData_printable(data)){
                    error_code = CryData_print(data, output);
                }else{
                    // TODO: Handle these errors properly
                    printf("WARNING: Unprintable not tag attr");
                    error_code = 234;
                }
            }else{
                // TODO: Handle these errors properly
                printf("WARNING: Attr not found");
                error_code = 123;
            }
        }
    }else{
        // TODO: Not done yet
        // if(output) fprintf(output, ">");
        // error_code = CryRender_eval_tags_childs_function(tag, env, output);
        // if(output) fprintf(output, "</%s>", full_name);
    }
    free(full_name);

    return error_code;
}

// TODO: It doesn't do what it's supposed to...
int CryRender_data_tag(CryTag parent_tag, CryTag tag, CryContext *context, FILE *output){
    return CryRender_attr_tag(parent_tag, tag, context, output);
}
