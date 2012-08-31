int      CryLibCore_if_tag_function(CryTag parent_tag, CryTag tag, CryContext *context, FILE *output);
int      CryLibCore_if_compare(CryContext *context, CryData *is_data, char *attr_name, int opcode, int *check_count);
int      CryLibCore_then_tag_function(CryTag parent_tag, CryTag tag, CryContext *context, FILE *output);
int      CryLibCore_else_tag_function(CryTag parent_tag, CryTag tag, CryContext *context, FILE *output);
int      CryLibCore_attributes_tag_function(CryTag parent_tag, CryTag tag, CryContext *context, FILE *output);
int      CryLibCore_funclib_tag_function(CryTag parent_tag, CryTag tag, CryContext *context, FILE *output);
CryData *CryLibCore_build_tag(CryTagFunction tag_function, CryTagLookupCallback tag_lookup_callback);
CryData *CryLibCore_if();
CryData *CryLibCore_then();
CryData *CryLibCore_else();
CryData *CryLibCore_attributes();
CryData *CryLibCore_define();
CryData *CryLibCore_funclib();
int      CryLibCore_include_corelib(CryEnv *env, char *namespace, int global_include);