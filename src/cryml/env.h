#pragma once

// Constants
#define ENV_CLEAN_STATUS                 0x0000
#define ENV_PARSING_STATUS               0x0100
#define ENV_PARSING_CONTEXT_STATUS       0x0101
#define ENV_PARSED_STATUS                0x0200
#define ENV_RENDERING_STATUS             0x0400
#define ENV_ERROR_STATUS                 0x0800

#define ENV_PARSING_MASK                 0x0100
#define ENV_PARSED_MASK                  0x0200
#define ENV_RENDERING_MASK               0x0400
#define ENV_ERROR_MASK                   0x0800

#define ENV_TAGS_BUCKET_SIZE  13
#define ENV_FUNC_BUCKET_SIZE  13
#define ENV_ATTR_BUCKET_SIZE  7
#define ENV_DATA_BUCKET_SIZE  7
#define ENV_EXTRA_BUCKET_SIZE 5

// Structs
struct CryLexStateStack {
    int         state;
    CryLexState next;
};

struct CryEnv {
    // Environment
    int              status;
    CryHash          tags;
    CryHash          functions;
    CryScope        *scopes;
    CryEnv          *parent;
    char            *root_path;
    pthread_mutex_t  render_lock;

    // Parsing
    char        *closing_unparsed_full_name;
    CryLexState  lex_states;
    CryTag       root_tag;
    CryTag       current_tag;
    CryText      current_text;
    CryAttr      current_attribute;

    // Rendering
    int         flags;
    CryHash     rendering_data;
    CryHash     extra;
    CryContext *rendering_context;
    CryExpr     default_eval;
};

// Prepare functions
void CrymlSetup();
void CrymlCleanup();

// Function Prototypes
CryEnv   *CryEnv_init();
CryEnv   *CryEnv_fork(CryEnv *parent_env);
int       CryEnv_default_tag_lookup(CryEnv *env, CryTag tag);
int       CryEnv_default_function_lookup(CryEnv *env, CryExpr expr);
void      CryEnv_tag_lookup(CryEnv *env, CryTag tag);
int       CryEnv_function_lookup(CryEnv *env, CryExpr expr);
CryScope *CryEnv_scope_lookup(CryEnv *env, char *scope_name);
void      CryEnv_set_flag(CryEnv *env, int flag);
void      CryEnv_cleanup(CryEnv *env);
void      CryEnv_lock(CryEnv *env);
void      CryEnv_unlock(CryEnv *env);
void      CryEnv_free(CryEnv *env);
void      CryEnv_include_tag(CryEnv *env, char *namespace, char *name, CryData *proto_tag, int global_include);
void      CryLexState_free(CryLexState state);



CryData *debug_function(CryContext *context, CryData *function_name, CryData *args); // TODO: REMOVE IT ASAP!
