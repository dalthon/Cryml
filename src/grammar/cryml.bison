%{
#include "../cryml/cryml.h"

// TODO: Handle Syntax Error
%}

%define api.pure

%token UNRECOGNIZED_TOKEN

%token OPEN_CLOSING_TAG
%token OPEN_TAG
%token CLOSE_SINGLETON_TAG
%token CLOSE_UNPARSED_TAG
%token CLOSE_COMPRESSED_TAG
%token CLOSE_TAG
%token UNPARSED_END

%token HIDE_MARK
%token ATTR_MARK
%token DATA_MARK
%token <string> NAMESPACE
%token <string> NAME
%token CLOSE_NAME
%token BLANK

%token ASSIGN
%token <cdouble> DOUBLE
%token <integer> INTEGER
%token <string>  SINGLE_QUOTED_STRING
%token <string>  DOUBLE_QUOTED_STRING
%token <string>  ATTRIBUTE
%token <string>  ATTR_DATA
%token <string>  OPEN_SCOPE
%token <string>  OPEN_FUNCTION_CALL
%token <string>  SCOPE_DATA
%token <string>  METHOD_CALL
%token CLOSE_SCOPE
%token COLON
%token COMMA_SEPARATOR
%token TRUECT
%token FALSECT
%token NULLCT

%token OR_OP
%token AND_OP
%token NOT_OP

%token EQ_OP
%token NE_OP
%token LT_OP
%token LE_OP
%token GT_OP
%token GE_OP

%token MUL_OP
%token DIV_OP
%token MOD_OP
%token ADD_OP
%token SUB_OP

%token OPEN_CURLY_BRACK
%token CLOSE_CURLY_BRACK

%token OPEN_SQUARE_BRACK
%token CLOSE_SQUARE_BRACK

%token OPEN_PARENTHESIS
%token CLOSE_PARENTHESIS

%token <text_element> TEXT
%token UNPARSED_TEXT
%token UNPARSED_LESS_THAN

%type <tag>        Document
%type <tag>        ContentList

%type <attribute>  TagAttrList

%type <tag>        Tag
%type <expression> TagAttrValue
%type <tag>        OpenTag
%type <integer>    CloseTag
%type <expression> ScopeCall

%type <expression> ArrayExpr
%type <expression> HashExpr
%type <expression> Expression
%type <expression> ObjectExpr
%type <expression> SimpleExpr
%type <expression> FunctionCall
%type <expression> MethodCall
%type <expression> IndexCall

%type <data>       ArgsList
%type <data>       HashList

%type <expression> FullExpr
%type <expression> CmpExpr
%type <expression> NotExpr
%type <expression> ArithExpr
%type <expression> MultiExpr
%type <expression> UnaryExpr
%type <boolean>    UnaryOps

%%
Document         : { $<tag>$ = Parser_init_root_tag(CRY_ENV); } ContentList { ($<tag>1)->first_child = $2; $$ = $<tag>1; Parser_finish(CRY_ENV); }
                 ;
ContentList      : { $$ = NULL; }
                 | { $<tag>$ = Parser_open_content_list(CRY_ENV); } Content { $$ = Parser_close_content_list(CRY_ENV, $<tag>1); }
                 ;
Content          : Tag          { Parser_enqueue_tag(CRY_ENV, $1);  }
                 | Content Tag  { Parser_enqueue_tag(CRY_ENV, $2);  }
                 | TEXT         { Parser_enqueue_text(CRY_ENV, $1); }
                 | Content TEXT { Parser_enqueue_text(CRY_ENV, $2); }
                 ;
Tag              : OpenTag TagAttrList CloseTag { $<tag>$ = Parser_close_opening_tag(CRY_ENV, $1, $2, $3); } ContentList ClosingTag { $$ = Parser_close_tag(CRY_ENV, $1, $5, $<tag>4); }
                 | OpenTag TagAttrList CLOSE_SINGLETON_TAG { $$ = Parser_close_singleton_tag(CRY_ENV, $1, $2); }
                 ;
