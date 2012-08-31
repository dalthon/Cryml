typedef union CrySType CrySType;

union CrySType {
    int             boolean;
    long int        integer;
    long double     cdouble;
    char           *string;
    CryTag          tag;
    CryAttr         attribute;
    CryExpr         expression;
    CryText         text;
    CryData        *data;
    CryStack        stack;
    CryTextElement *text_element;
};

struct CryParseContext {
    CryEnv   *env;
    yyscan_t  scanner;
};

#define YYPARSE_PARAM  context
#define YYLEX_PARAM    ((CryParseContext*)context)->scanner

#define CRY_CONTEXT    ((CryParseContext*)context)
#define CRY_ENV        ((CryParseContext*)context)->env

#define YYSTYPE CrySType

CryTag  Parser_init_root_tag(CryEnv *env);
void    Parser_finish(CryEnv *env);
CryTag  Parser_open_content_list(CryEnv *env);
CryTag  Parser_close_content_list(CryEnv *env, CryTag sentinel_tag);
CryTag  Parser_open_tag(int flag, char *namespace, char *name);
void    Parser_enqueue_tag(CryEnv *env, CryTag tag);
CryTag  Parser_close_opening_tag(CryEnv *env, CryTag tag, CryAttr attributes, int flag);
CryTag  Parser_close_tag(CryEnv *env, CryTag tag, CryTag first_child, CryTag push_tag);
CryTag  Parser_close_singleton_tag(CryEnv *env, CryTag tag, CryAttr attributes);
CryAttr Parser_open_attr_list(CryEnv *env);
CryAttr Parser_close_attr_list(CryEnv *env, CryAttr sentinel_attr);
void    Parser_enqueue_attr(CryEnv *env, int flags, char *namespace, char *name, CryExpr expression);
CryExpr Parser_integer_value(long int integer);
CryExpr Parser_double_value(long double cdouble);
CryExpr Parser_string_value(char *string);
CryExpr Parser_data_value(char *data_name);
CryExpr Parser_attribute_value(char *attribute_name);
CryExpr Parser_function_expression(CryEnv *env, char *function_name, CryData *array_data);
void    Parser_append_arg(CryData *array_data, CryExpr expression);
void    Parser_append_hash_key_and_value(CryData *array_data, char *key, CryExpr expression);
CryExpr Parser_scoped_value(CryEnv *env, char *scope_name);
void    Parser_init_scope_data(CryEnv *env);
void    Parser_push_scope_data(CryEnv *env, char *string);
void    Parser_push_escaped_scope_data(CryEnv *env, char *string);
CryExpr Parser_paired_expression(int opcode, CryExpr first_expr, CryExpr second_expr);
CryExpr Parser_unary_add_sub(int positive_number, CryExpr expression);
CryExpr Parser_boolean_value(int boolean_value);
CryExpr Parser_null_value();
void    Parser_enqueue_text(CryEnv *env, CryTextElement *text_element);
int     Parser_push_state(CryEnv *env, int state);
void    Parser_pop_state(CryEnv *env, int last_state);
int     Parser_parse(CryEnv *env, char *filename, int state);