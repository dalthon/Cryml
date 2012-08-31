%{
#include "../cryml/cryml.h"

// #define YY_USER_ACTION printf("\n\"%10s\"", yyget_text(yyscanner));
// #define YY_USER_ACTION printf("\n\"%d\"", yyget_lineno(yyscanner));

#include "../generated/syntactic.h"

#define PUSH_STATE(old_state, new_state) Lexer_push_state(yyscanner, old_state); BEGIN(new_state)
#define POP_STATE                        BEGIN(Lexer_pop_state(yyscanner))
%}

open_closing_tag               \<\/
open_tag                       \<
close_singleton_tag            \/\>
close_unparsed_tag             \&\>
close_compressed_tag           \~\>
close_tag                      \>

comment                        \<\!\-\-(.|{blank})*\-\-\>
doctype                        (\<\!DOCTYPE[^\>]*\>)|(\<\!doctype[^\>]*\>)
erb                            \<\%[^\%]*\%\>

open_scope_parse               \{
close_scope_parse              \}

open_curly_brackets            \{
close_curly_brackets           \}

open_escaped_curly_brackets    \\\{
close_escaped_curly_brackets   \\\}

open_square_brackets           \[
close_square_brackets          \]

open_parenthesis               \(
close_parenthesis              \)

hide_mark                      \*
attr_mark                      \@
data_mark                      \$

integer                        {digit}+
double                         (({digit}+\.{digit}+([Ee](\+|\-)?{digit}+)?)|({digit}+([Ee](\+|\-)?{digit}+)))
single_quoted_string           \'([^\']|(\\\'))*\'
double_quoted_string           \"([^\"]|(\\\"))*\"
attribute                      \@({name}\:)?{name}
data_attr                      \$({name}\:)?{name}

