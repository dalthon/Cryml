// Structs
struct CryFunctionLib {
    int       error;
    int       data_allocated;
    char     *path;
    void     *handler;
    CryStack  functions;
};

CryHash CryFunctionLibraries;
pthread_mutex_t  CryGlobalFuncLock;

typedef int (CryFunctionInitCallback)(CryFunctionLib *);
typedef int (CryTagInitCallback)(CryContext *);

CryFunctionLib *CryFunctionLib_init(char *path);
void            CryFunctionLib_prepare(CryFunctionLib *library);
void            CryFunctionLib_release(CryFunctionLib *library);
void            CryFunctionLib_finish(CryFunctionLib *library);
CryFunctionLib *CryFunctionLib_find(char *path);
int             CryFunctionLib_init_library(CryFunctionLib *library);
int             CryFunctionLib_add_function(CryFunctionLib *library, char *function_name, char *calling_name, int arity, CryFunctionLookupCallback function_lookup_callback);
void            CryFunctionLib_free(CryFunctionLib *library);
void            CryFunctionLib_apply(CryFunctionLib *library, CryEnv *env);
void            CryFunctionLib_lock();
void            CryFunctionLib_unlock();
int             CryFunction_default_function_lookup_callback(CryExpr expr, CryEnv *env, CryExpr proto_expr);