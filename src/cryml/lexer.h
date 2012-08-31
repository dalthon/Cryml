// Lex's Extra Type Definition
#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif

// Function Prototypes
void Lexer_namespace(yyscan_t scanner);
void Lexer_name(yyscan_t scanner);
void Lexer_text_element(yyscan_t scanner, int flag);
void Lexer_double(yyscan_t scanner);
void Lexer_integer(yyscan_t scanner);
void Lexer_single_quoted_string(yyscan_t scanner);
void Lexer_double_quoted_string(yyscan_t scanner);
void Lexer_data_attr(yyscan_t scanner);
void Lexer_attribute(yyscan_t scanner);
void Lexer_open_scope(yyscan_t scanner);
void Lexer_open_function(yyscan_t scanner);
void Lexer_scope_escaped_data(yyscan_t scanner);
void Lexer_scope_data(yyscan_t scanner);
int  Lexer_close_expression(yyscan_t scanner);
int  Lexer_unparsed_ended(yyscan_t scanner);
int  Lexer_unparsed_context(yyscan_t scanner);
void Lexer_push_state(yyscan_t scanner, int state);
int  Lexer_pop_state(yyscan_t scanner);