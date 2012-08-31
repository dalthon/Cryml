// Ray T using code from github.com/spkelly/C_Ray
// Requires imagemagick to convert ppm files to png

#include "../../src/cryml/cryml.h"
#include "ray_tracing.h"

int CryRender_eval_only_tags_childs_function(CryTag tag, CryContext *context, FILE * output){
    CryTag child_tag;

    for(child_tag = tag->first_child; child_tag != NULL; child_tag = child_tag->next){
        if( !(child_tag->flags & TAG_TEXT) ) CryRender_render_tag(tag, child_tag, context, output);
    }

    return 0;
}

int CryLibCore_raytracing_tag_function(CryTag parent_tag, CryTag tag, CryContext *context, FILE *output){
    char *path;
    char  system_call[256];
    FILE *file;

    path = CryContext_current_attr(context, "src")->value.string;

    file = fopen("temp.txt", "w");
    CryRender_eval_only_tags_childs_function(tag, context, file);
    fclose(file);

    sprintf(system_call, "c_ray temp.txt %s.ppm", path);
    system(system_call);
    system("rm temp.txt");
    sprintf(system_call, "convert %s.ppm %s.png", path, path);
    system(system_call);

    fprintf(output, "<img src=\"%s.png\"/>", path);
    return 0;
}

int CryLibCore_raytracing_param_tag_function(CryTag parent_tag, CryTag tag, CryContext *context, FILE *output){
    char     *key, *value;
    CryStack stack, s;

    stack = CryStack_init();
    CryHash_traverse(CryContext_stack(context)->attributes, stack);

    value = CryContext_current_attr(context, "name")->value.string;
    fprintf(output, "%s %s", tag->name, value);

    for(s = stack->next; s != NULL; s = s->next){
        key = s->data->value.string;
        if(strcmp(key, "name") != 0){
            value = CryContext_current_attr(context, key)->value.string;
            fprintf(output, "\n  %s %s", key, value);
        }
    }
    fprintf(output, " ;\n\n");
    CryStack_free(stack);

    return TRUE;
}

CryData *CryLibCore_raytracing(){
    return CryLibCore_build_tag(CryLibCore_raytracing_tag_function, CryTag_default_tag_lookup_callback);
}

CryData *CryLibCore_raytracing_param(){
    return CryLibCore_build_tag(CryLibCore_raytracing_param_tag_function, CryTag_default_tag_lookup_callback);
}

int Cryml_taglib_init(CryContext *context){
    int      global;
    char    *namespace;
    CryData *data;

    data = CryContext_current_attr(context, "namespace");
    if(data == NULL){
        namespace = NULL;
    }else{
        namespace = data->value.string;
    }

    data = CryContext_current_attr(context, "global");
    if(data == NULL){
        global = TRUE;
    }else{
        global = CryData_is_true(data);
    }

    CryEnv_include_tag(context->environment, namespace, "raytracing", CryLibCore_raytracing(),       TRUE);
    CryEnv_include_tag(context->environment, namespace, "rectangle",  CryLibCore_raytracing_param(), TRUE);
    CryEnv_include_tag(context->environment, namespace, "window",     CryLibCore_raytracing_param(), TRUE);
    CryEnv_include_tag(context->environment, namespace, "tplane",     CryLibCore_raytracing_param(), TRUE);
    CryEnv_include_tag(context->environment, namespace, "pplane",     CryLibCore_raytracing_param(), TRUE);
    CryEnv_include_tag(context->environment, namespace, "polygon",    CryLibCore_raytracing_param(), TRUE);
    CryEnv_include_tag(context->environment, namespace, "sphere",     CryLibCore_raytracing_param(), TRUE);
    CryEnv_include_tag(context->environment, namespace, "light",      CryLibCore_raytracing_param(), TRUE);
    
    return TRUE;
}