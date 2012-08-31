#include "generated/lexical.c"
#include "cryml/env.h"

#define DEBUG_LEAKS 1

int main(int argc, char* argv[]){
    char           system_call[80];
    CryEnv         *env;
    CryTag          tag;
    CryHash         hash, extra;
    CryContext     *ctx;
    CryFunctionLib *library;

    data_allocated = 0;

    CryParseContext context;

    if(argc < 2) {
        printf("Usage: %s filename\n", argv[0]);
        return -1;
    }

    CrymlSetup();
    env = CryEnv_init();

    // For debug purposes only!
    // env->flags        = TAG_UNCOMPRESSED_TEXT;
    env->flags        = 0;
    env->scopes       = malloc(sizeof(CryScope));
    env->scopes->name = strdup("rb");

    env->tags = CryHash_init(3);
    CryLibCore_include_corelib(env, "cryml", TRUE);

    // CryHash_push(env->tags, "if",      CryLibCore_if());
    // CryHash_push(env->tags, "funclib", CryLibCore_funclib());

    // library = CryFunctionLib_init("bin/lib_example_functions.so");
    // if(library) printf("Library Loaded!\n");
    // CryFunctionLib_apply(library, env);
    // printf("%s\n", CryArray_get(CryStack_top(library->functions)->value.expression->data->value.array, 0)->value.string);
    // printf("%d\n", CryArray_get(CryStack_top(library->functions)->value.expression->data->value.array, 1)->value.integer);

    // printf("Syntactic Analisys\n");
    Parser_parse(env, argv[1], 0);

    hash  = CryHash_init(3);
    extra = CryHash_init(3);
    CryHash_add(hash, "home_url", CryData_string("http://www.home.url"));
    ctx = CryContext_init(env, hash, extra);

    CryRender_render(ctx, stdout);

    CryHash_free(hash);
    CryHash_free(extra);
    CryContext_free(ctx);

    free(env->scopes->name);
    free(env->scopes);
    CryEnv_free(env);
    CrymlCleanup();

    printf("\n\nAllocated data: %d\n", data_allocated);

    if(DEBUG_LEAKS){
        sprintf(system_call, "leaks %d > tmp/parsing_leaks.txt\n", getpid());
        printf("%s", system_call);
        system(system_call);
    }

    return 0;
}