OpenTag          : OPEN_TAG HIDE_MARK NAMESPACE NAME { $$ = Parser_open_tag(TAG_PRE_RENDER,   $3, $4); }
                 | OPEN_TAG HIDE_MARK           NAME { $$ = Parser_open_tag(TAG_PRE_RENDER, NULL, $3); }
                 | OPEN_TAG DATA_MARK NAMESPACE NAME { $$ = Parser_open_tag( TAG_DATA_TAG,    $3, $4); }
                 | OPEN_TAG DATA_MARK           NAME { $$ = Parser_open_tag( TAG_DATA_TAG,  NULL, $3); }
                 | OPEN_TAG ATTR_MARK NAMESPACE NAME { $$ = Parser_open_tag( TAG_ATTR_TAG,    $3, $4); }
                 | OPEN_TAG ATTR_MARK           NAME { $$ = Parser_open_tag( TAG_ATTR_TAG,  NULL, $3); }
                 | OPEN_TAG           NAMESPACE NAME { $$ = Parser_open_tag(            0,    $2, $3); }
                 | OPEN_TAG                     NAME { $$ = Parser_open_tag(            0,  NULL, $2); }
                 ;
CloseTag         : CLOSE_UNPARSED_TAG   { $$ = TAG_UNPARSED_TEXT;     }
                 | CLOSE_COMPRESSED_TAG { $$ = TAG_UNCOMPRESSED_TEXT; }
                 | CLOSE_TAG            { $$ = TAG_CLEAN;             }
                 ;
ClosingTag       : OPEN_CLOSING_TAG CLOSE_NAME CLOSE_TAG
                 | UNPARSED_END
                 ;
TagAttrList      : { $$ = NULL; }
                 | { $<attribute>$ = Parser_open_attr_list(CRY_ENV); } TagAttributes { $$ = Parser_close_attr_list(CRY_ENV, $<attribute>1); }
                 ;
TagAttributes    : TagAttribute
                 | TagAttributes TagAttribute
                 ;
TagAttribute     : COLON     NAMESPACE NAME ASSIGN TagAttrValue { Parser_enqueue_attr(CRY_ENV, ATTR_PREDICATE,   $2, $3, $5);   }
                 | COLON               NAME ASSIGN TagAttrValue { Parser_enqueue_attr(CRY_ENV, ATTR_PREDICATE, NULL, $2, $4);   }
                 | HIDE_MARK NAMESPACE NAME ASSIGN TagAttrValue { Parser_enqueue_attr(CRY_ENV, ATTR_INVISIBLE,   $2, $3, $5);   }
                 | HIDE_MARK           NAME ASSIGN TagAttrValue { Parser_enqueue_attr(CRY_ENV, ATTR_INVISIBLE, NULL, $2, $4);   }
                 |           NAMESPACE NAME ASSIGN TagAttrValue { Parser_enqueue_attr(CRY_ENV,              0,   $1, $2, $4);   }
                 |                     NAME ASSIGN TagAttrValue { Parser_enqueue_attr(CRY_ENV,              0, NULL, $1, $3);   }
                 | COLON     NAMESPACE NAME                     { Parser_enqueue_attr(CRY_ENV, ATTR_PREDICATE,   $2, $3, NULL); }
                 | COLON               NAME                     { Parser_enqueue_attr(CRY_ENV, ATTR_PREDICATE, NULL, $2, NULL); }
                 | HIDE_MARK NAMESPACE NAME                     { Parser_enqueue_attr(CRY_ENV, ATTR_INVISIBLE,   $2, $3, NULL); }
                 | HIDE_MARK           NAME                     { Parser_enqueue_attr(CRY_ENV, ATTR_INVISIBLE, NULL, $2, NULL); }
                 |           NAMESPACE NAME                     { Parser_enqueue_attr(CRY_ENV,              0,   $1, $2, NULL); }
                 |                     NAME                     { Parser_enqueue_attr(CRY_ENV,              0, NULL, $1, NULL); }
                 ;
TagAttrValue     : FullExpr
                 | OPEN_CURLY_BRACK FullExpr CLOSE_CURLY_BRACK { $$ = $2; }
                 ;
FunctionCall     : OPEN_FUNCTION_CALL CLOSE_PARENTHESIS          { $$ = Parser_function_expression(CRY_ENV, $1, CryData_array(2)); }
                 | OPEN_FUNCTION_CALL ArgsList CLOSE_PARENTHESIS { $$ = Parser_function_expression(CRY_ENV, $1,               $2); }
                 ;
