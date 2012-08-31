#pragma once

int      CryRender_eval_only_tags_childs_function(CryTag tag, CryContext *context, FILE * output);
int      CryLibCore_raytracing_tag_function(CryTag parent_tag, CryTag tag, CryContext *context, FILE *output);
int      CryLibCore_raytracing_param_tag_function(CryTag parent_tag, CryTag tag, CryContext *context, FILE *output);
CryData *CryLibCore_raytracing();
CryData *CryLibCore_raytracing_param();
int      Cryml_taglib_init(CryContext *context);
void     CryHash_traverse(CryHash hash, CryStack stack);
void     CryTree_traverse(CryTree tree, CryStack stack);