open_scope                     {name}\{
open_function_call             {name}\(
method_call                    \.

assign                         \=

close_name                     \@?({name}\:)?{name}
namespace                      {name}\:
name                           {char}({separator}|{char}|{digit})*

colon                          \:
comma_separator                \,
blank                          [ \t\n\r]+
char                           [A-Za-z]
digit                          [0-9]
separator                      [\-\_]

scope_data                     [^\{\}(\\\{)(\\\})]+
function_data                  [^\(\)\,]+

text                           [^\< \t\n\r]+
unparsed_text                  [^\<]+
unparsed_end                   \<\/{close_name}\>
unparsed_less_than             \<

or_op                          \|\|
and_op                         \&\&
not_op                         \!

eq_op                          \=\=
ne_op                          \!\=
lt_op                          \<
le_op                          \<\=
gt_op                          \>
ge_op                          \>\=

mul_op                         \*
div_op                         \/
mod_op                         \%
add_op                         \+
sub_op                         \-

%option reentrant stack
%option extra-type="CryEnv *"
%option bison-bridge
%option yylineno

%x TAG_CTX
%x TAG_NAMESPACE_CTX
%x TAG_NAME_CTX

%x TAG_CLOSE_CTX
%x TAG_CLOSE_END_CTX

%x ATTR_OR_END_CTX
%x ATTR_NAMESPACE_CTX
%x ATTR_NAME_CTX
%x ATTR_ASSIGN_CTX
%x ASSIGN_CTX

%x SIMPLE_EXPR_CTX
%x FULL_EXPRESSION_CTX

%x SCOPE_CTX
%x INNER_SCOPE_CTX

%x UNPARSED_CTX

%x IGNORE_CTX

%%

<INITIAL>{open_closing_tag}                             { BEGIN(TAG_CLOSE_CTX);                          return OPEN_CLOSING_TAG; }
<INITIAL>{open_tag}                                     { BEGIN(TAG_CTX);                                return OPEN_TAG;         }
<INITIAL>{blank}                                        { Lexer_text_element(yyscanner, TEXT_BLANK);     return TEXT;             }
<INITIAL>{text}                                         { Lexer_text_element(yyscanner, TEXT_NON_BLANK); return TEXT;             }
<INITIAL>{comment}                                      { Lexer_text_element(yyscanner, TEXT_NON_BLANK); return TEXT;             }
<INITIAL>{doctype}                                      { Lexer_text_element(yyscanner, TEXT_NON_BLANK); return TEXT;             }
<INITIAL>{erb}                                          { Lexer_text_element(yyscanner, TEXT_NON_BLANK); return TEXT;             }

<UNPARSED_CTX>{unparsed_end}                            { if(Lexer_unparsed_ended(yyscanner)){ BEGIN(INITIAL); return UNPARSED_END; } Lexer_text_element(yyscanner, TEXT_NON_BLANK); return TEXT; }
<UNPARSED_CTX>{unparsed_text}                           {                                                                             Lexer_text_element(yyscanner, TEXT_NON_BLANK); return TEXT; }
<UNPARSED_CTX>{unparsed_less_than}                      {                                                                             Lexer_text_element(yyscanner, TEXT_NON_BLANK); return TEXT; }

<TAG_CTX>{hide_mark}                                    { BEGIN(TAG_NAMESPACE_CTX);                             return HIDE_MARK; }
<TAG_CTX>{attr_mark}                                    { BEGIN(TAG_NAMESPACE_CTX);                             return ATTR_MARK; }
<TAG_CTX>{data_mark}                                    { BEGIN(TAG_NAMESPACE_CTX);                             return DATA_MARK; }
<TAG_CTX>{namespace}                                    { BEGIN(TAG_NAME_CTX);      Lexer_namespace(yyscanner); return NAMESPACE; }
<TAG_CTX>{name}                                         { BEGIN(ATTR_OR_END_CTX);   Lexer_name(yyscanner);      return NAME;      }
<TAG_NAMESPACE_CTX>{namespace}                          { BEGIN(TAG_NAME_CTX);      Lexer_namespace(yyscanner); return NAMESPACE; }
<TAG_NAMESPACE_CTX>{name}                               { BEGIN(ATTR_OR_END_CTX);   Lexer_name(yyscanner);      return NAME;      }
<TAG_NAME_CTX>{name}                                    { BEGIN(ATTR_OR_END_CTX);   Lexer_name(yyscanner);      return NAME;      }

<ATTR_OR_END_CTX,ATTR_ASSIGN_CTX>{colon}                { BEGIN(ATTR_NAMESPACE_CTX);                             return COLON;     }
<ATTR_OR_END_CTX,ATTR_ASSIGN_CTX>{hide_mark}            { BEGIN(ATTR_NAMESPACE_CTX);                             return HIDE_MARK; }
<ATTR_OR_END_CTX,ATTR_ASSIGN_CTX>{namespace}            { BEGIN(ATTR_NAME_CTX);      Lexer_namespace(yyscanner); return NAMESPACE; }
<ATTR_OR_END_CTX,ATTR_ASSIGN_CTX>{name}                 { BEGIN(ATTR_ASSIGN_CTX);    Lexer_name(yyscanner);      return NAME;      }
<ATTR_NAMESPACE_CTX>{namespace}                         { BEGIN(ATTR_NAME_CTX);      Lexer_namespace(yyscanner); return NAMESPACE; }
<ATTR_NAMESPACE_CTX>{name}                              { BEGIN(ATTR_ASSIGN_CTX);    Lexer_name(yyscanner);      return NAME;      }
<ATTR_NAME_CTX>{name}                                   { BEGIN(ATTR_ASSIGN_CTX);    Lexer_name(yyscanner);      return NAME;      }
<ATTR_ASSIGN_CTX>{assign}                               { BEGIN(ASSIGN_CTX);                                     return ASSIGN;    }

<ASSIGN_CTX>{add_op}                                    { BEGIN(SIMPLE_EXPR_CTX); return ADD_OP; }
<ASSIGN_CTX>{sub_op}                                    { BEGIN(SIMPLE_EXPR_CTX); return SUB_OP; }
<ASSIGN_CTX>{not_op}                                    { BEGIN(SIMPLE_EXPR_CTX); return NOT_OP; }

<ASSIGN_CTX,SIMPLE_EXPR_CTX>{double}                    { BEGIN(ATTR_OR_END_CTX);                           Lexer_double(yyscanner);               return DOUBLE;               }
<ASSIGN_CTX,SIMPLE_EXPR_CTX>{integer}                   { BEGIN(ATTR_OR_END_CTX);                           Lexer_integer(yyscanner);              return INTEGER;              }
<ASSIGN_CTX,SIMPLE_EXPR_CTX>{single_quoted_string}      { BEGIN(ATTR_OR_END_CTX);                           Lexer_single_quoted_string(yyscanner); return SINGLE_QUOTED_STRING; }
<ASSIGN_CTX,SIMPLE_EXPR_CTX>{double_quoted_string}      { BEGIN(ATTR_OR_END_CTX);                           Lexer_double_quoted_string(yyscanner); return DOUBLE_QUOTED_STRING; }
<ASSIGN_CTX,SIMPLE_EXPR_CTX>{data_attr}                 { BEGIN(ATTR_OR_END_CTX);                           Lexer_data_attr(yyscanner);            return ATTR_DATA;            }
<ASSIGN_CTX,SIMPLE_EXPR_CTX>{attribute}                 { BEGIN(ATTR_OR_END_CTX);                           Lexer_attribute(yyscanner);            return ATTRIBUTE;            }
<ASSIGN_CTX,SIMPLE_EXPR_CTX>true                        { BEGIN(ATTR_OR_END_CTX);                                                                  return TRUECT;               }
<ASSIGN_CTX,SIMPLE_EXPR_CTX>false                       { BEGIN(ATTR_OR_END_CTX);                                                                  return FALSECT;              }
<ASSIGN_CTX,SIMPLE_EXPR_CTX>null                        { BEGIN(ATTR_OR_END_CTX);                                                                  return NULLCT;               }
<ASSIGN_CTX,SIMPLE_EXPR_CTX>{open_scope}                { PUSH_STATE(ATTR_OR_END_CTX, SCOPE_CTX);           Lexer_open_scope(yyscanner);           return OPEN_SCOPE;           }
<ASSIGN_CTX,SIMPLE_EXPR_CTX>{open_function_call}        { PUSH_STATE(ATTR_OR_END_CTX, FULL_EXPRESSION_CTX); Lexer_open_function(yyscanner);        return OPEN_FUNCTION_CALL;   }

<ASSIGN_CTX>{open_square_brackets}                      { PUSH_STATE(ATTR_OR_END_CTX, FULL_EXPRESSION_CTX); return OPEN_SQUARE_BRACK; }
<ASSIGN_CTX>{open_curly_brackets}                       { PUSH_STATE(ATTR_OR_END_CTX, FULL_EXPRESSION_CTX); return OPEN_CURLY_BRACK;  }

<FULL_EXPRESSION_CTX>{double}                           { Lexer_double(yyscanner);                                                              return DOUBLE;               }
<FULL_EXPRESSION_CTX>{integer}                          { Lexer_integer(yyscanner);                                                             return INTEGER;              }
<FULL_EXPRESSION_CTX>{single_quoted_string}             { Lexer_single_quoted_string(yyscanner);                                                return SINGLE_QUOTED_STRING; }
<FULL_EXPRESSION_CTX>{double_quoted_string}             { Lexer_double_quoted_string(yyscanner);                                                return DOUBLE_QUOTED_STRING; }
<FULL_EXPRESSION_CTX>{data_attr}                        { Lexer_data_attr(yyscanner);                                                           return ATTR_DATA;            }
<FULL_EXPRESSION_CTX>{attribute}                        { Lexer_attribute(yyscanner);                                                           return ATTRIBUTE;            }
<FULL_EXPRESSION_CTX>true                               {                                                                                       return TRUECT;               }
<FULL_EXPRESSION_CTX>false                              {                                                                                       return FALSECT;              }
<FULL_EXPRESSION_CTX>null                               {                                                                                       return NULLCT;               }
<FULL_EXPRESSION_CTX>{method_call}                      { /* TODO: Finish! */                                                                   return METHOD_CALL;          }
<FULL_EXPRESSION_CTX>{open_scope}                       { PUSH_STATE(FULL_EXPRESSION_CTX, SCOPE_CTX);           Lexer_open_scope(yyscanner);    return OPEN_SCOPE;           }
<FULL_EXPRESSION_CTX>{open_function_call}               { PUSH_STATE(FULL_EXPRESSION_CTX, FULL_EXPRESSION_CTX); Lexer_open_function(yyscanner); return OPEN_FUNCTION_CALL;   }
<FULL_EXPRESSION_CTX>{colon}                            {                                                                                       return COLON;                }
<FULL_EXPRESSION_CTX>{comma_separator}                  {                                                                                       return COMMA_SEPARATOR;      }
<FULL_EXPRESSION_CTX>{open_square_brackets}             { PUSH_STATE(FULL_EXPRESSION_CTX, FULL_EXPRESSION_CTX);                                 return OPEN_SQUARE_BRACK;    }
<FULL_EXPRESSION_CTX>{close_square_brackets}            { POP_STATE;                                                                            return CLOSE_SQUARE_BRACK;   }
<FULL_EXPRESSION_CTX>{open_curly_brackets}              { PUSH_STATE(FULL_EXPRESSION_CTX, FULL_EXPRESSION_CTX);                                 return OPEN_CURLY_BRACK;     }
<FULL_EXPRESSION_CTX>{close_curly_brackets}             { POP_STATE;                                                                            return CLOSE_CURLY_BRACK;    }
<FULL_EXPRESSION_CTX>{open_parenthesis}                 { PUSH_STATE(FULL_EXPRESSION_CTX, FULL_EXPRESSION_CTX);                                 return OPEN_PARENTHESIS;     }
<FULL_EXPRESSION_CTX>{close_parenthesis}                { POP_STATE;                                                                            return CLOSE_PARENTHESIS;    }

<FULL_EXPRESSION_CTX>{or_op}                            { return OR_OP;  }
<FULL_EXPRESSION_CTX>{and_op}                           { return AND_OP; }
<FULL_EXPRESSION_CTX>{not_op}                           { return NOT_OP; }
<FULL_EXPRESSION_CTX>{eq_op}                            { return EQ_OP;  }
<FULL_EXPRESSION_CTX>{ne_op}                            { return NE_OP;  }
<FULL_EXPRESSION_CTX>{lt_op}                            { return LT_OP;  }
<FULL_EXPRESSION_CTX>{le_op}                            { return LE_OP;  }
<FULL_EXPRESSION_CTX>{gt_op}                            { return GT_OP;  }
<FULL_EXPRESSION_CTX>{ge_op}                            { return GE_OP;  }
<FULL_EXPRESSION_CTX>{mul_op}                           { return MUL_OP; }
<FULL_EXPRESSION_CTX>{div_op}                           { return DIV_OP; }
<FULL_EXPRESSION_CTX>{mod_op}                           { return MOD_OP; }
<FULL_EXPRESSION_CTX>{add_op}                           { return ADD_OP; }
<FULL_EXPRESSION_CTX>{sub_op}                           { return SUB_OP; }

<SCOPE_CTX>{open_escaped_curly_brackets}                { Lexer_scope_escaped_data(yyscanner);    return SCOPE_DATA;       }
<SCOPE_CTX>{close_escaped_curly_brackets}               { Lexer_scope_escaped_data(yyscanner);    return SCOPE_DATA;       }
<SCOPE_CTX>{open_curly_brackets}                        { PUSH_STATE(SCOPE_CTX, INNER_SCOPE_CTX); return OPEN_CURLY_BRACK; }
<SCOPE_CTX>{close_scope_parse}                          { POP_STATE;                              return CLOSE_SCOPE;      }
<SCOPE_CTX>{scope_data}                                 { Lexer_scope_data(yyscanner);            return SCOPE_DATA;       }

<INNER_SCOPE_CTX>{open_escaped_curly_brackets}          { Lexer_scope_escaped_data(yyscanner);          return SCOPE_DATA;        }
<INNER_SCOPE_CTX>{close_escaped_curly_brackets}         { Lexer_scope_escaped_data(yyscanner);          return SCOPE_DATA;        }
<INNER_SCOPE_CTX>{open_curly_brackets}                  { PUSH_STATE(INNER_SCOPE_CTX, INNER_SCOPE_CTX); return OPEN_CURLY_BRACK;  }
<INNER_SCOPE_CTX>{close_curly_brackets}                 { POP_STATE;                                    return CLOSE_CURLY_BRACK; }
<INNER_SCOPE_CTX>{scope_data}                           { Lexer_scope_data(yyscanner);                  return SCOPE_DATA;        }

<ATTR_OR_END_CTX,ATTR_ASSIGN_CTX>{close_singleton_tag}  { BEGIN(INITIAL);                                                                   return CLOSE_SINGLETON_TAG;  }
<ATTR_OR_END_CTX,ATTR_ASSIGN_CTX>{close_unparsed_tag}   { BEGIN(UNPARSED_CTX);                                                              return CLOSE_UNPARSED_TAG;   }
<ATTR_OR_END_CTX,ATTR_ASSIGN_CTX>{close_compressed_tag} { if(Lexer_unparsed_context(yyscanner)){BEGIN(UNPARSED_CTX);}else{BEGIN(INITIAL);}; return CLOSE_COMPRESSED_TAG; }
<ATTR_OR_END_CTX,ATTR_ASSIGN_CTX>{close_tag}            { if(Lexer_unparsed_context(yyscanner)){BEGIN(UNPARSED_CTX);}else{BEGIN(INITIAL);}; return CLOSE_TAG;            }

<TAG_CLOSE_CTX>{close_name}                             { BEGIN(TAG_CLOSE_END_CTX); return CLOSE_NAME; }
<TAG_CLOSE_END_CTX>{close_tag}                          { BEGIN(INITIAL);           return CLOSE_TAG;  }

<TAG_CTX,TAG_NAMESPACE_CTX,ATTR_OR_END_CTX,ATTR_ASSIGN_CTX,ASSIGN_CTX>{blank} { ; }
<TAG_CLOSE_CTX,TAG_CLOSE_END_CTX,SIMPLE_EXPR_CTX,FULL_EXPRESSION_CTX>{blank}  { ; }
<IGNORE_CTX>(.|\n)+                                                           { ; }

<*>.|\n { BEGIN(IGNORE_CTX); return UNRECOGNIZED_TOKEN; }

%%

void Lexer_namespace(yyscan_t scanner){
    int       length;
    char     *namespace;
    CrySType *value;

    value         = yyget_lval(scanner);
    namespace     = yyget_text(scanner);
    length        = strlen(namespace);
    value->string = malloc(length*sizeof(char));

    sprintf(value->string, "%s", namespace);
    (value->string)[length-1] = '\0';
}

void Lexer_name(yyscan_t scanner){
    char     *name;
    CrySType *value;

    value         = yyget_lval(scanner);
    name          = yyget_text(scanner);
    value->string = strdup(name);
}

void Lexer_text_element(yyscan_t scanner, int flag){
    char     *text;
    CrySType *value;

    value               = yyget_lval(scanner);
    text                = yyget_text(scanner);
    value->text_element = CryTextElement_init(text, flag);
}

void Lexer_double(yyscan_t scanner){
    CrySType *value;

    value = yyget_lval(scanner);
    sscanf(yyget_text(scanner), "%Lf", &(value->cdouble));
}

void Lexer_integer(yyscan_t scanner){
    CrySType *value;

    value = yyget_lval(scanner);
    sscanf(yyget_text(scanner), "%ld", &(value->integer));
}

void Lexer_single_quoted_string(yyscan_t scanner){
    int       i, j, length;
    char     *text;
    CrySType *value;

    text          = yyget_text(scanner);
    length        = strlen(text);
    value         = yyget_lval(scanner);
    value->string = malloc(length*sizeof(char));

    for(i = 1, j = 0; i < length-1; i++, j++){
        if(text[i] == '\\' && i < length-2 && text[i+1] == '\''){
            (value->string)[j] = '\'';
            i++;
        }else{
            (value->string)[j] = text[i];
        }
    }
    (value->string)[j] = '\0';
}

void Lexer_double_quoted_string(yyscan_t scanner){
    int       i, j, length;
    char     *text;
    CrySType *value;

    text          = yyget_text(scanner);
    length        = strlen(text);
    value         = yyget_lval(scanner);
    value->string = malloc(length*sizeof(char));

    for(i = 1, j = 0; i < length-1; i++, j++){
        if(text[i] == '\\' && i < length-2 && text[i+1] == '\"'){
            (value->string)[j] = '\"';
            i++;
        }else{
            (value->string)[j] = text[i];
        }
    }
    (value->string)[j] = '\0';
}

void Lexer_data_attr(yyscan_t scanner){
    char     *data_name;
    CrySType *value;

    value         = yyget_lval(scanner);
    data_name     = yyget_text(scanner);
    value->string = strdup(data_name+1);
}

void Lexer_attribute(yyscan_t scanner){
    char     *attr_name;
    CrySType *value;

    value         = yyget_lval(scanner);
    attr_name     = yyget_text(scanner);
    value->string = strdup(attr_name+1);
}

void Lexer_open_scope(yyscan_t scanner){
    int       length;
    char     *scope_name;
    CrySType *value;

    value         = yyget_lval(scanner);
    scope_name    = yyget_text(scanner);
    length        = strlen(scope_name);
    value->string = malloc(length*sizeof(char));

    sprintf(value->string, "%s", scope_name);
    (value->string)[length-1] = '\0';
}

void Lexer_open_function(yyscan_t scanner){
    int       length;
    char     *scope_name;
    CrySType *value;

    value         = yyget_lval(scanner);
    scope_name    = yyget_text(scanner);
    length        = strlen(scope_name);
    value->string = malloc(length*sizeof(char));

    sprintf(value->string, "%s", scope_name);
    (value->string)[length-1] = '\0';
}

void Lexer_scope_escaped_data(yyscan_t scanner){
    int       length;
    char     *scope_data;
    CrySType *value;

    value         = yyget_lval(scanner);
    scope_data    = yyget_text(scanner);
    length        = strlen(scope_data);
    value->string = malloc((length + 1)*sizeof(char));

    strcpy(value->string, &(scope_data[1]));
    (value->string)[length] = '\0';
}

void Lexer_scope_data(yyscan_t scanner){
    char     *scope_data;
    CrySType *value;

    value         = yyget_lval(scanner);
    scope_data    = yyget_text(scanner);
    value->string = strdup(scope_data);
}

int Lexer_unparsed_ended(yyscan_t scanner){
    int     length, result;
    char   *text, *full_name, *tag_full_name;

    tag_full_name = yyget_extra(scanner)->closing_unparsed_full_name;
    text          = yyget_text(scanner);
    length        = strlen(text);
    full_name     = malloc((length-2)*sizeof(char));

    strcpy(full_name, &(text[2]));
    full_name[length-3] = '\0';

    result = (strcmp(tag_full_name, full_name) == 0);
    free(full_name);

    return result;
}

int Lexer_unparsed_context(yyscan_t scanner){
    return( yyget_extra(scanner)->status == ENV_PARSING_CONTEXT_STATUS );
}

void Lexer_push_state(yyscan_t scanner, int state){
    CryEnv      *env;
    CryLexState  lex_state;

    env              = yyget_extra(scanner);
    lex_state        = malloc(sizeof(CryLexStateStack));
    lex_state->next  = env->lex_states;
    lex_state->state = state;
    env->lex_states  = lex_state;
}

int Lexer_pop_state(yyscan_t scanner){
    int          last_state;
    CryEnv      *env;
    CryLexState  lex_state;

    env             = yyget_extra(scanner);
    lex_state       = env->lex_states;
    last_state      = lex_state->state;
    env->lex_states = lex_state->next;
    lex_state->next = NULL;
    free(lex_state);

    return last_state;
}

int yyerror(const char *msg){
    fprintf(stderr, "Error: %s\n", msg);
    return 0;
}