MethodCall       : METHOD_CALL                                               { /*TODO: Method call*/ $$ = Parser_function_expression(CRY_ENV, $1, NULL); }
                 | METHOD_CALL OPEN_FUNCTION_CALL CLOSE_PARENTHESIS          { /*TODO: Method call*/ $$ = Parser_function_expression(CRY_ENV, $2, NULL); }
                 | METHOD_CALL OPEN_FUNCTION_CALL ArgsList CLOSE_PARENTHESIS { /*TODO: Method call*/ $$ = Parser_function_expression(CRY_ENV, $2,   $3); }
                 ;
IndexCall        : OPEN_SQUARE_BRACK Expression CLOSE_SQUARE_BRACK {/*TODO: Index call*/ $$ = $2; }
                 ;
ArrayExpr        : OPEN_SQUARE_BRACK CLOSE_SQUARE_BRACK          { $$ = CryExpr_init(EXPR_ARRAY, NULL); }
                 | OPEN_SQUARE_BRACK ArgsList CLOSE_SQUARE_BRACK { $$ = CryExpr_init(EXPR_ARRAY,   $2); }
                 ;
ArgsList         : FullExpr                          { $$ = CryData_array(1); Parser_append_arg($$, $1); }
                 | ArgsList COMMA_SEPARATOR FullExpr { $$ = $1;               Parser_append_arg($$, $3); }
                 ;
HashExpr         : OPEN_CURLY_BRACK CLOSE_CURLY_BRACK          { $$ = CryExpr_init(EXPR_HASH, NULL); }
                 | OPEN_CURLY_BRACK HashList CLOSE_CURLY_BRACK { $$ = CryExpr_init(EXPR_HASH,   $2); }
                 ;
HashList         : SINGLE_QUOTED_STRING COLON FullExpr                          { $$ = CryData_array(2); Parser_append_hash_key_and_value($$, $1, $3); }
                 | DOUBLE_QUOTED_STRING COLON FullExpr                          { $$ = CryData_array(2); Parser_append_hash_key_and_value($$, $1, $3); }
                 | HashList COMMA_SEPARATOR SINGLE_QUOTED_STRING COLON FullExpr { $$ = $1;               Parser_append_hash_key_and_value($$, $3, $5); }
                 | HashList COMMA_SEPARATOR DOUBLE_QUOTED_STRING COLON FullExpr { $$ = $1;               Parser_append_hash_key_and_value($$, $3, $5); }
                 ;
ScopeCall        : OPEN_SCOPE                                                CLOSE_SCOPE { $$ = Parser_scoped_value(CRY_ENV, $1); }
                 | OPEN_SCOPE { Parser_init_scope_data(CRY_ENV); } ScopeData CLOSE_SCOPE { $$ = Parser_scoped_value(CRY_ENV, $1); }
                 ;
ScopeData        : SCOPE_DATA                 { Parser_push_scope_data(CRY_ENV, $1);          }
                 | ScopeData SCOPE_DATA       { Parser_push_scope_data(CRY_ENV, $2);          }
                 |           OPEN_CURLY_BRACK { Parser_push_escaped_scope_data(CRY_ENV, "{"); } ScopeData { Parser_push_escaped_scope_data(CRY_ENV, "}"); } CLOSE_CURLY_BRACK
                 | ScopeData OPEN_CURLY_BRACK { Parser_push_escaped_scope_data(CRY_ENV, "{"); } ScopeData { Parser_push_escaped_scope_data(CRY_ENV, "}"); } CLOSE_CURLY_BRACK
                 ;
// TODO: Perform checks to determine if operation can't be allowed due incompatible data types
FullExpr         : NotExpr                    { $$ = $1;                                             }
                 | FullExpr OR_OP  NotExpr    { $$ = Parser_paired_expression(EXPR_OR,  $1, $3);     }
                 | FullExpr AND_OP NotExpr    { $$ = Parser_paired_expression(EXPR_AND, $1, $3);     }
                 ;
NotExpr          : CmpExpr                    { $$ = $1;                                             }
                 | NOT_OP CmpExpr             { $$ = CryExpr_init(EXPR_NOT, CryData_expression($2)); }
                 ;
