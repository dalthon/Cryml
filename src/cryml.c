#include "generated/lexical.c"

#define DEBUG_LEAKS 1

int main(int argc, char* argv[]){
    char           system_call[80];
    CryEnv         *env;
    CryTag          tag;
    CryHash         hash, extra;
    CryContext     *ctx;
    CryFunctionLib *library;
    FILE           *output;

    data_allocated = 0;

    CryParseContext context;

    if(argc < 3) {
        printf("Usage: %s input output\n", argv[0]);
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

    Parser_parse(env, argv[1], 0);

    hash  = CryHash_init(3);
    extra = CryHash_init(3);
    ctx = CryContext_init(env, hash, extra);

    output = fopen(argv[2], "w");
    CryRender_render(ctx, output);
    fclose(output);

    CryHash_free(hash);
    CryHash_free(extra);
    CryContext_free(ctx);

    free(env->scopes->name);
    free(env->scopes);
    CryEnv_free(env);
    CrymlCleanup();

    return 0;
}