CmpExpr          : ArithExpr                  { $$ = $1;                                         }
                 | CmpExpr EQ_OP ArithExpr    { $$ = Parser_paired_expression(EXPR_EQ, $1, $3);  }
                 | CmpExpr NE_OP ArithExpr    { $$ = Parser_paired_expression(EXPR_NE, $1, $3);  }
                 | CmpExpr LT_OP ArithExpr    { $$ = Parser_paired_expression(EXPR_LT, $1, $3);  }
                 | CmpExpr LE_OP ArithExpr    { $$ = Parser_paired_expression(EXPR_LE, $1, $3);  }
                 | CmpExpr GT_OP ArithExpr    { $$ = Parser_paired_expression(EXPR_GT, $1, $3);  }
                 | CmpExpr GE_OP ArithExpr    { $$ = Parser_paired_expression(EXPR_GE, $1, $3);  }
                 ;
ArithExpr        : MultiExpr                  { $$ = $1;                                         }
                 | ArithExpr ADD_OP MultiExpr { $$ = Parser_paired_expression(EXPR_ADD, $1, $3); }
                 | ArithExpr SUB_OP MultiExpr { $$ = Parser_paired_expression(EXPR_SUB, $1, $3); }
                 ;
MultiExpr        : UnaryExpr                  { $$ = $1;                                         }
                 | MultiExpr MUL_OP UnaryExpr { $$ = Parser_paired_expression(EXPR_MUL, $1, $3); }
                 | MultiExpr DIV_OP UnaryExpr { $$ = Parser_paired_expression(EXPR_DIV, $1, $3); }
                 | MultiExpr MOD_OP UnaryExpr { $$ = Parser_paired_expression(EXPR_MOD, $1, $3); }
                 ;
UnaryExpr        : Expression          { $$ = $1;                           }
                 | UnaryOps Expression { $$ = Parser_unary_add_sub($1, $2); }
                 ;
UnaryOps         : ADD_OP          { $$ = TRUE;    }
                 | SUB_OP          { $$ = FALSE;   }
                 | UnaryOps ADD_OP { $$ = $1;      }
                 | UnaryOps SUB_OP { $$ = $1^TRUE; }
                 ;
Expression       : ObjectExpr            { $$ = $1; }
                 | Expression MethodCall { $$ = $2; }
                 | Expression IndexCall  { $$ = $2; }
                 ;
ObjectExpr       : OPEN_PARENTHESIS FullExpr CLOSE_PARENTHESIS { $$ = $2;                         }
                 | ScopeCall                                   { $$ = $1;                         }
                 | FunctionCall                                { $$ = $1;                         }
                 | SimpleExpr                                  { $$ = $1;                         }
                 | ATTR_DATA                                   { $$ = Parser_data_value($1);      }
                 | ATTRIBUTE                                   { $$ = Parser_attribute_value($1); }
                 | ArrayExpr                                   { $$ = $1;                         }
                 | HashExpr                                    { $$ = $1;                         }
                 ;
SimpleExpr       : INTEGER              { $$ = Parser_integer_value($1);    }
                 | DOUBLE               { $$ = Parser_double_value($1);     }
                 | SINGLE_QUOTED_STRING { $$ = Parser_string_value($1);     }
                 | DOUBLE_QUOTED_STRING { $$ = Parser_string_value($1);     }
                 | TRUECT               { $$ = Parser_boolean_value(TRUE);  }
                 | FALSECT              { $$ = Parser_boolean_value(FALSE); }
                 | NULLCT               { $$ = Parser_null_value();         }
                 ;
%%

CryTag Parser_init_root_tag(CryEnv *env){
    if(env->root_tag == NULL){
        env->root_tag                        = CryTag_init();
        env->current_tag                     = env->root_tag;
        env->current_tag->eval               = CryExpr_init(EXPR_ENV, NULL);
        env->current_tag->eval->tag_function = &CryRender_tag_group;
    }
    env->status = ENV_PARSING_STATUS;
    return env->root_tag;
}

void Parser_finish(CryEnv *env){
    if( !(env->status & ENV_ERROR_MASK) ){
        env->status = ENV_PARSED_STATUS;
    }
}

CryTag Parser_open_content_list(CryEnv *env){
    env->current_tag = CryTag_init();
    return env->current_tag;
}

CryTag Parser_close_content_list(CryEnv *env, CryTag sentinel_tag){
    CryTag first_tag;

    first_tag          = sentinel_tag->next;
    sentinel_tag->next = NULL;
    CryTag_free(sentinel_tag);

    return first_tag;
}

CryTag Parser_open_tag(int flag, char *namespace, char *name){
    CryTag tag;

    tag = CryTag_init();
    if(flag != 0) CryTag_set_flag(tag, flag);
    if(namespace){
        CryTag_set_namespace(tag, namespace);
        free(namespace);
    }

    CryTag_set_name(tag, name);
    free(name);

    return tag;
}

void Parser_enqueue_tag(CryEnv *env, CryTag tag){
    env->current_tag->next = tag;
    env->current_tag       = tag;
}

CryTag Parser_close_opening_tag(CryEnv *env, CryTag tag, CryAttr attributes, int flag){
    tag->attributes = attributes;
    CryTag_set_flag(tag, flag);
    if(tag->flags & TAG_UNPARSED_TEXT){
        env->status                     = ENV_PARSING_CONTEXT_STATUS;
        env->closing_unparsed_full_name = CryTag_full_name(tag);
    }
    CryEnv_tag_lookup(env, tag);
    return env->current_tag;
}

CryTag Parser_close_tag(CryEnv *env, CryTag tag, CryTag first_child, CryTag push_tag){
    if(tag->flags & TAG_UNPARSED_TEXT){
        free(env->closing_unparsed_full_name);
        env->status                     = ENV_PARSING_STATUS;
        env->closing_unparsed_full_name = NULL;
    }
    tag->first_child = first_child;
    env->current_tag = push_tag;

    if(tag->flags & TAG_PRE_RENDER){
        CryRender_render_tag(NULL, tag, env->rendering_context, NULL);
    }
    return tag;
}

CryTag Parser_close_singleton_tag(CryEnv *env, CryTag tag, CryAttr attributes){
    CryTag_set_flag(tag, TAG_SINGLETON);
    tag->attributes = attributes;

    CryEnv_tag_lookup(env, tag);

    if(tag->flags & TAG_PRE_RENDER){
        CryRender_render_tag(NULL, tag, env->rendering_context, NULL);
    }

    return tag;
}

CryAttr Parser_open_attr_list(CryEnv *env){
    env->current_attribute = CryAttr_init(0);
    return env->current_attribute;
}

CryAttr Parser_close_attr_list(CryEnv *env, CryAttr sentinel_attr){
    CryAttr attr_list;

    attr_list              = sentinel_attr->next;
    sentinel_attr->next    = NULL;
    env->current_attribute = NULL;

    CryAttr_free(sentinel_attr);
    return attr_list;
}

void Parser_enqueue_attr(CryEnv *env, int flags, char *namespace, char *name, CryExpr expression){
    CryAttr attribute;

    attribute = CryAttr_init(flags);

    if(namespace){
        CryAttr_set_namespace(attribute, namespace);
        free(namespace);
    }

    if(expression == NULL){
        attribute->expression = CryExpr_init(EXPR_EVAL, CryData_string(name));
    }else{
        attribute->expression = expression;
    }

    CryAttr_set_name(attribute, name);
    free(name);
    env->current_attribute->next = attribute;
    env->current_attribute       = attribute;
}

CryExpr Parser_integer_value(long int integer){
    return CryExpr_init(EXPR_EVAL, CryData_integer(integer));
}

CryExpr Parser_double_value(long double cdouble){
    return CryExpr_init(EXPR_EVAL, CryData_double(cdouble));
}

CryExpr Parser_string_value(char *string){
    CryData *string_data;

    string_data = CryData_string(string);
    free(string);

    return CryExpr_init(EXPR_EVAL, string_data);
}


CryExpr Parser_data_value(char *data_name){
    CryData *string_data;

    string_data = CryData_string(data_name);
    free(data_name);

    return CryExpr_init(EXPR_DATA, string_data);
}

CryExpr Parser_attribute_value(char *attribute_name){
    CryData *string_data;

    string_data = CryData_string(attribute_name);
    free(attribute_name);

    return CryExpr_init(EXPR_ATTR, string_data);
}

CryExpr Parser_function_expression(CryEnv *env, char *function_name, CryData *array_data){
    CryExpr  expr;
    CryData *function_data;

    function_data = CryData_array(2);
    CryArray_add(function_data->value.array, CryData_string(function_name));
    if(array_data) CryArray_add(function_data->value.array, array_data);
    free(function_name);

    expr = CryExpr_init(EXPR_FUNC, function_data);
    CryEnv_function_lookup(env, expr);

    return expr;
}

void Parser_append_arg(CryData *array_data, CryExpr expression){
    CryArray_add(array_data->value.array, CryData_expression(expression));
}

void Parser_append_hash_key_and_value(CryData *array_data, char *key, CryExpr expression){
    CryArray_add( array_data->value.array, CryData_string(key)            );
    CryArray_add( array_data->value.array, CryData_expression(expression) );
    free(key);
}

CryExpr Parser_scoped_value(CryEnv *env, char *scope_name){
    char    *evaluable_string;
    CryExpr expr;

    if(env->current_text == NULL){
        expr = CryExpr_init(EXPR_SCOPED_CALL, CryData_string(""));
    }else{
        evaluable_string  = CryText_to_string(env->current_text);
        expr              = CryExpr_init(EXPR_SCOPED_CALL, CryData_string(evaluable_string));
        free(evaluable_string);
        CryText_free(env->current_text);
        env->current_text = NULL;
    }
    expr->scope = CryEnv_scope_lookup(env, scope_name);
    free(scope_name);

    if(expr->scope == NULL){
        // TODO: handle scope missing error!
        CryExpr_free(expr);
        return NULL;
    }
    return expr;
}

void Parser_init_scope_data(CryEnv *env){
    env->current_text = CryText_init();
}

void Parser_push_scope_data(CryEnv *env, char *string){
    CryText_append(env->current_text, string, TEXT_NON_BLANK);
    free(string);
}

void Parser_push_escaped_scope_data(CryEnv *env, char *string){
    CryText_append(env->current_text, string, TEXT_NON_BLANK);
}

CryExpr Parser_paired_expression(int opcode, CryExpr first_expr, CryExpr second_expr){
    CryExpr expr;

    expr = CryExpr_init(opcode, CryData_array(2));

    CryArray_add(expr->data->value.array, CryData_expression(first_expr));
    CryArray_add(expr->data->value.array, CryData_expression(second_expr));

    return expr;
}

CryExpr Parser_unary_add_sub(int positive_expr, CryExpr expression){
    if(positive_expr){
        return expression;
    }else{
        return CryExpr_init(EXPR_MINUS, CryData_expression(expression));
    }
}

CryExpr Parser_boolean_value(int boolean_value){
    return CryExpr_init(EXPR_EVAL, CryData_boolean(boolean_value));
}

CryExpr Parser_null_value(){
    return CryExpr_init(EXPR_EVAL, CryData_null());
}

void Parser_enqueue_text(CryEnv *env, CryTextElement *text_element){
    CryTag tag;

    if(env->current_tag != NULL && env->current_tag->flags & TAG_TEXT){
        CryText_append(env->current_tag->text, text_element->text, text_element->flags);
        CryTextElement_free(text_element);
    }else{
        tag                     = CryTag_init();
        tag->text               = CryText_init();
        tag->eval               = CryExpr_init(EXPR_ENV, NULL);
        tag->eval->tag_function = &CryRender_text_tag;
        CryTag_set_flag(tag, (TAG_TEXT | env->flags));
        CryText_append(tag->text, text_element->text, text_element->flags);
        CryTextElement_free(text_element);
        env->current_tag->next = tag;
        env->current_tag       = tag;
    }
}

int Parser_parse(CryEnv *env, char *filename, int state){
    CryParseContext context;

    context.env = env;
    yylex_init_extra(env, &(context.scanner));
    yyset_in(fopen(filename, "r"), context.scanner); // TODO: Handle error when file is missing!

    yyparse(&context);
    yylex_destroy(context.scanner);

    return 0;
}