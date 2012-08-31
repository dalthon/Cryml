#include "../../src/generated/lexical.c"
#include "unit.h"

#define EXECUTE_TWICE(x) x ; x

// Test Array expressions
#define GET_ARRAY_EXPRESSION(index)     CryArray_get(array, index)->value.expression
#define GET_ARRAY_ELEMENT(index)        GET_ARRAY_EXPRESSION(index)->data->value

// Test function expressions
#define GET_FUNCTION_NAME               CryArray_get(array, 0)->value.string
#define GET_FUNCTION_ARRAY              CryArray_get(array, 1)
#define GET_FUNCTION_ARGS               GET_FUNCTION_ARRAY->value.array
#define GET_FUNCTION_ARG_COUNT          CryArray_size(GET_FUNCTION_ARGS)
#define GET_FUNCTION_ARG_EXPR(index)    CryArray_get(GET_FUNCTION_ARGS, index)->value.expression
#define GET_FUNCTION_ARG_VALUE(index)   GET_FUNCTION_ARG_EXPR(index)->data->value

int before_all_test_parser(void){ return 0; }
int after_all_test_parser(void){  return 0; }

CryEnv *init_parser_environment(char *scope_name){
    CryEnv *env;

    env               = CryEnv_init();
    env->scopes       = malloc(sizeof(CryScope));
    env->scopes->name = strdup(scope_name);

    return env;
}

void free_parser_environment(CryEnv *env){
    free(env->scopes->name);
    free(env->scopes);
    CryEnv_free(env);
}

void test_simple_text_tag(void){
    CryEnv         *env;
    CryTextElement *text_element;
    env = init_parser_environment("rb");

    EXECUTE_TWICE(
        Parser_parse(env, "test/examples/simple_text_tag.cryml", 0);

        CU_ASSERT_EQUAL(env->root_tag->next,                     NULL    );
        CU_ASSERT_EQUAL(env->root_tag->first_child->flags,       TAG_TEXT);
        CU_ASSERT_EQUAL(env->root_tag->first_child->first_child, NULL    );
        CU_ASSERT_EQUAL(env->root_tag->first_child->next,        NULL    );
        CU_ASSERT_EQUAL(env->root_tag->first_child->namespace,   NULL    );
        CU_ASSERT_EQUAL(env->root_tag->first_child->name,        NULL    );

        text_element = env->root_tag->first_child->text->first_element;
        CU_ASSERT_EQUAL(text_element->flags,       TEXT_NON_BLANK);
        CU_ASSERT_STRING_EQUAL(text_element->text, "Chunky"      );

        text_element = text_element->next;
        CU_ASSERT_EQUAL(text_element->flags,       TEXT_BLANK);
        CU_ASSERT_STRING_EQUAL(text_element->text, " "       );

        text_element = text_element->next;
        CU_ASSERT_EQUAL(text_element->flags,       TEXT_NON_BLANK);
        CU_ASSERT_STRING_EQUAL(text_element->text, "Bacon!"      );

        CU_ASSERT_EQUAL(text_element->next, NULL);
        CryEnv_cleanup(env);
    );

    free_parser_environment(env);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_simple_open_close_tag(void){
    CryEnv *env;
    env = init_parser_environment("rb");

    EXECUTE_TWICE(
        Parser_parse(env, "test/examples/simple_open_close_tag.cryml", 0);

        CU_ASSERT_EQUAL(env->root_tag->next,                          NULL       );
        CU_ASSERT_EQUAL(env->root_tag->first_child->flags,            TAG_CLEAN  );
        CU_ASSERT_EQUAL(env->root_tag->first_child->first_child,      NULL       );
        CU_ASSERT_EQUAL(env->root_tag->first_child->next,             NULL       );
        CU_ASSERT_STRING_EQUAL(env->root_tag->first_child->namespace, "open"     );
        CU_ASSERT_STRING_EQUAL(env->root_tag->first_child->name,      "close_tag");
        CryEnv_cleanup(env);
    );

    free_parser_environment(env);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_simple_singleton_tag(void){
    CryEnv *env;
    env = init_parser_environment("rb");

    EXECUTE_TWICE(
        Parser_parse(env, "test/examples/simple_singleton_tag.cryml", 0);

        CU_ASSERT_EQUAL(env->root_tag->next,                     NULL         );
        CU_ASSERT_EQUAL(env->root_tag->first_child->flags,       TAG_SINGLETON);
        CU_ASSERT_EQUAL(env->root_tag->first_child->first_child, NULL         );
        CU_ASSERT_EQUAL(env->root_tag->first_child->next,        NULL         );
        CU_ASSERT_EQUAL(env->root_tag->first_child->namespace,   NULL         );
        CU_ASSERT_STRING_EQUAL(env->root_tag->first_child->name, "singleton"  );
        CryEnv_cleanup(env);
    );

    free_parser_environment(env);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_simple_html(void){
    char   *string;
    CryEnv *env;
    CryTag  tag, sibling_tag;

    env = init_parser_environment("rb");

    EXECUTE_TWICE(
        Parser_parse(env, "test/examples/simple_html.cryml", 0);

        tag = env->root_tag;
        CU_ASSERT_EQUAL(tag->next, NULL);

        tag = tag->first_child;
        CU_ASSERT_EQUAL(tag->flags,       TAG_CLEAN);
        CU_ASSERT_EQUAL(tag->next,        NULL     );
        CU_ASSERT_EQUAL(tag->text,        NULL     );
        CU_ASSERT_EQUAL(tag->attributes,  NULL     );
        CU_ASSERT_EQUAL(tag->namespace,   NULL     );
        CU_ASSERT_STRING_EQUAL(tag->name, "html"   );

        tag = tag->first_child;
        CU_ASSERT_EQUAL(tag->flags,                      TAG_TEXT  );
        CU_ASSERT_EQUAL(tag->first_child,                NULL      );
        CU_ASSERT_EQUAL(tag->namespace,                  NULL      );
        CU_ASSERT_EQUAL(tag->name,                       NULL      );
        CU_ASSERT_EQUAL(tag->text->first_element->flags, TEXT_BLANK);
        CU_ASSERT_EQUAL(tag->text->first_element->next,  NULL      );

        tag = tag->next;
        CU_ASSERT_EQUAL(tag->flags,       TAG_CLEAN);
        CU_ASSERT_EQUAL(tag->text,        NULL     );
        CU_ASSERT_EQUAL(tag->attributes,  NULL     );
        CU_ASSERT_EQUAL(tag->namespace,   NULL     );
        CU_ASSERT_STRING_EQUAL(tag->name, "head"   );

        sibling_tag = tag->next;
        tag = tag->first_child;
        CU_ASSERT_EQUAL(tag->flags,       TAG_CLEAN);
        CU_ASSERT_EQUAL(tag->text,        NULL     );
        CU_ASSERT_EQUAL(tag->attributes,  NULL     );
        CU_ASSERT_EQUAL(tag->namespace,   NULL     );
        CU_ASSERT_STRING_EQUAL(tag->name, "title"  );

        CU_ASSERT_EQUAL(tag->next->flags,                      TAG_TEXT  );
        CU_ASSERT_EQUAL(tag->next->first_child,                NULL      );
        CU_ASSERT_EQUAL(tag->next->namespace,                  NULL      );
        CU_ASSERT_EQUAL(tag->next->name,                       NULL      );
        CU_ASSERT_EQUAL(tag->next->next,                       NULL      );
        CU_ASSERT_EQUAL(tag->next->text->first_element->flags, TEXT_BLANK);
        CU_ASSERT_EQUAL(tag->next->text->first_element->next,  NULL      );

        tag = tag->first_child;
        CU_ASSERT_EQUAL(tag->flags,                                  TAG_TEXT      );
        CU_ASSERT_EQUAL(tag->first_child,                            NULL          );
        CU_ASSERT_EQUAL(tag->namespace,                              NULL          );
        CU_ASSERT_EQUAL(tag->name,                                   NULL          );
        CU_ASSERT_EQUAL(tag->next,                                   NULL          );
        CU_ASSERT_EQUAL(tag->text->first_element->flags,             TEXT_NON_BLANK);
        CU_ASSERT_EQUAL(tag->text->first_element->next->flags,       TEXT_BLANK    );
        CU_ASSERT_EQUAL(tag->text->first_element->next->next->flags, TEXT_NON_BLANK);
        CU_ASSERT_EQUAL(tag->text->first_element->next->next->next,  NULL          );

        string = CryText_to_string(tag->text);
        CU_ASSERT_STRING_EQUAL(string, "Cryml Rocks!");
        free(string);

        tag = sibling_tag;
        CU_ASSERT_EQUAL(tag->flags,                      TAG_TEXT  );
        CU_ASSERT_EQUAL(tag->first_child,                NULL      );
        CU_ASSERT_EQUAL(tag->namespace,                  NULL      );
        CU_ASSERT_EQUAL(tag->name,                       NULL      );
        CU_ASSERT_EQUAL(tag->text->first_element->flags, TEXT_BLANK);
        CU_ASSERT_EQUAL(tag->text->first_element->next,  NULL      );

        tag = tag->next;
        CU_ASSERT_EQUAL(tag->flags,       TAG_CLEAN);
        CU_ASSERT_EQUAL(tag->text,        NULL     );
        CU_ASSERT_EQUAL(tag->namespace,   NULL     );
        CU_ASSERT_STRING_EQUAL(tag->name, "body"   );
        CU_ASSERT_EQUAL(tag->attributes,  NULL     );
        CU_ASSERT_NOT_EQUAL(tag->next,    NULL     );

        sibling_tag = tag->next;
        CU_ASSERT_EQUAL(sibling_tag->flags,                      TAG_TEXT  );
        CU_ASSERT_EQUAL(sibling_tag->first_child,                NULL      );
        CU_ASSERT_EQUAL(sibling_tag->namespace,                  NULL      );
        CU_ASSERT_EQUAL(sibling_tag->name,                       NULL      );
        CU_ASSERT_EQUAL(sibling_tag->text->first_element->flags, TEXT_BLANK);
        CU_ASSERT_EQUAL(sibling_tag->text->first_element->next,  NULL      );

        tag = tag->first_child;
        CU_ASSERT_EQUAL(tag->flags,                      TAG_TEXT  );
        CU_ASSERT_EQUAL(tag->first_child,                NULL      );
        CU_ASSERT_EQUAL(tag->namespace,                  NULL      );
        CU_ASSERT_EQUAL(tag->name,                       NULL      );
        CU_ASSERT_EQUAL(tag->text->first_element->flags, TEXT_BLANK);
        CU_ASSERT_EQUAL(tag->text->first_element->next,  NULL      );

        tag = tag->next;
        CU_ASSERT_EQUAL(tag->flags,                                             TAG_CLEAN   );
        CU_ASSERT_EQUAL(tag->text,                                              NULL        );
        CU_ASSERT_EQUAL(tag->namespace,                                         NULL        );
        CU_ASSERT_STRING_EQUAL(tag->name,                                       "h1"        );
        CU_ASSERT_FALSE(tag->attributes->flags & ATTR_INVISIBLE                             );
        CU_ASSERT_EQUAL(tag->attributes->namespace,                             NULL        );
        CU_ASSERT_STRING_EQUAL(tag->attributes->name,                           "class"     );
        CU_ASSERT_STRING_EQUAL(tag->attributes->expression->data->value.string, "with_style");
        CU_ASSERT_EQUAL(tag->attributes->expression->opcode,                    EXPR_EVAL   );
        CU_ASSERT_EQUAL(tag->attributes->expression->data->flags,               DATA_STRING );

        sibling_tag = tag->next;
        tag = tag->first_child;
        CU_ASSERT_EQUAL(tag->flags,       TAG_TEXT);
        CU_ASSERT_EQUAL(tag->first_child, NULL    );
        CU_ASSERT_EQUAL(tag->namespace,   NULL    );
        CU_ASSERT_EQUAL(tag->name,        NULL    );
        CU_ASSERT_EQUAL(tag->next,        NULL    );

        string = CryText_to_string(tag->text);
        CU_ASSERT_STRING_EQUAL(string, "Cryml is almost HTML compatible!");
        free(string);

        CU_ASSERT_EQUAL(sibling_tag->flags,                      TAG_TEXT  );
        CU_ASSERT_EQUAL(sibling_tag->first_child,                NULL      );
        CU_ASSERT_EQUAL(sibling_tag->namespace,                  NULL      );
        CU_ASSERT_EQUAL(sibling_tag->name,                       NULL      );
        CU_ASSERT_EQUAL(sibling_tag->text->first_element->flags, TEXT_BLANK);
        CU_ASSERT_EQUAL(sibling_tag->text->first_element->next,  NULL      );

        tag = sibling_tag->next;
        CU_ASSERT_EQUAL(tag->flags,       TAG_CLEAN);
        CU_ASSERT_EQUAL(tag->text,        NULL     );
        CU_ASSERT_EQUAL(tag->attributes,  NULL     );
        CU_ASSERT_EQUAL(tag->namespace,   NULL     );
        CU_ASSERT_STRING_EQUAL(tag->name, "p"      );

        sibling_tag = tag->next;
        tag = tag->first_child;
        CU_ASSERT_EQUAL(tag->flags,       TAG_TEXT);
        CU_ASSERT_EQUAL(tag->first_child, NULL    );
        CU_ASSERT_EQUAL(tag->namespace,   NULL    );
        CU_ASSERT_EQUAL(tag->name,        NULL    );
        CU_ASSERT_EQUAL(tag->next,        NULL    );

        string = CryText_to_string(tag->text);
        CU_ASSERT_STRING_EQUAL(string, "Doesn't have any %${{}}");
        free(string);

        CU_ASSERT_EQUAL(sibling_tag->flags,                      TAG_TEXT  );
        CU_ASSERT_EQUAL(sibling_tag->first_child,                NULL      );
        CU_ASSERT_EQUAL(sibling_tag->namespace,                  NULL      );
        CU_ASSERT_EQUAL(sibling_tag->name,                       NULL      );
        CU_ASSERT_EQUAL(sibling_tag->text->first_element->flags, TEXT_BLANK);
        CU_ASSERT_EQUAL(sibling_tag->text->first_element->next,  NULL      );

        tag = sibling_tag->next;
        CU_ASSERT_EQUAL(tag->flags,       TAG_SINGLETON);
        CU_ASSERT_EQUAL(tag->text,        NULL         );
        CU_ASSERT_EQUAL(tag->attributes,  NULL         );
        CU_ASSERT_EQUAL(tag->first_child, NULL         );
        CU_ASSERT_EQUAL(tag->namespace,   NULL         );
        CU_ASSERT_STRING_EQUAL(tag->name, "br"         );

        tag = tag->next;
        CU_ASSERT_EQUAL(tag->flags,                      TAG_TEXT  );
        CU_ASSERT_EQUAL(tag->first_child,                NULL      );
        CU_ASSERT_EQUAL(tag->namespace,                  NULL      );
        CU_ASSERT_EQUAL(tag->name,                       NULL      );
        CU_ASSERT_EQUAL(tag->text->first_element->flags, TEXT_BLANK);
        CU_ASSERT_EQUAL(tag->text->first_element->next,  NULL      );

        tag = tag->next;
        CU_ASSERT_EQUAL(tag->flags,       TAG_CLEAN);
        CU_ASSERT_EQUAL(tag->text,        NULL     );
        CU_ASSERT_EQUAL(tag->attributes,  NULL     );
        CU_ASSERT_EQUAL(tag->namespace,   NULL     );
        CU_ASSERT_STRING_EQUAL(tag->name, "p"      );

        CU_ASSERT_EQUAL(tag->first_child->flags,       TAG_TEXT);
        CU_ASSERT_EQUAL(tag->first_child->first_child, NULL    );
        CU_ASSERT_EQUAL(tag->first_child->namespace,   NULL    );
        CU_ASSERT_EQUAL(tag->first_child->name,        NULL    );
        CU_ASSERT_EQUAL(tag->first_child->next,        NULL    );

        string = CryText_to_string(tag->first_child->text);
        CU_ASSERT_STRING_EQUAL(string, "mixed between tag's content!");
        free(string);

        tag = tag->next;
        CU_ASSERT_EQUAL(tag->flags,                      TAG_TEXT  );
        CU_ASSERT_EQUAL(tag->next,                       NULL      );
        CU_ASSERT_EQUAL(tag->first_child,                NULL      );
        CU_ASSERT_EQUAL(tag->namespace,                  NULL      );
        CU_ASSERT_EQUAL(tag->name,                       NULL      );
        CU_ASSERT_EQUAL(tag->text->first_element->flags, TEXT_BLANK);
        CU_ASSERT_EQUAL(tag->text->first_element->next,  NULL      );

        CryEnv_cleanup(env);
    );

    free_parser_environment(env);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_simple_attributes(void){
    CryTag       tag;
    CryEnv      *env;
    CryAttr      attr;
    long double  precision = 0.00000000000001;

    env = init_parser_environment("rb");

    EXECUTE_TWICE(
        Parser_parse(env, "test/examples/simple_attributes.cryml", 0);

        tag = env->root_tag;
        CU_ASSERT_EQUAL(tag->next, NULL);

        tag = tag->first_child;
        attr = tag->attributes;
        CU_ASSERT_EQUAL(tag->flags,                            TAG_SINGLETON );
        CU_ASSERT_EQUAL(tag->text,                             NULL          );
        CU_ASSERT_EQUAL(tag->namespace,                        NULL          );
        CU_ASSERT_STRING_EQUAL(tag->name,                      "integer"     );
        CU_ASSERT_EQUAL(attr->namespace,                       NULL          );
        CU_ASSERT_STRING_EQUAL(attr->name,                     "number"      );
        CU_ASSERT_EQUAL(attr->expression->opcode,              EXPR_EVAL     );
        CU_ASSERT_EQUAL(attr->expression->data->value.integer, 1234          );
        CU_ASSERT_EQUAL(attr->expression->data->flags,         DATA_INTEGER  );

        tag = tag->next;
        CU_ASSERT_EQUAL(tag->flags,                      TAG_TEXT  );
        CU_ASSERT_EQUAL(tag->first_child,                NULL      );
        CU_ASSERT_EQUAL(tag->namespace,                  NULL      );
        CU_ASSERT_EQUAL(tag->name,                       NULL      );
        CU_ASSERT_EQUAL(tag->text->first_element->flags, TEXT_BLANK);
        CU_ASSERT_EQUAL(tag->text->first_element->next,  NULL      );

        tag = tag->next;
        attr = tag->attributes;
        CU_ASSERT_EQUAL(tag->flags,                                   TAG_SINGLETON  );
        CU_ASSERT_EQUAL(tag->text,                                    NULL           );
        CU_ASSERT_STRING_EQUAL(tag->namespace,                        "attr"         );
        CU_ASSERT_STRING_EQUAL(tag->name,                             "double"       );
        CU_ASSERT_EQUAL(attr->namespace,                              NULL           );
        CU_ASSERT_STRING_EQUAL(attr->name,                            "number"       );
        CU_ASSERT_EQUAL(attr->expression->opcode,                     EXPR_EVAL      );
        CU_ASSERT_DOUBLE_EQUAL(attr->expression->data->value.cdouble, 3.14, precision);
        CU_ASSERT_EQUAL(attr->expression->data->flags,                DATA_DOUBLE    );

        tag = tag->next;
        CU_ASSERT_EQUAL(tag->flags,                      TAG_TEXT  );
        CU_ASSERT_EQUAL(tag->first_child,                NULL      );
        CU_ASSERT_EQUAL(tag->namespace,                  NULL      );
        CU_ASSERT_EQUAL(tag->name,                       NULL      );
        CU_ASSERT_EQUAL(tag->text->first_element->flags, TEXT_BLANK);
        CU_ASSERT_EQUAL(tag->text->first_element->next,  NULL      );

        tag = tag->next;
        attr = tag->attributes;
        CU_ASSERT_EQUAL(tag->flags,                                   TAG_SINGLETON          );
        CU_ASSERT_EQUAL(tag->text,                                    NULL                   );
        CU_ASSERT_STRING_EQUAL(tag->namespace,                        "attr"                 );
        CU_ASSERT_STRING_EQUAL(tag->name,                             "string"               );
        CU_ASSERT_STRING_EQUAL(attr->namespace,                       "single"               );
        CU_ASSERT_STRING_EQUAL(attr->name,                            "quoted"               );
        CU_ASSERT_EQUAL(attr->expression->opcode,                     EXPR_EVAL              );
        CU_ASSERT_STRING_EQUAL(attr->expression->data->value.string,  "single quoted string!");
        CU_ASSERT_EQUAL(attr->expression->data->flags,                DATA_STRING            );

        tag = tag->next;
        CU_ASSERT_EQUAL(tag->flags,                      TAG_TEXT  );
        CU_ASSERT_EQUAL(tag->first_child,                NULL      );
        CU_ASSERT_EQUAL(tag->namespace,                  NULL      );
        CU_ASSERT_EQUAL(tag->name,                       NULL      );
        CU_ASSERT_EQUAL(tag->text->first_element->flags, TEXT_BLANK);
        CU_ASSERT_EQUAL(tag->text->first_element->next,  NULL      );

        tag = tag->next;
        attr = tag->attributes;
        CU_ASSERT_EQUAL(tag->flags,                                   TAG_SINGLETON                );
        CU_ASSERT_EQUAL(tag->text,                                    NULL                         );
        CU_ASSERT_STRING_EQUAL(tag->namespace,                        "escaped"                    );
        CU_ASSERT_STRING_EQUAL(tag->name,                             "string"                     );
        CU_ASSERT_STRING_EQUAL(attr->namespace,                       "single"                     );
        CU_ASSERT_STRING_EQUAL(attr->name,                            "quoted"                     );
        CU_ASSERT_EQUAL(attr->expression->opcode,                     EXPR_EVAL                    );
        CU_ASSERT_STRING_EQUAL(attr->expression->data->value.string,  "sin\"gle 'quoted' stri\"ng!");
        CU_ASSERT_EQUAL(attr->expression->data->flags,                DATA_STRING                  );

        tag = tag->next;
        CU_ASSERT_EQUAL(tag->flags,                      TAG_TEXT  );
        CU_ASSERT_EQUAL(tag->first_child,                NULL      );
        CU_ASSERT_EQUAL(tag->namespace,                  NULL      );
        CU_ASSERT_EQUAL(tag->name,                       NULL      );
        CU_ASSERT_EQUAL(tag->text->first_element->flags, TEXT_BLANK);
        CU_ASSERT_EQUAL(tag->text->first_element->next,  NULL      );

        tag = tag->next;
        attr = tag->attributes;
        CU_ASSERT_EQUAL(tag->flags,                                   TAG_SINGLETON          );
        CU_ASSERT_EQUAL(tag->text,                                    NULL                   );
        CU_ASSERT_STRING_EQUAL(tag->namespace,                        "attr"                 );
        CU_ASSERT_STRING_EQUAL(tag->name,                             "string"               );
        CU_ASSERT_STRING_EQUAL(attr->namespace,                       "double"               );
        CU_ASSERT_STRING_EQUAL(attr->name,                            "quoted"               );
        CU_ASSERT_EQUAL(attr->expression->opcode,                     EXPR_EVAL              );
        CU_ASSERT_STRING_EQUAL(attr->expression->data->value.string,  "double quoted string!");
        CU_ASSERT_EQUAL(attr->expression->data->flags,                DATA_STRING            );

        tag = tag->next;
        CU_ASSERT_EQUAL(tag->flags,                      TAG_TEXT  );
        CU_ASSERT_EQUAL(tag->first_child,                NULL      );
        CU_ASSERT_EQUAL(tag->namespace,                  NULL      );
        CU_ASSERT_EQUAL(tag->name,                       NULL      );
        CU_ASSERT_EQUAL(tag->text->first_element->flags, TEXT_BLANK);
        CU_ASSERT_EQUAL(tag->text->first_element->next,  NULL      );

        tag = tag->next;
        attr = tag->attributes;
        CU_ASSERT_EQUAL(tag->flags,                                   TAG_SINGLETON                );
        CU_ASSERT_EQUAL(tag->text,                                    NULL                         );
        CU_ASSERT_STRING_EQUAL(tag->namespace,                        "escaped"                    );
        CU_ASSERT_STRING_EQUAL(tag->name,                             "string"                     );
        CU_ASSERT_STRING_EQUAL(attr->namespace,                       "double"                     );
        CU_ASSERT_STRING_EQUAL(attr->name,                            "quoted"                     );
        CU_ASSERT_EQUAL(attr->expression->opcode,                     EXPR_EVAL                    );
        CU_ASSERT_STRING_EQUAL(attr->expression->data->value.string,  "dou\"ble 'quoted' stri\"ng!");
        CU_ASSERT_EQUAL(attr->expression->data->flags,                DATA_STRING                  );

        tag = tag->next;
        CU_ASSERT_EQUAL(tag->flags,                      TAG_TEXT  );
        CU_ASSERT_EQUAL(tag->first_child,                NULL      );
        CU_ASSERT_EQUAL(tag->namespace,                  NULL      );
        CU_ASSERT_EQUAL(tag->name,                       NULL      );
        CU_ASSERT_EQUAL(tag->text->first_element->flags, TEXT_BLANK);
        CU_ASSERT_EQUAL(tag->text->first_element->next,  NULL      );

        tag = tag->next;
        attr = tag->attributes;
        CU_ASSERT_EQUAL(tag->flags,                                   TAG_SINGLETON   );
        CU_ASSERT_EQUAL(tag->text,                                    NULL            );
        CU_ASSERT_EQUAL(tag->namespace,                               NULL            );
        CU_ASSERT_STRING_EQUAL(tag->name,                             "attribute"     );
        CU_ASSERT_STRING_EQUAL(attr->namespace,                       "attr"          );
        CU_ASSERT_STRING_EQUAL(attr->name,                            "unnamespaced"  );
        CU_ASSERT_EQUAL(attr->expression->opcode,                     EXPR_ATTR       );
        CU_ASSERT_STRING_EQUAL(attr->expression->data->value.string,  "attribute_name");
        CU_ASSERT_EQUAL(attr->expression->data->flags,                DATA_STRING     );

        tag = tag->next;
        CU_ASSERT_EQUAL(tag->flags,                      TAG_TEXT  );
        CU_ASSERT_EQUAL(tag->first_child,                NULL      );
        CU_ASSERT_EQUAL(tag->namespace,                  NULL      );
        CU_ASSERT_EQUAL(tag->name,                       NULL      );
        CU_ASSERT_EQUAL(tag->text->first_element->flags, TEXT_BLANK);
        CU_ASSERT_EQUAL(tag->text->first_element->next,  NULL      );

        tag = tag->next;
        attr = tag->attributes;
        CU_ASSERT_EQUAL(tag->flags,                                   TAG_SINGLETON   );
        CU_ASSERT_EQUAL(tag->text,                                    NULL            );
        CU_ASSERT_EQUAL(tag->namespace,                               NULL            );
        CU_ASSERT_STRING_EQUAL(tag->name,                             "attribute"     );
        CU_ASSERT_STRING_EQUAL(attr->namespace,                       "attr"          );
        CU_ASSERT_STRING_EQUAL(attr->name,                            "namespaced"    );
        CU_ASSERT_EQUAL(attr->expression->opcode,                     EXPR_ATTR       );
        CU_ASSERT_STRING_EQUAL(attr->expression->data->value.string,  "attribute:name");
        CU_ASSERT_EQUAL(attr->expression->data->flags,                DATA_STRING     );

        tag = tag->next;
        CU_ASSERT_EQUAL(tag->flags,                      TAG_TEXT  );
        CU_ASSERT_EQUAL(tag->first_child,                NULL      );
        CU_ASSERT_EQUAL(tag->namespace,                  NULL      );
        CU_ASSERT_EQUAL(tag->name,                       NULL      );
        CU_ASSERT_EQUAL(tag->text->first_element->flags, TEXT_BLANK);
        CU_ASSERT_EQUAL(tag->text->first_element->next,  NULL      );

        tag = tag->next;
        attr = tag->attributes;
        CU_ASSERT_EQUAL(tag->flags,                                   TAG_SINGLETON     );
        CU_ASSERT_EQUAL(tag->text,                                    NULL              );
        CU_ASSERT_STRING_EQUAL(tag->namespace,                        "attr"            );
        CU_ASSERT_STRING_EQUAL(tag->name,                             "scope"           );
        CU_ASSERT_STRING_EQUAL(attr->namespace,                       "empty"           );
        CU_ASSERT_STRING_EQUAL(attr->name,                            "scope"           );
        CU_ASSERT_NOT_EQUAL(attr->expression,                         NULL              );
        CU_ASSERT_EQUAL(attr->expression->opcode,                     EXPR_SCOPED_CALL  );
        CU_ASSERT_STRING_EQUAL(attr->expression->data->value.string,  ""                );
        CU_ASSERT_EQUAL(attr->expression->data->flags,                DATA_STRING       );
        CU_ASSERT_EQUAL(attr->expression->scope,                      env->scopes       );

        tag = tag->next;
        CU_ASSERT_EQUAL(tag->flags,                      TAG_TEXT   );
        CU_ASSERT_EQUAL(tag->first_child,                NULL       );
        CU_ASSERT_EQUAL(tag->namespace,                  NULL       );
        CU_ASSERT_EQUAL(tag->name,                       NULL       );
        CU_ASSERT_EQUAL(tag->text->first_element->flags, TEXT_BLANK );
        CU_ASSERT_EQUAL(tag->text->first_element->next,  NULL       );
        CU_ASSERT_STRING_EQUAL(tag->text->first_element->text,  "\n");

        tag = tag->next;
        attr = tag->attributes;
        CU_ASSERT_EQUAL(tag->flags,                                   TAG_SINGLETON     );
        CU_ASSERT_EQUAL(tag->text,                                    NULL              );
        CU_ASSERT_STRING_EQUAL(tag->namespace,                        "attr"            );
        CU_ASSERT_STRING_EQUAL(tag->name,                             "scope"           );
        CU_ASSERT_STRING_EQUAL(attr->namespace,                       "not_empty"       );
        CU_ASSERT_STRING_EQUAL(attr->name,                            "scope"           );
        CU_ASSERT_NOT_EQUAL(attr->expression,                         NULL              );
        CU_ASSERT_EQUAL(attr->expression->opcode,                     EXPR_SCOPED_CALL  );
        CU_ASSERT_STRING_EQUAL(attr->expression->data->value.string,  " not empty ok?! ");
        CU_ASSERT_EQUAL(attr->expression->data->flags,                DATA_STRING       );
        CU_ASSERT_EQUAL(attr->expression->scope,                      env->scopes       );

        tag = tag->next;
        CU_ASSERT_EQUAL(tag->flags,                      TAG_TEXT  );
        CU_ASSERT_EQUAL(tag->first_child,                NULL      );
        CU_ASSERT_EQUAL(tag->namespace,                  NULL      );
        CU_ASSERT_EQUAL(tag->name,                       NULL      );
        CU_ASSERT_EQUAL(tag->text->first_element->flags, TEXT_BLANK);
        CU_ASSERT_EQUAL(tag->text->first_element->next,  NULL      );

        tag = tag->next;
        attr = tag->attributes;
        CU_ASSERT_EQUAL(tag->flags,                                   TAG_SINGLETON                                 );
        CU_ASSERT_EQUAL(tag->text,                                    NULL                                          );
        CU_ASSERT_STRING_EQUAL(tag->namespace,                        "attr"                                        );
        CU_ASSERT_STRING_EQUAL(tag->name,                             "scope"                                       );
        CU_ASSERT_STRING_EQUAL(attr->namespace,                       "balanced"                                    );
        CU_ASSERT_STRING_EQUAL(attr->name,                            "scope"                                       );
        CU_ASSERT_NOT_EQUAL(attr->expression,                         NULL                                          );
        CU_ASSERT_EQUAL(attr->expression->opcode,                     EXPR_SCOPED_CALL                              );
        CU_ASSERT_STRING_EQUAL(attr->expression->data->value.string,  " balanced, { so i dont need to be escaped!} ");
        CU_ASSERT_EQUAL(attr->expression->data->flags,                DATA_STRING                                   );
        CU_ASSERT_EQUAL(attr->expression->scope,                      env->scopes                                   );

        tag = tag->next;
        CU_ASSERT_EQUAL(tag->flags,                      TAG_TEXT  );
        CU_ASSERT_EQUAL(tag->first_child,                NULL      );
        CU_ASSERT_EQUAL(tag->namespace,                  NULL      );
        CU_ASSERT_EQUAL(tag->name,                       NULL      );
        CU_ASSERT_EQUAL(tag->text->first_element->flags, TEXT_BLANK);
        CU_ASSERT_EQUAL(tag->text->first_element->next,  NULL      );

        tag = tag->next;
        attr = tag->attributes;
        CU_ASSERT_EQUAL(tag->flags,                                   TAG_SINGLETON                                );
        CU_ASSERT_EQUAL(tag->text,                                    NULL                                         );
        CU_ASSERT_STRING_EQUAL(tag->namespace,                        "attr"                                       );
        CU_ASSERT_STRING_EQUAL(tag->name,                             "scope"                                      );
        CU_ASSERT_STRING_EQUAL(attr->namespace,                       "escaped"                                    );
        CU_ASSERT_STRING_EQUAL(attr->name,                            "scope"                                      );
        CU_ASSERT_NOT_EQUAL(attr->expression,                         NULL                                         );
        CU_ASSERT_EQUAL(attr->expression->opcode,                     EXPR_SCOPED_CALL                             );
        CU_ASSERT_STRING_EQUAL(attr->expression->data->value.string,  " escaped, so i dont { need to be balanced! ");
        CU_ASSERT_EQUAL(attr->expression->data->flags,                DATA_STRING                                  );
        CU_ASSERT_EQUAL(attr->expression->scope,                      env->scopes                                  );

        tag = tag->next;
        CU_ASSERT_EQUAL(tag->flags,                      TAG_TEXT  );
        CU_ASSERT_EQUAL(tag->first_child,                NULL      );
        CU_ASSERT_EQUAL(tag->namespace,                  NULL      );
        CU_ASSERT_EQUAL(tag->name,                       NULL      );
        CU_ASSERT_EQUAL(tag->text->first_element->flags, TEXT_BLANK);
        CU_ASSERT_EQUAL(tag->text->first_element->next,  NULL      );

        tag = tag->next;
        attr = tag->attributes;
        CU_ASSERT_EQUAL(tag->flags,                                   TAG_SINGLETON                                                );
        CU_ASSERT_EQUAL(tag->text,                                    NULL                                                         );
        CU_ASSERT_STRING_EQUAL(tag->namespace,                        "attr"                                                       );
        CU_ASSERT_STRING_EQUAL(tag->name,                             "scope"                                                      );
        CU_ASSERT_STRING_EQUAL(attr->namespace,                       "balanced_and_escaped"                                       );
        CU_ASSERT_STRING_EQUAL(attr->name,                            "scope"                                                      );
        CU_ASSERT_NOT_EQUAL(attr->expression,                         NULL                                                         );
        CU_ASSERT_EQUAL(attr->expression->opcode,                     EXPR_SCOPED_CALL                                             );
        CU_ASSERT_STRING_EQUAL(attr->expression->data->value.string,  " escaped, {so i \{ dont need to be } balanced every time!! ");
        CU_ASSERT_EQUAL(attr->expression->data->flags,                DATA_STRING                                                  );
        CU_ASSERT_EQUAL(attr->expression->scope,                      env->scopes                                                  );

        tag = tag->next;
        CU_ASSERT_EQUAL(tag->flags,                      TAG_TEXT  );
        CU_ASSERT_EQUAL(tag->first_child,                NULL      );
        CU_ASSERT_EQUAL(tag->namespace,                  NULL      );
        CU_ASSERT_EQUAL(tag->name,                       NULL      );
        CU_ASSERT_EQUAL(tag->text->first_element->flags, TEXT_BLANK);
        CU_ASSERT_EQUAL(tag->text->first_element->next,  NULL      );

        tag = tag->next;
        attr = tag->attributes;
        CU_ASSERT_EQUAL(tag->flags,                            TAG_SINGLETON);
        CU_ASSERT_EQUAL(tag->text,                             NULL         );
        CU_ASSERT_STRING_EQUAL(tag->namespace,                 "bool"       );
        CU_ASSERT_STRING_EQUAL(tag->name,                      "true"       );
        CU_ASSERT_EQUAL(attr->namespace,                       NULL         );
        CU_ASSERT_STRING_EQUAL(attr->name,                     "value"      );
        CU_ASSERT_EQUAL(attr->expression->opcode,              EXPR_EVAL    );
        CU_ASSERT_EQUAL(attr->expression->data->value.boolean, TRUE         );
        CU_ASSERT_EQUAL(attr->expression->data->flags,         DATA_BOOLEAN );

        tag = tag->next;
        CU_ASSERT_EQUAL(tag->flags,                      TAG_TEXT  );
        CU_ASSERT_EQUAL(tag->first_child,                NULL      );
        CU_ASSERT_EQUAL(tag->namespace,                  NULL      );
        CU_ASSERT_EQUAL(tag->name,                       NULL      );
        CU_ASSERT_EQUAL(tag->text->first_element->flags, TEXT_BLANK);
        CU_ASSERT_EQUAL(tag->text->first_element->next,  NULL      );

        tag = tag->next;
        attr = tag->attributes;
        CU_ASSERT_EQUAL(tag->flags,                            TAG_SINGLETON);
        CU_ASSERT_EQUAL(tag->text,                             NULL         );
        CU_ASSERT_STRING_EQUAL(tag->namespace,                 "bool"       );
        CU_ASSERT_STRING_EQUAL(tag->name,                      "false"      );
        CU_ASSERT_EQUAL(attr->namespace,                       NULL         );
        CU_ASSERT_STRING_EQUAL(attr->name,                     "value"      );
        CU_ASSERT_EQUAL(attr->expression->opcode,              EXPR_EVAL    );
        CU_ASSERT_EQUAL(attr->expression->data->value.boolean, FALSE        );
        CU_ASSERT_EQUAL(attr->expression->data->flags,         DATA_BOOLEAN );

        tag = tag->next;
        CU_ASSERT_EQUAL(tag->flags,                      TAG_TEXT  );
        CU_ASSERT_EQUAL(tag->first_child,                NULL      );
        CU_ASSERT_EQUAL(tag->namespace,                  NULL      );
        CU_ASSERT_EQUAL(tag->name,                       NULL      );
        CU_ASSERT_EQUAL(tag->text->first_element->flags, TEXT_BLANK);
        CU_ASSERT_EQUAL(tag->text->first_element->next,  NULL      );

        tag = tag->next;
        attr = tag->attributes;
        CU_ASSERT_EQUAL(tag->flags,                    TAG_SINGLETON);
        CU_ASSERT_EQUAL(tag->text,                     NULL         );
        CU_ASSERT_EQUAL(tag->next,                     NULL         );
        CU_ASSERT_STRING_EQUAL(tag->namespace,         "nUlL"       );
        CU_ASSERT_STRING_EQUAL(tag->name,              "null_tag"   );
        CU_ASSERT_EQUAL(attr->namespace,               NULL         );
        CU_ASSERT_STRING_EQUAL(attr->name,             "value"      );
        CU_ASSERT_EQUAL(attr->expression->opcode,      EXPR_EVAL    );
        CU_ASSERT_EQUAL(attr->expression->data->flags, DATA_NULL    );

        CryEnv_cleanup(env);
    );

    free_parser_environment(env);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_unary_simple_attributes(void){
    CryEnv      *env;
    CryAttr      attr;
    long double  precision = 0.00000000000001;

    env = init_parser_environment("rb");

    // EXECUTE_TWICE(
        Parser_parse(env, "test/examples/unary_simple_attributes.cryml", 0);

        CU_ASSERT_EQUAL(env->root_tag->next,                     NULL);
        CU_ASSERT_EQUAL(env->root_tag->first_child->next,        NULL);
        CU_ASSERT_EQUAL(env->root_tag->first_child->first_child, NULL);

        attr = env->root_tag->first_child->attributes;
        CU_ASSERT_FALSE(attr->flags & ATTR_INVISIBLE);
        CU_ASSERT_STRING_EQUAL(attr->namespace,                "positive"    );
        CU_ASSERT_STRING_EQUAL(attr->name,                     "integer"     );
        CU_ASSERT_EQUAL(attr->expression->opcode,              EXPR_EVAL     );
        CU_ASSERT_EQUAL(attr->expression->data->value.integer, 123           );
        CU_ASSERT_EQUAL(attr->expression->data->flags,         DATA_INTEGER  );

        attr  = attr->next;
        CU_ASSERT_FALSE(attr->flags & ATTR_INVISIBLE);
        CU_ASSERT_STRING_EQUAL(attr->namespace,                                        "negative"    );
        CU_ASSERT_STRING_EQUAL(attr->name,                                             "integer"     );
        CU_ASSERT_EQUAL(attr->expression->opcode,                                      EXPR_MINUS    );
        CU_ASSERT_EQUAL(attr->expression->data->value.expression->opcode,              EXPR_EVAL     );
        CU_ASSERT_EQUAL(attr->expression->data->value.expression->data->flags,         DATA_INTEGER  );
        CU_ASSERT_EQUAL(attr->expression->data->value.expression->data->value.integer, 321           );

        attr  = attr->next;
        CU_ASSERT_FALSE(attr->flags & ATTR_INVISIBLE);
        CU_ASSERT_STRING_EQUAL(attr->namespace,                       "positive"     );
        CU_ASSERT_STRING_EQUAL(attr->name,                            "double"       );
        CU_ASSERT_EQUAL(attr->expression->opcode,                     EXPR_EVAL      );
        CU_ASSERT_DOUBLE_EQUAL(attr->expression->data->value.cdouble, 3.14, precision);
        CU_ASSERT_EQUAL(attr->expression->data->flags,                DATA_DOUBLE    );

        attr  = attr->next;
        CU_ASSERT_FALSE(attr->flags & ATTR_INVISIBLE);
        CU_ASSERT_STRING_EQUAL(attr->namespace,                                               "negative"     );
        CU_ASSERT_STRING_EQUAL(attr->name,                                                    "double"       );
        CU_ASSERT_EQUAL(attr->expression->opcode,                                             EXPR_MINUS     );
        CU_ASSERT_EQUAL(attr->expression->data->value.expression->opcode,                     EXPR_EVAL      );
        CU_ASSERT_EQUAL(attr->expression->data->value.expression->data->flags,                DATA_DOUBLE    );
        CU_ASSERT_DOUBLE_EQUAL(attr->expression->data->value.expression->data->value.cdouble, 3.14, precision);

        attr = attr->next;
        CU_ASSERT_FALSE(attr->flags & ATTR_INVISIBLE);
        CU_ASSERT_STRING_EQUAL(attr->namespace,                       "positive" );
        CU_ASSERT_STRING_EQUAL(attr->name,                            "attr"     );
        CU_ASSERT_EQUAL(attr->expression->opcode,                     EXPR_ATTR  );
        CU_ASSERT_STRING_EQUAL(attr->expression->data->value.string,  "attrib"   );
        CU_ASSERT_EQUAL(attr->expression->data->flags,                DATA_STRING);

        attr = attr->next;
        CU_ASSERT_FALSE(attr->flags & ATTR_INVISIBLE);
        CU_ASSERT_STRING_EQUAL(attr->namespace,                                              "negative" );
        CU_ASSERT_STRING_EQUAL(attr->name,                                                   "attr"     );
        CU_ASSERT_EQUAL(attr->expression->opcode,                                            EXPR_MINUS );
        CU_ASSERT_EQUAL(attr->expression->data->value.expression->opcode,                    EXPR_ATTR  );
        CU_ASSERT_EQUAL(attr->expression->data->value.expression->data->flags,               DATA_STRING);
        CU_ASSERT_STRING_EQUAL(attr->expression->data->value.expression->data->value.string, "ute"      );

        // TODO: Complete unary expression tests!

        CryEnv_cleanup(env);
    // );

    free_parser_environment(env);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_multiple_attributes(void){
    CryEnv      *env;
    CryAttr      attr;
    long double  precision = 0.00000000000001;

    env = init_parser_environment("rb");

    EXECUTE_TWICE(
        Parser_parse(env, "test/examples/multiple_attributes.cryml", 0);

        CU_ASSERT_EQUAL(env->root_tag->next,                     NULL);
        CU_ASSERT_EQUAL(env->root_tag->first_child->next,        NULL);
        CU_ASSERT_EQUAL(env->root_tag->first_child->first_child, NULL);

        attr = env->root_tag->first_child->attributes;
        CU_ASSERT_FALSE(attr->flags & ATTR_INVISIBLE);
        CU_ASSERT_EQUAL(attr->namespace,                       NULL          );
        CU_ASSERT_STRING_EQUAL(attr->name,                     "integer"     );
        CU_ASSERT_EQUAL(attr->expression->opcode,              EXPR_EVAL     );
        CU_ASSERT_EQUAL(attr->expression->data->value.integer, 1234          );
        CU_ASSERT_EQUAL(attr->expression->data->flags,         DATA_INTEGER  );

        attr = attr->next;
        CU_ASSERT(attr->flags & ATTR_INVISIBLE);
        CU_ASSERT_EQUAL(attr->namespace,                              NULL           );
        CU_ASSERT_STRING_EQUAL(attr->name,                            "double"       );
        CU_ASSERT_EQUAL(attr->expression->opcode,                     EXPR_EVAL      );
        CU_ASSERT_DOUBLE_EQUAL(attr->expression->data->value.cdouble, 3.14, precision);
        CU_ASSERT_EQUAL(attr->expression->data->flags,                DATA_DOUBLE    );

        attr = attr->next;
        CU_ASSERT_FALSE(attr->flags & ATTR_INVISIBLE);
        CU_ASSERT_STRING_EQUAL(attr->namespace,                       "single"               );
        CU_ASSERT_STRING_EQUAL(attr->name,                            "quoted"               );
        CU_ASSERT_EQUAL(attr->expression->opcode,                     EXPR_EVAL              );
        CU_ASSERT_STRING_EQUAL(attr->expression->data->value.string,  "single quoted string!");
        CU_ASSERT_EQUAL(attr->expression->data->flags,                DATA_STRING            );

        attr = attr->next;
        CU_ASSERT(attr->flags & ATTR_INVISIBLE);
        CU_ASSERT_STRING_EQUAL(attr->namespace,                       "single"                     );
        CU_ASSERT_STRING_EQUAL(attr->name,                            "escaped"                    );
        CU_ASSERT_EQUAL(attr->expression->opcode,                     EXPR_EVAL                    );
        CU_ASSERT_STRING_EQUAL(attr->expression->data->value.string,  "sin\"gle 'quoted' stri\"ng!");
        CU_ASSERT_EQUAL(attr->expression->data->flags,                DATA_STRING                  );

        attr = attr->next;
        CU_ASSERT_FALSE(attr->flags & ATTR_INVISIBLE);
        CU_ASSERT_STRING_EQUAL(attr->namespace,                       "double"               );
        CU_ASSERT_STRING_EQUAL(attr->name,                            "quoted"               );
        CU_ASSERT_EQUAL(attr->expression->opcode,                     EXPR_EVAL              );
        CU_ASSERT_STRING_EQUAL(attr->expression->data->value.string,  "double quoted string!");
        CU_ASSERT_EQUAL(attr->expression->data->flags,                DATA_STRING            );

        attr = attr->next;
        CU_ASSERT(attr->flags & ATTR_INVISIBLE);
        CU_ASSERT_STRING_EQUAL(attr->namespace,                       "double"                     );
        CU_ASSERT_STRING_EQUAL(attr->name,                            "escaped"                    );
        CU_ASSERT_EQUAL(attr->expression->opcode,                     EXPR_EVAL                    );
        CU_ASSERT_STRING_EQUAL(attr->expression->data->value.string,  "dou\"ble 'quoted' stri\"ng!");
        CU_ASSERT_EQUAL(attr->expression->data->flags,                DATA_STRING                  );

        attr = attr->next;
        CU_ASSERT_FALSE(attr->flags & ATTR_INVISIBLE);
        CU_ASSERT_STRING_EQUAL(attr->namespace,                       "attr"          );
        CU_ASSERT_STRING_EQUAL(attr->name,                            "unnamespaced"  );
        CU_ASSERT_EQUAL(attr->expression->opcode,                     EXPR_ATTR       );
        CU_ASSERT_STRING_EQUAL(attr->expression->data->value.string,  "attribute_name");
        CU_ASSERT_EQUAL(attr->expression->data->flags,                DATA_STRING     );

        attr = attr->next;
        CU_ASSERT(attr->flags & ATTR_INVISIBLE);
        CU_ASSERT_STRING_EQUAL(attr->namespace,                       "attr"          );
        CU_ASSERT_STRING_EQUAL(attr->name,                            "namespaced"    );
        CU_ASSERT_EQUAL(attr->expression->opcode,                     EXPR_ATTR       );
        CU_ASSERT_STRING_EQUAL(attr->expression->data->value.string,  "attribute:name");
        CU_ASSERT_EQUAL(attr->expression->data->flags,                DATA_STRING     );

        attr = attr->next;
        CU_ASSERT_FALSE(attr->flags & ATTR_INVISIBLE);
        CU_ASSERT_STRING_EQUAL(attr->namespace,                       "empty"         );
        CU_ASSERT_STRING_EQUAL(attr->name,                            "scope"         );
        CU_ASSERT_NOT_EQUAL(attr->expression,                         NULL            );
        CU_ASSERT_EQUAL(attr->expression->opcode,                     EXPR_SCOPED_CALL);
        CU_ASSERT_STRING_EQUAL(attr->expression->data->value.string,  ""              );
        CU_ASSERT_EQUAL(attr->expression->data->flags,                DATA_STRING     );
        CU_ASSERT_EQUAL(attr->expression->scope,                      env->scopes     );

        attr = attr->next;
        CU_ASSERT(attr->flags & ATTR_INVISIBLE);
        CU_ASSERT_STRING_EQUAL(attr->namespace,                       "not_empty"       );
        CU_ASSERT_STRING_EQUAL(attr->name,                            "scope"           );
        CU_ASSERT_NOT_EQUAL(attr->expression,                         NULL              );
        CU_ASSERT_EQUAL(attr->expression->opcode,                     EXPR_SCOPED_CALL  );
        CU_ASSERT_STRING_EQUAL(attr->expression->data->value.string,  " not empty ok?! ");
        CU_ASSERT_EQUAL(attr->expression->data->flags,                DATA_STRING       );
        CU_ASSERT_EQUAL(attr->expression->scope,                      env->scopes       );

        attr = attr->next;
        CU_ASSERT_FALSE(attr->flags & ATTR_INVISIBLE);
        CU_ASSERT_STRING_EQUAL(attr->namespace,                       "balanced"                                    );
        CU_ASSERT_STRING_EQUAL(attr->name,                            "scope"                                       );
        CU_ASSERT_NOT_EQUAL(attr->expression,                         NULL                                          );
        CU_ASSERT_EQUAL(attr->expression->opcode,                     EXPR_SCOPED_CALL                              );
        CU_ASSERT_STRING_EQUAL(attr->expression->data->value.string,  " balanced, { so i dont need to be escaped!} ");
        CU_ASSERT_EQUAL(attr->expression->data->flags,                DATA_STRING                                   );
        CU_ASSERT_EQUAL(attr->expression->scope,                      env->scopes                                   );

        attr = attr->next;
        CU_ASSERT(attr->flags & ATTR_INVISIBLE);
        CU_ASSERT_STRING_EQUAL(attr->namespace,                       "escaped"                                    );
        CU_ASSERT_STRING_EQUAL(attr->name,                            "scope"                                      );
        CU_ASSERT_NOT_EQUAL(attr->expression,                         NULL                                         );
        CU_ASSERT_EQUAL(attr->expression->opcode,                     EXPR_SCOPED_CALL                             );
        CU_ASSERT_STRING_EQUAL(attr->expression->data->value.string,  " escaped, so i dont { need to be balanced! ");
        CU_ASSERT_EQUAL(attr->expression->data->flags,                DATA_STRING                                  );
        CU_ASSERT_EQUAL(attr->expression->scope,                      env->scopes                                  );

        attr = attr->next;
        CU_ASSERT_FALSE(attr->flags & ATTR_INVISIBLE);
        CU_ASSERT_STRING_EQUAL(attr->namespace,                       "balanced_and_escaped"                                       );
        CU_ASSERT_STRING_EQUAL(attr->name,                            "scope"                                                      );
        CU_ASSERT_NOT_EQUAL(attr->expression,                         NULL                                                         );
        CU_ASSERT_EQUAL(attr->expression->opcode,                     EXPR_SCOPED_CALL                                             );
        CU_ASSERT_STRING_EQUAL(attr->expression->data->value.string,  " escaped, {so i \{ dont need to be } balanced every time!! ");
        CU_ASSERT_EQUAL(attr->expression->data->flags,                DATA_STRING                                                  );
        CU_ASSERT_EQUAL(attr->expression->scope,                      env->scopes                                                  );

        attr = attr->next;
        CU_ASSERT(attr->flags & ATTR_INVISIBLE);
        CU_ASSERT_EQUAL(attr->namespace,                       NULL         );
        CU_ASSERT_STRING_EQUAL(attr->name,                     "true_value" );
        CU_ASSERT_EQUAL(attr->expression->opcode,              EXPR_EVAL    );
        CU_ASSERT_EQUAL(attr->expression->data->value.boolean, TRUE         );
        CU_ASSERT_EQUAL(attr->expression->data->flags,         DATA_BOOLEAN );

        attr = attr->next;
        CU_ASSERT_FALSE(attr->flags & ATTR_INVISIBLE);
        CU_ASSERT_EQUAL(attr->namespace,                       NULL         );
        CU_ASSERT_STRING_EQUAL(attr->name,                     "false_value");
        CU_ASSERT_EQUAL(attr->expression->opcode,              EXPR_EVAL    );
        CU_ASSERT_EQUAL(attr->expression->data->value.boolean, FALSE        );
        CU_ASSERT_EQUAL(attr->expression->data->flags,         DATA_BOOLEAN );

        attr = attr->next;
        CU_ASSERT(attr->flags & ATTR_INVISIBLE);
        CU_ASSERT_EQUAL(attr->namespace,               NULL         );
        CU_ASSERT_STRING_EQUAL(attr->name,             "null_value" );
        CU_ASSERT_EQUAL(attr->expression->opcode,      EXPR_EVAL    );
        CU_ASSERT_EQUAL(attr->expression->data->flags, DATA_NULL    );

        CU_ASSERT_EQUAL(attr->next, NULL);

        CryEnv_cleanup(env);
    );

    free_parser_environment(env);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_array_attributes(void){
    CryEnv      *env;
    CryAttr      attr;
    CryArray     array;
    long double  precision = 0.00000000000001;

    env = init_parser_environment("rb");

    EXECUTE_TWICE(
        Parser_parse(env, "test/examples/array_attributes.cryml", 0);

        CU_ASSERT_EQUAL(env->root_tag->next,                     NULL);
        CU_ASSERT_EQUAL(env->root_tag->first_child->next,        NULL);
        CU_ASSERT_EQUAL(env->root_tag->first_child->first_child, NULL);

        attr  = env->root_tag->first_child->attributes;
        array = attr->expression->data->value.array;
        CU_ASSERT(attr->flags & ATTR_INVISIBLE);
        CU_ASSERT_STRING_EQUAL(attr->namespace,                                                                                    "expr"         );
        CU_ASSERT_STRING_EQUAL(attr->name,                                                                                         "array"        );
        CU_ASSERT_EQUAL(attr->expression->opcode,                                                                                  EXPR_ARRAY     );
        CU_ASSERT_EQUAL(CryArray_size(array),                                                                                      1              );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,                                                                           EXPR_ADD       );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags,                                                                      DATA_ARRAY     );
        CU_ASSERT_EQUAL(CryArray_size(GET_ARRAY_EXPRESSION(0)->data->value.array),                                                 2              );
        CU_ASSERT_EQUAL(CryArray_get(GET_ARRAY_EXPRESSION(0)->data->value.array, 0)->flags,                                        DATA_EXPR      );
        CU_ASSERT_EQUAL(CryArray_get(GET_ARRAY_EXPRESSION(0)->data->value.array, 0)->value.expression->opcode,                     EXPR_EVAL      );
        CU_ASSERT_EQUAL(CryArray_get(GET_ARRAY_EXPRESSION(0)->data->value.array, 0)->value.expression->data->flags,                DATA_INTEGER   );
        CU_ASSERT_EQUAL(CryArray_get(GET_ARRAY_EXPRESSION(0)->data->value.array, 0)->value.expression->data->value.integer,        2              );
        CU_ASSERT_EQUAL(CryArray_get(GET_ARRAY_EXPRESSION(0)->data->value.array, 1)->flags,                                        DATA_EXPR      );
        CU_ASSERT_EQUAL(CryArray_get(GET_ARRAY_EXPRESSION(0)->data->value.array, 1)->value.expression->opcode,                     EXPR_EVAL      );
        CU_ASSERT_EQUAL(CryArray_get(GET_ARRAY_EXPRESSION(0)->data->value.array, 1)->value.expression->data->flags,                DATA_DOUBLE    );
        CU_ASSERT_DOUBLE_EQUAL(CryArray_get(GET_ARRAY_EXPRESSION(0)->data->value.array, 1)->value.expression->data->value.cdouble, 10.0, precision);

        attr = attr->next;
        array = attr->expression->data->value.array;
        CU_ASSERT_FALSE(attr->flags & ATTR_INVISIBLE);
        CU_ASSERT_STRING_EQUAL(attr->namespace,               "par"       );
        CU_ASSERT_STRING_EQUAL(attr->name,                    "array"     );
        CU_ASSERT_EQUAL(attr->expression->opcode,             EXPR_ARRAY  );
        CU_ASSERT_EQUAL(CryArray_size(array),                 1           );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,      EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags, DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(0).integer,         42          );

        attr = attr->next;
        CU_ASSERT(attr->flags & ATTR_INVISIBLE);
        CU_ASSERT_STRING_EQUAL(attr->namespace,   "empty"   );
        CU_ASSERT_STRING_EQUAL(attr->name,        "array"   );
        CU_ASSERT_EQUAL(attr->expression->opcode, EXPR_ARRAY);
        CU_ASSERT_EQUAL(attr->expression->data,   NULL      );

        attr  = attr->next;
        array = attr->expression->data->value.array;
        CU_ASSERT_FALSE(attr->flags & ATTR_INVISIBLE);
        CU_ASSERT_STRING_EQUAL(attr->namespace,               "numeric"      );
        CU_ASSERT_STRING_EQUAL(attr->name,                    "array"        );
        CU_ASSERT_EQUAL(attr->expression->opcode,             EXPR_ARRAY     );
        CU_ASSERT_EQUAL(attr->expression->data->flags,        DATA_ARRAY     );
        CU_ASSERT_EQUAL(CryArray_size(array),                 6              );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,      EXPR_EVAL      );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,      EXPR_EVAL      );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(2)->opcode,      EXPR_EVAL      );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(3)->opcode,      EXPR_EVAL      );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(4)->opcode,      EXPR_EVAL      );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(5)->opcode,      EXPR_EVAL      );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags, DATA_INTEGER   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags, DATA_INTEGER   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(2)->data->flags, DATA_INTEGER   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(3)->data->flags, DATA_DOUBLE    );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(4)->data->flags, DATA_DOUBLE    );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(5)->data->flags, DATA_DOUBLE    );
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(0).integer,         1              );
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(1).integer,         2              );
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(2).integer,         3              );
        CU_ASSERT_DOUBLE_EQUAL(GET_ARRAY_ELEMENT(3).cdouble,  3.14, precision);
        CU_ASSERT_DOUBLE_EQUAL(GET_ARRAY_ELEMENT(4).cdouble,  6.28, precision);
        CU_ASSERT_DOUBLE_EQUAL(GET_ARRAY_ELEMENT(5).cdouble,  31.4, precision);

        attr  = attr->next;
        array = attr->expression->data->value.array;
        CU_ASSERT(attr->flags & ATTR_INVISIBLE);
        CU_ASSERT_STRING_EQUAL(attr->namespace,               "string"           );
        CU_ASSERT_STRING_EQUAL(attr->name,                    "array"            );
        CU_ASSERT_EQUAL(attr->expression->opcode,             EXPR_ARRAY         );
        CU_ASSERT_EQUAL(attr->expression->data->flags,        DATA_ARRAY         );
        CU_ASSERT_EQUAL(CryArray_size(array),                 3                  );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,      EXPR_EVAL          );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,      EXPR_EVAL          );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(2)->opcode,      EXPR_EVAL          );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags, DATA_STRING        );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags, DATA_STRING        );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(2)->data->flags, DATA_STRING        );
        CU_ASSERT_STRING_EQUAL(GET_ARRAY_ELEMENT(0).string,   "'single' string"  );
        CU_ASSERT_STRING_EQUAL(GET_ARRAY_ELEMENT(1).string,   "\"double\" string");
        CU_ASSERT_STRING_EQUAL(GET_ARRAY_ELEMENT(2).string,   "strings!"         );

        attr  = attr->next;
        array = attr->expression->data->value.array;
        CU_ASSERT_FALSE(attr->flags & ATTR_INVISIBLE);
        CU_ASSERT_STRING_EQUAL(attr->namespace,              "scope"                     );
        CU_ASSERT_STRING_EQUAL(attr->name,                   "array"                     );
        CU_ASSERT_EQUAL(attr->expression->opcode,            EXPR_ARRAY                  );
        CU_ASSERT_EQUAL(attr->expression->data->flags,       DATA_ARRAY                  );
        CU_ASSERT_EQUAL(CryArray_size(array),                4                           );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,     EXPR_SCOPED_CALL            );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,     EXPR_SCOPED_CALL            );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(2)->opcode,     EXPR_SCOPED_CALL            );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(3)->opcode,     EXPR_EVAL                   );
        CU_ASSERT_STRING_EQUAL(GET_ARRAY_ELEMENT(0).string,  " class << self; self; end ");
        CU_ASSERT_STRING_EQUAL(GET_ARRAY_ELEMENT(1).string,  " 10.times{ |x| puts x } "  );
        CU_ASSERT_STRING_EQUAL(GET_ARRAY_ELEMENT(2).string,  ""                          );
        CU_ASSERT_STRING_EQUAL(GET_ARRAY_ELEMENT(3).string,  "something else"            );

        attr  = attr->next;
        array = attr->expression->data->value.array;
        CU_ASSERT(attr->flags & ATTR_INVISIBLE);
        CU_ASSERT_STRING_EQUAL(attr->namespace,               "boolean"      );
        CU_ASSERT_STRING_EQUAL(attr->name,                    "array"        );
        CU_ASSERT_EQUAL(attr->expression->opcode,             EXPR_ARRAY     );
        CU_ASSERT_EQUAL(attr->expression->data->flags,        DATA_ARRAY     );
        CU_ASSERT_EQUAL(CryArray_size(array),                 5              );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,      EXPR_EVAL      );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,      EXPR_EVAL      );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(2)->opcode,      EXPR_EVAL      );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(3)->opcode,      EXPR_EVAL      );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(4)->opcode,      EXPR_EVAL      );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags, DATA_BOOLEAN   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags, DATA_BOOLEAN   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(2)->data->flags, DATA_NULL      );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(3)->data->flags, DATA_BOOLEAN   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(4)->data->flags, DATA_BOOLEAN   );
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(0).boolean,         TRUE           );
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(1).boolean,         FALSE          );
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(2).unknown,         NULL           );
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(3).boolean,         FALSE          );
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(4).boolean,         TRUE           );

        attr  = attr->next;
        array = attr->expression->data->value.array;
        CU_ASSERT_FALSE(attr->flags & ATTR_INVISIBLE);
        CU_ASSERT_STRING_EQUAL(attr->namespace,               "attributes");
        CU_ASSERT_STRING_EQUAL(attr->name,                    "array"     );
        CU_ASSERT_EQUAL(attr->expression->opcode,             EXPR_ARRAY  );
        CU_ASSERT_EQUAL(attr->expression->data->flags,        DATA_ARRAY  );
        CU_ASSERT_EQUAL(CryArray_size(array),                 4           );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,      EXPR_ATTR   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,      EXPR_ATTR   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(2)->opcode,      EXPR_ATTR   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(3)->opcode,      EXPR_ATTR   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags, DATA_STRING );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags, DATA_STRING );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(2)->data->flags, DATA_STRING );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(3)->data->flags, DATA_STRING );
        CU_ASSERT_STRING_EQUAL(GET_ARRAY_ELEMENT(0).string,   "attr"      );
        CU_ASSERT_STRING_EQUAL(GET_ARRAY_ELEMENT(1).string,   "ns:attr"   );
        CU_ASSERT_STRING_EQUAL(GET_ARRAY_ELEMENT(2).string,   "true"      );
        CU_ASSERT_STRING_EQUAL(GET_ARRAY_ELEMENT(3).string,   "null"      );

        attr  = attr->next;
        array = attr->expression->data->value.array;
        CU_ASSERT(attr->flags & ATTR_INVISIBLE);
        CU_ASSERT_STRING_EQUAL(attr->namespace,                                           "function");
        CU_ASSERT_STRING_EQUAL(attr->name,                                                "array"   );
        CU_ASSERT_EQUAL(attr->expression->opcode,                                         EXPR_ARRAY);
        CU_ASSERT_EQUAL(attr->expression->data->flags,                                    DATA_ARRAY);
        CU_ASSERT_EQUAL(CryArray_size(array),                                             4         );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,                                  EXPR_FUNC );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,                                  EXPR_FUNC );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(2)->opcode,                                  EXPR_FUNC );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(3)->opcode,                                  EXPR_FUNC );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags,                             DATA_ARRAY);
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags,                             DATA_ARRAY);
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(2)->data->flags,                             DATA_ARRAY);
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(3)->data->flags,                             DATA_ARRAY);
        CU_ASSERT_STRING_EQUAL(CryArray_get(GET_ARRAY_ELEMENT(0).array, 0)->value.string, "exit"    );
        CU_ASSERT_STRING_EQUAL(CryArray_get(GET_ARRAY_ELEMENT(1).array, 0)->value.string, "atoi"    );
        CU_ASSERT_STRING_EQUAL(CryArray_get(GET_ARRAY_ELEMENT(2).array, 0)->value.string, "print"   );
        CU_ASSERT_STRING_EQUAL(CryArray_get(GET_ARRAY_ELEMENT(3).array, 0)->value.string, "wharever");

        CU_ASSERT_EQUAL(CryArray_size(CryArray_get(GET_ARRAY_ELEMENT(0).array, 1)->value.array), 0);
        CU_ASSERT_EQUAL(CryArray_size(CryArray_get(GET_ARRAY_ELEMENT(1).array, 1)->value.array), 1);
        CU_ASSERT_EQUAL(CryArray_size(CryArray_get(GET_ARRAY_ELEMENT(2).array, 1)->value.array), 2);
        CU_ASSERT_EQUAL(CryArray_size(CryArray_get(GET_ARRAY_ELEMENT(3).array, 1)->value.array), 2);

        CU_ASSERT_EQUAL(CryArray_get(CryArray_get(GET_ARRAY_ELEMENT(1).array, 1)->value.array, 0)->value.expression->opcode, EXPR_EVAL );
        CU_ASSERT_EQUAL(CryArray_get(CryArray_get(GET_ARRAY_ELEMENT(2).array, 1)->value.array, 0)->value.expression->opcode, EXPR_EVAL );
        CU_ASSERT_EQUAL(CryArray_get(CryArray_get(GET_ARRAY_ELEMENT(2).array, 1)->value.array, 1)->value.expression->opcode, EXPR_ATTR );
        CU_ASSERT_EQUAL(CryArray_get(CryArray_get(GET_ARRAY_ELEMENT(3).array, 1)->value.array, 0)->value.expression->opcode, EXPR_ARRAY);
        CU_ASSERT_EQUAL(CryArray_get(CryArray_get(GET_ARRAY_ELEMENT(3).array, 1)->value.array, 1)->value.expression->opcode, EXPR_HASH );

        CU_ASSERT_EQUAL(CryArray_get(CryArray_get(GET_ARRAY_ELEMENT(1).array, 1)->value.array, 0)->value.expression->data->flags, DATA_STRING);
        CU_ASSERT_EQUAL(CryArray_get(CryArray_get(GET_ARRAY_ELEMENT(2).array, 1)->value.array, 0)->value.expression->data->flags, DATA_STRING);
        CU_ASSERT_EQUAL(CryArray_get(CryArray_get(GET_ARRAY_ELEMENT(2).array, 1)->value.array, 1)->value.expression->data->flags, DATA_STRING);
        CU_ASSERT_EQUAL(CryArray_get(CryArray_get(GET_ARRAY_ELEMENT(3).array, 1)->value.array, 0)->value.expression->data->flags, DATA_ARRAY );
        CU_ASSERT_EQUAL(CryArray_get(CryArray_get(GET_ARRAY_ELEMENT(3).array, 1)->value.array, 1)->value.expression->data->flags, DATA_ARRAY );

        CU_ASSERT_STRING_EQUAL(       CryArray_get(CryArray_get(GET_ARRAY_ELEMENT(1).array, 1)->value.array, 0)->value.expression->data->value.string, "1234"       );
        CU_ASSERT_STRING_EQUAL(       CryArray_get(CryArray_get(GET_ARRAY_ELEMENT(2).array, 1)->value.array, 0)->value.expression->data->value.string, "string %40s");
        CU_ASSERT_STRING_EQUAL(       CryArray_get(CryArray_get(GET_ARRAY_ELEMENT(2).array, 1)->value.array, 1)->value.expression->data->value.string, "name"       );
        CU_ASSERT_EQUAL(CryArray_size(CryArray_get(CryArray_get(GET_ARRAY_ELEMENT(3).array, 1)->value.array, 0)->value.expression->data->value.array), 3            );
        CU_ASSERT_EQUAL(CryArray_size(CryArray_get(CryArray_get(GET_ARRAY_ELEMENT(3).array, 1)->value.array, 1)->value.expression->data->value.array), 2            );

        CU_ASSERT_EQUAL(CryArray_get(CryArray_get(CryArray_get(GET_ARRAY_ELEMENT(3).array, 1)->value.array, 0)->value.expression->data->value.array, 0)->value.expression->opcode, EXPR_EVAL  );
        CU_ASSERT_EQUAL(CryArray_get(CryArray_get(CryArray_get(GET_ARRAY_ELEMENT(3).array, 1)->value.array, 0)->value.expression->data->value.array, 1)->value.expression->opcode, EXPR_EVAL  );
        CU_ASSERT_EQUAL(CryArray_get(CryArray_get(CryArray_get(GET_ARRAY_ELEMENT(3).array, 1)->value.array, 0)->value.expression->data->value.array, 2)->value.expression->opcode, EXPR_EVAL  );
        CU_ASSERT_EQUAL(       CryArray_get(CryArray_get(CryArray_get(GET_ARRAY_ELEMENT(3).array, 1)->value.array, 1)->value.expression->data->value.array, 0)->flags,             DATA_STRING);
        CU_ASSERT_STRING_EQUAL(CryArray_get(CryArray_get(CryArray_get(GET_ARRAY_ELEMENT(3).array, 1)->value.array, 1)->value.expression->data->value.array, 0)->value.string,      "key"      );
        CU_ASSERT_EQUAL(CryArray_get(CryArray_get(CryArray_get(GET_ARRAY_ELEMENT(3).array, 1)->value.array, 1)->value.expression->data->value.array, 1)->value.expression->opcode, EXPR_EVAL  );

        CU_ASSERT_EQUAL(CryArray_get(CryArray_get(CryArray_get(GET_ARRAY_ELEMENT(3).array, 1)->value.array, 0)->value.expression->data->value.array, 0)->value.expression->data->flags, DATA_INTEGER);
        CU_ASSERT_EQUAL(CryArray_get(CryArray_get(CryArray_get(GET_ARRAY_ELEMENT(3).array, 1)->value.array, 0)->value.expression->data->value.array, 1)->value.expression->data->flags, DATA_INTEGER);
        CU_ASSERT_EQUAL(CryArray_get(CryArray_get(CryArray_get(GET_ARRAY_ELEMENT(3).array, 1)->value.array, 0)->value.expression->data->value.array, 2)->value.expression->data->flags, DATA_INTEGER);
        CU_ASSERT_EQUAL(CryArray_get(CryArray_get(CryArray_get(GET_ARRAY_ELEMENT(3).array, 1)->value.array, 1)->value.expression->data->value.array, 1)->value.expression->data->flags, DATA_STRING );

        CU_ASSERT_EQUAL(       CryArray_get(CryArray_get(CryArray_get(GET_ARRAY_ELEMENT(3).array, 1)->value.array, 0)->value.expression->data->value.array, 0)->value.expression->data->value.integer, 1      );
        CU_ASSERT_EQUAL(       CryArray_get(CryArray_get(CryArray_get(GET_ARRAY_ELEMENT(3).array, 1)->value.array, 0)->value.expression->data->value.array, 1)->value.expression->data->value.integer, 2      );
        CU_ASSERT_EQUAL(       CryArray_get(CryArray_get(CryArray_get(GET_ARRAY_ELEMENT(3).array, 1)->value.array, 0)->value.expression->data->value.array, 2)->value.expression->data->value.integer, 3      );
        CU_ASSERT_STRING_EQUAL(CryArray_get(CryArray_get(CryArray_get(GET_ARRAY_ELEMENT(3).array, 1)->value.array, 1)->value.expression->data->value.array, 1)->value.expression->data->value.string,  "value");

        attr  = attr->next;
        array = attr->expression->data->value.array;
        CU_ASSERT_FALSE(attr->flags & ATTR_INVISIBLE);
        CU_ASSERT_STRING_EQUAL(attr->namespace,               "nested"    );
        CU_ASSERT_STRING_EQUAL(attr->name,                    "arrays"    );
        CU_ASSERT_EQUAL(attr->expression->opcode,             EXPR_ARRAY  );
        CU_ASSERT_EQUAL(attr->expression->data->flags,        DATA_ARRAY  );
        CU_ASSERT_EQUAL(CryArray_size(array),                 4           );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,      EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,      EXPR_ARRAY  );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(2)->opcode,      EXPR_ARRAY  );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(3)->opcode,      EXPR_ARRAY  );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags, DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags, DATA_ARRAY  );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(2)->data->flags, DATA_ARRAY  );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(3)->data->flags, DATA_ARRAY  );

        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(0).integer, 1);

        CU_ASSERT_EQUAL(CryArray_size(GET_ARRAY_ELEMENT(1).array),                                          1           );
        CU_ASSERT_EQUAL(CryArray_get(GET_ARRAY_ELEMENT(1).array, 0)->flags,                                 DATA_EXPR   );
        CU_ASSERT_EQUAL(CryArray_get(GET_ARRAY_ELEMENT(1).array, 0)->value.expression->opcode,              EXPR_EVAL   );
        CU_ASSERT_EQUAL(CryArray_get(GET_ARRAY_ELEMENT(1).array, 0)->value.expression->data->flags,         DATA_INTEGER);
        CU_ASSERT_EQUAL(CryArray_get(GET_ARRAY_ELEMENT(1).array, 0)->value.expression->data->value.integer, 2           );

        CU_ASSERT_EQUAL(CryArray_size(GET_ARRAY_ELEMENT(2).array),                                          2           );
        CU_ASSERT_EQUAL(CryArray_get(GET_ARRAY_ELEMENT(2).array, 0)->flags,                                 DATA_EXPR   );
        CU_ASSERT_EQUAL(CryArray_get(GET_ARRAY_ELEMENT(2).array, 0)->value.expression->opcode,              EXPR_EVAL   );
        CU_ASSERT_EQUAL(CryArray_get(GET_ARRAY_ELEMENT(2).array, 0)->value.expression->data->flags,         DATA_INTEGER);
        CU_ASSERT_EQUAL(CryArray_get(GET_ARRAY_ELEMENT(2).array, 0)->value.expression->data->value.integer, 3           );
        CU_ASSERT_EQUAL(CryArray_get(GET_ARRAY_ELEMENT(2).array, 1)->flags,                                 DATA_EXPR   );
        CU_ASSERT_EQUAL(CryArray_get(GET_ARRAY_ELEMENT(2).array, 1)->value.expression->opcode,              EXPR_EVAL   );
        CU_ASSERT_EQUAL(CryArray_get(GET_ARRAY_ELEMENT(2).array, 1)->value.expression->data->flags,         DATA_INTEGER);
        CU_ASSERT_EQUAL(CryArray_get(GET_ARRAY_ELEMENT(2).array, 1)->value.expression->data->value.integer, 4           );

        CU_ASSERT_EQUAL(CryArray_size(GET_ARRAY_ELEMENT(3).array),                                                                                                2           );
        CU_ASSERT_EQUAL(CryArray_get(GET_ARRAY_ELEMENT(3).array, 0)->flags,                                                                                       DATA_EXPR   );
        CU_ASSERT_EQUAL(CryArray_get(GET_ARRAY_ELEMENT(3).array, 0)->value.expression->opcode,                                                                    EXPR_EVAL   );
        CU_ASSERT_EQUAL(CryArray_get(GET_ARRAY_ELEMENT(3).array, 0)->value.expression->data->flags,                                                               DATA_INTEGER);
        CU_ASSERT_EQUAL(CryArray_get(GET_ARRAY_ELEMENT(3).array, 0)->value.expression->data->value.integer,                                                       5           );
        CU_ASSERT_EQUAL(CryArray_get(GET_ARRAY_ELEMENT(3).array, 1)->flags,                                                                                       DATA_EXPR   );
        CU_ASSERT_EQUAL(CryArray_get(GET_ARRAY_ELEMENT(3).array, 1)->value.expression->opcode,                                                                    EXPR_ARRAY  );
        CU_ASSERT_EQUAL(CryArray_get(GET_ARRAY_ELEMENT(3).array, 1)->value.expression->data->flags,                                                               DATA_ARRAY  );
        CU_ASSERT_EQUAL(CryArray_size(CryArray_get(GET_ARRAY_ELEMENT(3).array, 1)->value.expression->data->value.array),                                          1           );
        CU_ASSERT_EQUAL(CryArray_get(CryArray_get(GET_ARRAY_ELEMENT(3).array, 1)->value.expression->data->value.array, 0)->flags,                                 DATA_EXPR   );
        CU_ASSERT_EQUAL(CryArray_get(CryArray_get(GET_ARRAY_ELEMENT(3).array, 1)->value.expression->data->value.array, 0)->value.expression->opcode,              EXPR_EVAL   );
        CU_ASSERT_EQUAL(CryArray_get(CryArray_get(GET_ARRAY_ELEMENT(3).array, 1)->value.expression->data->value.array, 0)->value.expression->data->flags,         DATA_INTEGER);
        CU_ASSERT_EQUAL(CryArray_get(CryArray_get(GET_ARRAY_ELEMENT(3).array, 1)->value.expression->data->value.array, 0)->value.expression->data->value.integer, 6           );

        attr  = attr->next;
        array = attr->expression->data->value.array;
        CU_ASSERT(attr->flags & ATTR_INVISIBLE);
        CU_ASSERT_STRING_EQUAL(attr->namespace,               "hashed"    );
        CU_ASSERT_STRING_EQUAL(attr->name,                    "array"     );
        CU_ASSERT_EQUAL(attr->expression->opcode,             EXPR_ARRAY  );
        CU_ASSERT_EQUAL(attr->expression->data->flags,        DATA_ARRAY  );
        CU_ASSERT_EQUAL(CryArray_size(array),                 3           );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,      EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,      EXPR_HASH   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(2)->opcode,      EXPR_HASH   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags, DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags, DATA_ARRAY  );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(2)->data->flags, DATA_ARRAY  );

        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(0).integer, 99);

        CU_ASSERT_EQUAL(CryArray_size(GET_ARRAY_ELEMENT(1).array),                                          2           );
        CU_ASSERT_EQUAL(CryArray_get(GET_ARRAY_ELEMENT(1).array, 0)->flags,                                 DATA_STRING );
        CU_ASSERT_STRING_EQUAL(CryArray_get(GET_ARRAY_ELEMENT(1).array, 0)->value.string,                   "key"       );
        CU_ASSERT_EQUAL(CryArray_get(GET_ARRAY_ELEMENT(1).array, 1)->flags,                                 DATA_EXPR   );
        CU_ASSERT_EQUAL(CryArray_get(GET_ARRAY_ELEMENT(1).array, 1)->value.expression->opcode,              EXPR_EVAL   );
        CU_ASSERT_EQUAL(CryArray_get(GET_ARRAY_ELEMENT(1).array, 1)->value.expression->data->flags,         DATA_INTEGER);
        CU_ASSERT_EQUAL(CryArray_get(GET_ARRAY_ELEMENT(1).array, 1)->value.expression->data->value.integer, 123         );

        CU_ASSERT_EQUAL(CryArray_size(GET_ARRAY_ELEMENT(2).array),                                                 4             );
        CU_ASSERT_EQUAL(CryArray_get(GET_ARRAY_ELEMENT(2).array, 0)->flags,                                        DATA_STRING   );
        CU_ASSERT_STRING_EQUAL(CryArray_get(GET_ARRAY_ELEMENT(2).array, 0)->value.string,                          "one"         );
        CU_ASSERT_EQUAL(CryArray_get(GET_ARRAY_ELEMENT(2).array, 1)->flags,                                        DATA_EXPR     );
        CU_ASSERT_EQUAL(CryArray_get(GET_ARRAY_ELEMENT(2).array, 1)->value.expression->opcode,                     EXPR_EVAL     );
        CU_ASSERT_EQUAL(CryArray_get(GET_ARRAY_ELEMENT(2).array, 1)->value.expression->data->flags,                DATA_INTEGER  );
        CU_ASSERT_EQUAL(CryArray_get(GET_ARRAY_ELEMENT(2).array, 1)->value.expression->data->value.integer,        1             );
        CU_ASSERT_EQUAL(CryArray_get(GET_ARRAY_ELEMENT(2).array, 2)->flags,                                        DATA_STRING   );
        CU_ASSERT_STRING_EQUAL(CryArray_get(GET_ARRAY_ELEMENT(2).array, 2)->value.string,                          "two"         );
        CU_ASSERT_EQUAL(CryArray_get(GET_ARRAY_ELEMENT(2).array, 3)->flags,                                        DATA_EXPR     );
        CU_ASSERT_EQUAL(CryArray_get(GET_ARRAY_ELEMENT(2).array, 3)->value.expression->opcode,                     EXPR_EVAL     );
        CU_ASSERT_EQUAL(CryArray_get(GET_ARRAY_ELEMENT(2).array, 3)->value.expression->data->flags,                DATA_DOUBLE   );
        CU_ASSERT_DOUBLE_EQUAL(CryArray_get(GET_ARRAY_ELEMENT(2).array, 3)->value.expression->data->value.cdouble, 2.0, precision);


        CU_ASSERT_FALSE(attr->next);
        CryEnv_cleanup(env);
    );

    free_parser_environment(env);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_function_attributes(void){
    CryEnv      *env;
    CryAttr      attr;
    CryArray     array;
    long double  precision = 0.00000000000001;

    env = init_parser_environment("rb");

    EXECUTE_TWICE(
        Parser_parse(env, "test/examples/function_attributes.cryml", 0);

        CU_ASSERT_EQUAL(env->root_tag->next,                     NULL);
        CU_ASSERT_EQUAL(env->root_tag->first_child->next,        NULL);
        CU_ASSERT_EQUAL(env->root_tag->first_child->first_child, NULL);

        attr  = env->root_tag->first_child->attributes;
        array = attr->expression->data->value.array;
        CU_ASSERT(attr->flags & ATTR_INVISIBLE);
        CU_ASSERT_STRING_EQUAL(attr->namespace,                                                                                     "expr"         );
        CU_ASSERT_STRING_EQUAL(attr->name,                                                                                          "function"     );
        CU_ASSERT_EQUAL(attr->expression->opcode,                                                                                   EXPR_FUNC      );
        CU_ASSERT_EQUAL(CryArray_size(array),                                                                                       2              );
        CU_ASSERT_STRING_EQUAL(GET_FUNCTION_NAME,                                                                                   "expression"   );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_COUNT,                                                                                     1              );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(0)->opcode,                                                                           EXPR_ADD       );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(0)->data->flags,                                                                      DATA_ARRAY     );
        CU_ASSERT_EQUAL(CryArray_size(GET_FUNCTION_ARG_EXPR(0)->data->value.array),                                                 2              );
        CU_ASSERT_EQUAL(CryArray_get(GET_FUNCTION_ARG_EXPR(0)->data->value.array, 0)->flags,                                        DATA_EXPR      );
        CU_ASSERT_EQUAL(CryArray_get(GET_FUNCTION_ARG_EXPR(0)->data->value.array, 0)->value.expression->opcode,                     EXPR_EVAL      );
        CU_ASSERT_EQUAL(CryArray_get(GET_FUNCTION_ARG_EXPR(0)->data->value.array, 0)->value.expression->data->flags,                DATA_INTEGER   );
        CU_ASSERT_EQUAL(CryArray_get(GET_FUNCTION_ARG_EXPR(0)->data->value.array, 0)->value.expression->data->value.integer,        2              );
        CU_ASSERT_EQUAL(CryArray_get(GET_FUNCTION_ARG_EXPR(0)->data->value.array, 1)->flags,                                        DATA_EXPR      );
        CU_ASSERT_EQUAL(CryArray_get(GET_FUNCTION_ARG_EXPR(0)->data->value.array, 1)->value.expression->opcode,                     EXPR_EVAL      );
        CU_ASSERT_EQUAL(CryArray_get(GET_FUNCTION_ARG_EXPR(0)->data->value.array, 1)->value.expression->data->flags,                DATA_DOUBLE    );
        CU_ASSERT_DOUBLE_EQUAL(CryArray_get(GET_FUNCTION_ARG_EXPR(0)->data->value.array, 1)->value.expression->data->value.cdouble, 10.0, precision);

        attr  = attr->next;
        array = attr->expression->data->value.array;
        CU_ASSERT_FALSE(attr->flags & ATTR_INVISIBLE);
        CU_ASSERT_STRING_EQUAL(attr->namespace,                "par"        );
        CU_ASSERT_STRING_EQUAL(attr->name,                     "function"   );
        CU_ASSERT_EQUAL(attr->expression->opcode,              EXPR_FUNC    );
        CU_ASSERT_EQUAL(CryArray_size(array),                  2            );
        CU_ASSERT_STRING_EQUAL(GET_FUNCTION_NAME,              "parenthesis");
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_COUNT,                1            );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(0)->opcode,      EXPR_EVAL    );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(0)->data->flags, DATA_INTEGER );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_VALUE(0).integer,     42           );

        attr = attr->next;
        array = attr->expression->data->value.array;
        CU_ASSERT(attr->flags & ATTR_INVISIBLE);
        CU_ASSERT_STRING_EQUAL(attr->namespace,        "empty"         );
        CU_ASSERT_STRING_EQUAL(attr->name,             "function"      );
        CU_ASSERT_EQUAL(attr->expression->opcode,      EXPR_FUNC       );
        CU_ASSERT_EQUAL(attr->expression->data->flags, DATA_ARRAY      );
        CU_ASSERT_EQUAL(CryArray_size(array),          2               );
        CU_ASSERT_STRING_EQUAL(GET_FUNCTION_NAME,      "empty_function");
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_COUNT,        0               );

        attr  = attr->next;
        array = attr->expression->data->value.array;
        CU_ASSERT_FALSE(attr->flags & ATTR_INVISIBLE);
        CU_ASSERT_STRING_EQUAL(attr->namespace,                   "numeric"         );
        CU_ASSERT_STRING_EQUAL(attr->name,                        "function"        );
        CU_ASSERT_EQUAL(attr->expression->opcode,                 EXPR_FUNC         );
        CU_ASSERT_EQUAL(attr->expression->data->flags,            DATA_ARRAY        );
        CU_ASSERT_EQUAL(CryArray_size(array),                     2                 );
        CU_ASSERT_STRING_EQUAL(GET_FUNCTION_NAME,                 "numeric_function");
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_COUNT,                   6                 );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(0)->opcode,         EXPR_EVAL         );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(1)->opcode,         EXPR_EVAL         );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(2)->opcode,         EXPR_EVAL         );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(3)->opcode,         EXPR_EVAL         );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(4)->opcode,         EXPR_EVAL         );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(5)->opcode,         EXPR_EVAL         );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(0)->data->flags,    DATA_INTEGER      );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(1)->data->flags,    DATA_INTEGER      );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(2)->data->flags,    DATA_INTEGER      );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(3)->data->flags,    DATA_DOUBLE       );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(4)->data->flags,    DATA_DOUBLE       );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(5)->data->flags,    DATA_DOUBLE       );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_VALUE(0).integer,        1                 );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_VALUE(1).integer,        2                 );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_VALUE(2).integer,        3                 );
        CU_ASSERT_DOUBLE_EQUAL(GET_FUNCTION_ARG_VALUE(3).cdouble, 3.14, precision   );
        CU_ASSERT_DOUBLE_EQUAL(GET_FUNCTION_ARG_VALUE(4).cdouble, 6.28, precision   );
        CU_ASSERT_DOUBLE_EQUAL(GET_FUNCTION_ARG_VALUE(5).cdouble, 31.4, precision   );

        attr  = attr->next;
        array = attr->expression->data->value.array;
        CU_ASSERT(attr->flags & ATTR_INVISIBLE);
        CU_ASSERT_STRING_EQUAL(attr->namespace,                  "string"           );
        CU_ASSERT_STRING_EQUAL(attr->name,                       "function"         );
        CU_ASSERT_EQUAL(attr->expression->opcode,                EXPR_FUNC          );
        CU_ASSERT_EQUAL(attr->expression->data->flags,           DATA_ARRAY         );
        CU_ASSERT_EQUAL(CryArray_size(array),                    2                  );
        CU_ASSERT_STRING_EQUAL(GET_FUNCTION_NAME,                "cat"              );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_COUNT,                  3                  );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(0)->opcode,        EXPR_EVAL          );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(1)->opcode,        EXPR_EVAL          );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(2)->opcode,        EXPR_EVAL          );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(0)->data->flags,   DATA_STRING        );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(1)->data->flags,   DATA_STRING        );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(2)->data->flags,   DATA_STRING        );
        CU_ASSERT_STRING_EQUAL(GET_FUNCTION_ARG_VALUE(0).string, "'single' string"  );
        CU_ASSERT_STRING_EQUAL(GET_FUNCTION_ARG_VALUE(1).string, "\"double\" string");
        CU_ASSERT_STRING_EQUAL(GET_FUNCTION_ARG_VALUE(2).string, "strings!"         );

        attr  = attr->next;
        array = attr->expression->data->value.array;
        CU_ASSERT_FALSE(attr->flags & ATTR_INVISIBLE);
        CU_ASSERT_STRING_EQUAL(attr->namespace,                  "scope"                     );
        CU_ASSERT_STRING_EQUAL(attr->name,                       "function"                  );
        CU_ASSERT_EQUAL(attr->expression->opcode,                EXPR_FUNC                   );
        CU_ASSERT_EQUAL(attr->expression->data->flags,           DATA_ARRAY                  );
        CU_ASSERT_EQUAL(CryArray_size(array),                    2                           );
        CU_ASSERT_STRING_EQUAL(GET_FUNCTION_NAME,                "scoped"                    );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_COUNT,                  4                           );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(0)->opcode,        EXPR_SCOPED_CALL            );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(1)->opcode,        EXPR_SCOPED_CALL            );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(2)->opcode,        EXPR_SCOPED_CALL            );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(3)->opcode,        EXPR_EVAL                   );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(0)->data->flags,   DATA_STRING                 );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(1)->data->flags,   DATA_STRING                 );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(2)->data->flags,   DATA_STRING                 );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(3)->data->flags,   DATA_STRING                 );
        CU_ASSERT_STRING_EQUAL(GET_FUNCTION_ARG_VALUE(0).string, " class << self; self; end ");
        CU_ASSERT_STRING_EQUAL(GET_FUNCTION_ARG_VALUE(1).string, " 10.times{ |x| puts x } "  );
        CU_ASSERT_STRING_EQUAL(GET_FUNCTION_ARG_VALUE(2).string, ""                          );
        CU_ASSERT_STRING_EQUAL(GET_FUNCTION_ARG_VALUE(3).string, "something else"            );

        attr  = attr->next;
        array = attr->expression->data->value.array;
        CU_ASSERT(attr->flags & ATTR_INVISIBLE);
        CU_ASSERT_STRING_EQUAL(attr->namespace,                "boolean"        );
        CU_ASSERT_STRING_EQUAL(attr->name,                     "function"       );
        CU_ASSERT_EQUAL(attr->expression->opcode,              EXPR_FUNC        );
        CU_ASSERT_EQUAL(attr->expression->data->flags,         DATA_ARRAY       );
        CU_ASSERT_EQUAL(CryArray_size(array),                  2                );
        CU_ASSERT_STRING_EQUAL(GET_FUNCTION_NAME,              "trues_and_false");
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_COUNT,                5                );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(0)->opcode,      EXPR_EVAL        );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(1)->opcode,      EXPR_EVAL        );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(2)->opcode,      EXPR_EVAL        );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(3)->opcode,      EXPR_EVAL        );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(4)->opcode,      EXPR_EVAL        );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(0)->data->flags, DATA_BOOLEAN     );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(1)->data->flags, DATA_BOOLEAN     );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(2)->data->flags, DATA_NULL        );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(3)->data->flags, DATA_BOOLEAN     );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(4)->data->flags, DATA_BOOLEAN     );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_VALUE(0).boolean,     TRUE             );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_VALUE(1).boolean,     FALSE            );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_VALUE(2).unknown,     NULL             );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_VALUE(3).boolean,     FALSE            );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_VALUE(4).boolean,     TRUE             );

        attr  = attr->next;
        array = attr->expression->data->value.array;
        CU_ASSERT_FALSE(attr->flags & ATTR_INVISIBLE);
        CU_ASSERT_STRING_EQUAL(attr->namespace,                  "attributes");
        CU_ASSERT_STRING_EQUAL(attr->name,                       "function"  );
        CU_ASSERT_EQUAL(attr->expression->opcode,                EXPR_FUNC   );
        CU_ASSERT_EQUAL(attr->expression->data->flags,           DATA_ARRAY  );
        CU_ASSERT_EQUAL(CryArray_size(array),                    2           );
        CU_ASSERT_STRING_EQUAL(GET_FUNCTION_NAME,                "attributes");
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_COUNT,                  4           );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(0)->opcode,        EXPR_ATTR   );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(1)->opcode,        EXPR_ATTR   );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(2)->opcode,        EXPR_ATTR   );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(3)->opcode,        EXPR_ATTR   );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(0)->data->flags,   DATA_STRING );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(1)->data->flags,   DATA_STRING );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(2)->data->flags,   DATA_STRING );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(3)->data->flags,   DATA_STRING );
        CU_ASSERT_STRING_EQUAL(GET_FUNCTION_ARG_VALUE(0).string, "attr"      );
        CU_ASSERT_STRING_EQUAL(GET_FUNCTION_ARG_VALUE(1).string, "ns:attr"   );
        CU_ASSERT_STRING_EQUAL(GET_FUNCTION_ARG_VALUE(2).string, "true"      );
        CU_ASSERT_STRING_EQUAL(GET_FUNCTION_ARG_VALUE(3).string, "null"      );

        attr  = attr->next;
        array = attr->expression->data->value.array;
        CU_ASSERT(attr->flags & ATTR_INVISIBLE);
        CU_ASSERT_STRING_EQUAL(attr->namespace,                                                  "func"     );
        CU_ASSERT_STRING_EQUAL(attr->name,                                                       "function" );
        CU_ASSERT_EQUAL(attr->expression->opcode,                                                EXPR_FUNC  );
        CU_ASSERT_EQUAL(attr->expression->data->flags,                                           DATA_ARRAY );
        CU_ASSERT_EQUAL(CryArray_size(array),                                                    2          );
        CU_ASSERT_STRING_EQUAL(GET_FUNCTION_NAME,                                                "functions");
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_COUNT,                                                  4          );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(0)->opcode,                                        EXPR_FUNC  );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(1)->opcode,                                        EXPR_FUNC  );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(2)->opcode,                                        EXPR_FUNC  );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(3)->opcode,                                        EXPR_FUNC  );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(0)->data->flags,                                   DATA_ARRAY );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(1)->data->flags,                                   DATA_ARRAY );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(2)->data->flags,                                   DATA_ARRAY );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(3)->data->flags,                                   DATA_ARRAY );
        CU_ASSERT_STRING_EQUAL(CryArray_get(GET_FUNCTION_ARG_VALUE(0).array, 0)->value.string,  "exit"      );
        CU_ASSERT_STRING_EQUAL(CryArray_get(GET_FUNCTION_ARG_VALUE(1).array, 0)->value.string,  "atoi"      );
        CU_ASSERT_STRING_EQUAL(CryArray_get(GET_FUNCTION_ARG_VALUE(2).array, 0)->value.string,  "print"     );
        CU_ASSERT_STRING_EQUAL(CryArray_get(GET_FUNCTION_ARG_VALUE(3).array, 0)->value.string,  "wharever"  );

        CU_ASSERT_EQUAL(CryArray_size(CryArray_get(GET_FUNCTION_ARG_VALUE(0).array, 1)->value.array), 0);
        CU_ASSERT_EQUAL(CryArray_size(CryArray_get(GET_FUNCTION_ARG_VALUE(1).array, 1)->value.array), 1);
        CU_ASSERT_EQUAL(CryArray_size(CryArray_get(GET_FUNCTION_ARG_VALUE(2).array, 1)->value.array), 2);
        CU_ASSERT_EQUAL(CryArray_size(CryArray_get(GET_FUNCTION_ARG_VALUE(3).array, 1)->value.array), 2);

        CU_ASSERT_EQUAL(CryArray_get(CryArray_get(GET_FUNCTION_ARG_VALUE(1).array, 1)->value.array, 0)->value.expression->opcode, EXPR_EVAL );
        CU_ASSERT_EQUAL(CryArray_get(CryArray_get(GET_FUNCTION_ARG_VALUE(2).array, 1)->value.array, 0)->value.expression->opcode, EXPR_EVAL );
        CU_ASSERT_EQUAL(CryArray_get(CryArray_get(GET_FUNCTION_ARG_VALUE(2).array, 1)->value.array, 1)->value.expression->opcode, EXPR_ATTR );
        CU_ASSERT_EQUAL(CryArray_get(CryArray_get(GET_FUNCTION_ARG_VALUE(3).array, 1)->value.array, 0)->value.expression->opcode, EXPR_ARRAY);
        CU_ASSERT_EQUAL(CryArray_get(CryArray_get(GET_FUNCTION_ARG_VALUE(3).array, 1)->value.array, 1)->value.expression->opcode, EXPR_HASH );

        CU_ASSERT_EQUAL(CryArray_get(CryArray_get(GET_FUNCTION_ARG_VALUE(1).array, 1)->value.array, 0)->value.expression->data->flags, DATA_STRING);
        CU_ASSERT_EQUAL(CryArray_get(CryArray_get(GET_FUNCTION_ARG_VALUE(2).array, 1)->value.array, 0)->value.expression->data->flags, DATA_STRING);
        CU_ASSERT_EQUAL(CryArray_get(CryArray_get(GET_FUNCTION_ARG_VALUE(2).array, 1)->value.array, 1)->value.expression->data->flags, DATA_STRING);
        CU_ASSERT_EQUAL(CryArray_get(CryArray_get(GET_FUNCTION_ARG_VALUE(3).array, 1)->value.array, 0)->value.expression->data->flags, DATA_ARRAY );
        CU_ASSERT_EQUAL(CryArray_get(CryArray_get(GET_FUNCTION_ARG_VALUE(3).array, 1)->value.array, 1)->value.expression->data->flags, DATA_ARRAY );

        CU_ASSERT_STRING_EQUAL(       CryArray_get(CryArray_get(GET_FUNCTION_ARG_VALUE(1).array, 1)->value.array, 0)->value.expression->data->value.string, "1234"       );
        CU_ASSERT_STRING_EQUAL(       CryArray_get(CryArray_get(GET_FUNCTION_ARG_VALUE(2).array, 1)->value.array, 0)->value.expression->data->value.string, "string %40s");
        CU_ASSERT_STRING_EQUAL(       CryArray_get(CryArray_get(GET_FUNCTION_ARG_VALUE(2).array, 1)->value.array, 1)->value.expression->data->value.string, "name"       );
        CU_ASSERT_EQUAL(CryArray_size(CryArray_get(CryArray_get(GET_FUNCTION_ARG_VALUE(3).array, 1)->value.array, 0)->value.expression->data->value.array), 3            );
        CU_ASSERT_EQUAL(CryArray_size(CryArray_get(CryArray_get(GET_FUNCTION_ARG_VALUE(3).array, 1)->value.array, 1)->value.expression->data->value.array), 2            );

        CU_ASSERT_EQUAL(CryArray_get(CryArray_get(CryArray_get(GET_FUNCTION_ARG_VALUE(3).array, 1)->value.array, 0)->value.expression->data->value.array, 0)->value.expression->opcode, EXPR_EVAL  );
        CU_ASSERT_EQUAL(CryArray_get(CryArray_get(CryArray_get(GET_FUNCTION_ARG_VALUE(3).array, 1)->value.array, 0)->value.expression->data->value.array, 1)->value.expression->opcode, EXPR_EVAL  );
        CU_ASSERT_EQUAL(CryArray_get(CryArray_get(CryArray_get(GET_FUNCTION_ARG_VALUE(3).array, 1)->value.array, 0)->value.expression->data->value.array, 2)->value.expression->opcode, EXPR_EVAL  );
        CU_ASSERT_EQUAL(       CryArray_get(CryArray_get(CryArray_get(GET_FUNCTION_ARG_VALUE(3).array, 1)->value.array, 1)->value.expression->data->value.array, 0)->flags,             DATA_STRING);
        CU_ASSERT_STRING_EQUAL(CryArray_get(CryArray_get(CryArray_get(GET_FUNCTION_ARG_VALUE(3).array, 1)->value.array, 1)->value.expression->data->value.array, 0)->value.string,      "key"      );
        CU_ASSERT_EQUAL(CryArray_get(CryArray_get(CryArray_get(GET_FUNCTION_ARG_VALUE(3).array, 1)->value.array, 1)->value.expression->data->value.array, 1)->value.expression->opcode, EXPR_EVAL  );

        CU_ASSERT_EQUAL(CryArray_get(CryArray_get(CryArray_get(GET_FUNCTION_ARG_VALUE(3).array, 1)->value.array, 0)->value.expression->data->value.array, 0)->value.expression->data->flags, DATA_INTEGER);
        CU_ASSERT_EQUAL(CryArray_get(CryArray_get(CryArray_get(GET_FUNCTION_ARG_VALUE(3).array, 1)->value.array, 0)->value.expression->data->value.array, 1)->value.expression->data->flags, DATA_INTEGER);
        CU_ASSERT_EQUAL(CryArray_get(CryArray_get(CryArray_get(GET_FUNCTION_ARG_VALUE(3).array, 1)->value.array, 0)->value.expression->data->value.array, 2)->value.expression->data->flags, DATA_INTEGER);
        CU_ASSERT_EQUAL(CryArray_get(CryArray_get(CryArray_get(GET_FUNCTION_ARG_VALUE(3).array, 1)->value.array, 1)->value.expression->data->value.array, 1)->value.expression->data->flags, DATA_STRING );

        CU_ASSERT_EQUAL(       CryArray_get(CryArray_get(CryArray_get(GET_FUNCTION_ARG_VALUE(3).array, 1)->value.array, 0)->value.expression->data->value.array, 0)->value.expression->data->value.integer, 1      );
        CU_ASSERT_EQUAL(       CryArray_get(CryArray_get(CryArray_get(GET_FUNCTION_ARG_VALUE(3).array, 1)->value.array, 0)->value.expression->data->value.array, 1)->value.expression->data->value.integer, 2      );
        CU_ASSERT_EQUAL(       CryArray_get(CryArray_get(CryArray_get(GET_FUNCTION_ARG_VALUE(3).array, 1)->value.array, 0)->value.expression->data->value.array, 2)->value.expression->data->value.integer, 3      );
        CU_ASSERT_STRING_EQUAL(CryArray_get(CryArray_get(CryArray_get(GET_FUNCTION_ARG_VALUE(3).array, 1)->value.array, 1)->value.expression->data->value.array, 1)->value.expression->data->value.string,  "value");

        attr  = attr->next;
        array = attr->expression->data->value.array;
        CU_ASSERT_FALSE(attr->flags & ATTR_INVISIBLE);
        CU_ASSERT_STRING_EQUAL(attr->namespace,                "nested_arrays");
        CU_ASSERT_STRING_EQUAL(attr->name,                     "func"         );
        CU_ASSERT_EQUAL(attr->expression->opcode,              EXPR_FUNC      );
        CU_ASSERT_EQUAL(attr->expression->data->flags,         DATA_ARRAY     );
        CU_ASSERT_EQUAL(CryArray_size(array),                  2              );
        CU_ASSERT_STRING_EQUAL(GET_FUNCTION_NAME,              "nested"       );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_COUNT,                4              );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(0)->opcode,      EXPR_EVAL      );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(1)->opcode,      EXPR_ARRAY     );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(2)->opcode,      EXPR_ARRAY     );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(3)->opcode,      EXPR_ARRAY     );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(0)->data->flags, DATA_INTEGER   );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(1)->data->flags, DATA_ARRAY     );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(2)->data->flags, DATA_ARRAY     );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(3)->data->flags, DATA_ARRAY     );

        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_VALUE(0).integer, 1);

        CU_ASSERT_EQUAL(CryArray_size(GET_FUNCTION_ARG_VALUE(1).array),                                          1           );
        CU_ASSERT_EQUAL(CryArray_get(GET_FUNCTION_ARG_VALUE(1).array, 0)->flags,                                 DATA_EXPR   );
        CU_ASSERT_EQUAL(CryArray_get(GET_FUNCTION_ARG_VALUE(1).array, 0)->value.expression->opcode,              EXPR_EVAL   );
        CU_ASSERT_EQUAL(CryArray_get(GET_FUNCTION_ARG_VALUE(1).array, 0)->value.expression->data->flags,         DATA_INTEGER);
        CU_ASSERT_EQUAL(CryArray_get(GET_FUNCTION_ARG_VALUE(1).array, 0)->value.expression->data->value.integer, 2           );

        CU_ASSERT_EQUAL(CryArray_size(GET_FUNCTION_ARG_VALUE(2).array),                                          2           );
        CU_ASSERT_EQUAL(CryArray_get(GET_FUNCTION_ARG_VALUE(2).array, 0)->flags,                                 DATA_EXPR   );
        CU_ASSERT_EQUAL(CryArray_get(GET_FUNCTION_ARG_VALUE(2).array, 0)->value.expression->opcode,              EXPR_EVAL   );
        CU_ASSERT_EQUAL(CryArray_get(GET_FUNCTION_ARG_VALUE(2).array, 0)->value.expression->data->flags,         DATA_INTEGER);
        CU_ASSERT_EQUAL(CryArray_get(GET_FUNCTION_ARG_VALUE(2).array, 0)->value.expression->data->value.integer, 3           );
        CU_ASSERT_EQUAL(CryArray_get(GET_FUNCTION_ARG_VALUE(2).array, 1)->flags,                                 DATA_EXPR   );
        CU_ASSERT_EQUAL(CryArray_get(GET_FUNCTION_ARG_VALUE(2).array, 1)->value.expression->opcode,              EXPR_EVAL   );
        CU_ASSERT_EQUAL(CryArray_get(GET_FUNCTION_ARG_VALUE(2).array, 1)->value.expression->data->flags,         DATA_INTEGER);
        CU_ASSERT_EQUAL(CryArray_get(GET_FUNCTION_ARG_VALUE(2).array, 1)->value.expression->data->value.integer, 4           );

        CU_ASSERT_EQUAL(CryArray_size(GET_FUNCTION_ARG_VALUE(3).array),                                                                                                2           );
        CU_ASSERT_EQUAL(CryArray_get(GET_FUNCTION_ARG_VALUE(3).array, 0)->flags,                                                                                       DATA_EXPR   );
        CU_ASSERT_EQUAL(CryArray_get(GET_FUNCTION_ARG_VALUE(3).array, 0)->value.expression->opcode,                                                                    EXPR_EVAL   );
        CU_ASSERT_EQUAL(CryArray_get(GET_FUNCTION_ARG_VALUE(3).array, 0)->value.expression->data->flags,                                                               DATA_INTEGER);
        CU_ASSERT_EQUAL(CryArray_get(GET_FUNCTION_ARG_VALUE(3).array, 0)->value.expression->data->value.integer,                                                       5           );
        CU_ASSERT_EQUAL(CryArray_get(GET_FUNCTION_ARG_VALUE(3).array, 1)->flags,                                                                                       DATA_EXPR   );
        CU_ASSERT_EQUAL(CryArray_get(GET_FUNCTION_ARG_VALUE(3).array, 1)->value.expression->opcode,                                                                    EXPR_ARRAY  );
        CU_ASSERT_EQUAL(CryArray_get(GET_FUNCTION_ARG_VALUE(3).array, 1)->value.expression->data->flags,                                                               DATA_ARRAY  );
        CU_ASSERT_EQUAL(CryArray_size(CryArray_get(GET_FUNCTION_ARG_VALUE(3).array, 1)->value.expression->data->value.array),                                          1           );
        CU_ASSERT_EQUAL(CryArray_get(CryArray_get(GET_FUNCTION_ARG_VALUE(3).array, 1)->value.expression->data->value.array, 0)->flags,                                 DATA_EXPR   );
        CU_ASSERT_EQUAL(CryArray_get(CryArray_get(GET_FUNCTION_ARG_VALUE(3).array, 1)->value.expression->data->value.array, 0)->value.expression->opcode,              EXPR_EVAL   );
        CU_ASSERT_EQUAL(CryArray_get(CryArray_get(GET_FUNCTION_ARG_VALUE(3).array, 1)->value.expression->data->value.array, 0)->value.expression->data->flags,         DATA_INTEGER);
        CU_ASSERT_EQUAL(CryArray_get(CryArray_get(GET_FUNCTION_ARG_VALUE(3).array, 1)->value.expression->data->value.array, 0)->value.expression->data->value.integer, 6           );

        attr  = attr->next;
        array = attr->expression->data->value.array;
        CU_ASSERT(attr->flags & ATTR_INVISIBLE);
        CU_ASSERT_STRING_EQUAL(attr->namespace,                "hashed"    );
        CU_ASSERT_STRING_EQUAL(attr->name,                     "function"  );
        CU_ASSERT_EQUAL(attr->expression->opcode,              EXPR_FUNC   );
        CU_ASSERT_EQUAL(attr->expression->data->flags,         DATA_ARRAY  );
        CU_ASSERT_EQUAL(CryArray_size(array),                  2           );
        CU_ASSERT_STRING_EQUAL(GET_FUNCTION_NAME,              "HASH"      );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_COUNT,                3           );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(0)->opcode,      EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(1)->opcode,      EXPR_HASH   );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(2)->opcode,      EXPR_HASH   );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(0)->data->flags, DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(1)->data->flags, DATA_ARRAY  );
        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_EXPR(2)->data->flags, DATA_ARRAY  );

        CU_ASSERT_EQUAL(GET_FUNCTION_ARG_VALUE(0).integer, 99);

        CU_ASSERT_EQUAL(CryArray_size(GET_FUNCTION_ARG_VALUE(1).array),                                          2           );
        CU_ASSERT_EQUAL(CryArray_get(GET_FUNCTION_ARG_VALUE(1).array, 0)->flags,                                 DATA_STRING );
        CU_ASSERT_STRING_EQUAL(CryArray_get(GET_FUNCTION_ARG_VALUE(1).array, 0)->value.string,                   "key"       );
        CU_ASSERT_EQUAL(CryArray_get(GET_FUNCTION_ARG_VALUE(1).array, 1)->flags,                                 DATA_EXPR   );
        CU_ASSERT_EQUAL(CryArray_get(GET_FUNCTION_ARG_VALUE(1).array, 1)->value.expression->opcode,              EXPR_EVAL   );
        CU_ASSERT_EQUAL(CryArray_get(GET_FUNCTION_ARG_VALUE(1).array, 1)->value.expression->data->flags,         DATA_INTEGER);
        CU_ASSERT_EQUAL(CryArray_get(GET_FUNCTION_ARG_VALUE(1).array, 1)->value.expression->data->value.integer, 123         );

        CU_ASSERT_EQUAL(CryArray_size(GET_FUNCTION_ARG_VALUE(2).array),                                                 4             );
        CU_ASSERT_EQUAL(CryArray_get(GET_FUNCTION_ARG_VALUE(2).array, 0)->flags,                                        DATA_STRING   );
        CU_ASSERT_STRING_EQUAL(CryArray_get(GET_FUNCTION_ARG_VALUE(2).array, 0)->value.string,                          "one"         );
        CU_ASSERT_EQUAL(CryArray_get(GET_FUNCTION_ARG_VALUE(2).array, 1)->flags,                                        DATA_EXPR     );
        CU_ASSERT_EQUAL(CryArray_get(GET_FUNCTION_ARG_VALUE(2).array, 1)->value.expression->opcode,                     EXPR_EVAL     );
        CU_ASSERT_EQUAL(CryArray_get(GET_FUNCTION_ARG_VALUE(2).array, 1)->value.expression->data->flags,                DATA_INTEGER  );
        CU_ASSERT_EQUAL(CryArray_get(GET_FUNCTION_ARG_VALUE(2).array, 1)->value.expression->data->value.integer,        1             );
        CU_ASSERT_EQUAL(CryArray_get(GET_FUNCTION_ARG_VALUE(2).array, 2)->flags,                                        DATA_STRING   );
        CU_ASSERT_STRING_EQUAL(CryArray_get(GET_FUNCTION_ARG_VALUE(2).array, 2)->value.string,                          "two"         );
        CU_ASSERT_EQUAL(CryArray_get(GET_FUNCTION_ARG_VALUE(2).array, 3)->flags,                                        DATA_EXPR     );
        CU_ASSERT_EQUAL(CryArray_get(GET_FUNCTION_ARG_VALUE(2).array, 3)->value.expression->opcode,                     EXPR_EVAL     );
        CU_ASSERT_EQUAL(CryArray_get(GET_FUNCTION_ARG_VALUE(2).array, 3)->value.expression->data->flags,                DATA_DOUBLE   );
        CU_ASSERT_DOUBLE_EQUAL(CryArray_get(GET_FUNCTION_ARG_VALUE(2).array, 3)->value.expression->data->value.cdouble, 2.0, precision);

        CU_ASSERT_FALSE(attr->next);
        CryEnv_cleanup(env);
    );

    free_parser_environment(env);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_native_expressions(void){
    CryEnv      *env;
    CryAttr      attr;
    CryExpr      expr;
    CryArray     array, other_array;
    long double  precision = 0.00000000000001;

    env = init_parser_environment("rb");

    EXECUTE_TWICE(
        Parser_parse(env, "test/examples/native_expressions.cryml", 0);

        CU_ASSERT_EQUAL(env->root_tag->next,                     NULL);
        CU_ASSERT_EQUAL(env->root_tag->first_child->next,        NULL);
        CU_ASSERT_EQUAL(env->root_tag->first_child->first_child, NULL);

        // arithmetic:without_parenthesis = { 1 + 17 * 3 /  7 - 2   % 3 }
        attr  = env->root_tag->first_child->attributes;
        expr  = attr->expression;
        array = expr->data->value.array;
        CU_ASSERT_STRING_EQUAL(attr->namespace, "arithmetic"         );
        CU_ASSERT_STRING_EQUAL(attr->name,      "without_parenthesis");

        CU_ASSERT_EQUAL(expr->opcode,                              EXPR_SUB    );
        CU_ASSERT_EQUAL(CryArray_size(array),                      2           );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,           EXPR_ADD    );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags,      DATA_ARRAY  );
        CU_ASSERT_EQUAL(CryArray_size(GET_ARRAY_ELEMENT(0).array), 2           );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,           EXPR_MOD    );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags,      DATA_ARRAY  );
        CU_ASSERT_EQUAL(CryArray_size(GET_ARRAY_ELEMENT(1).array), 2           );

        other_array = GET_ARRAY_ELEMENT(0).array;
        array       = GET_ARRAY_ELEMENT(1).array;
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,           EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags,      DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(0).integer,              2           );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,           EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags,      DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(1).integer,              3           );

        array = other_array;
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,           EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags,      DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(0).integer,              1           );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,           EXPR_DIV    );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags,      DATA_ARRAY  );
        CU_ASSERT_EQUAL(CryArray_size(GET_ARRAY_ELEMENT(1).array), 2           );

        array = GET_ARRAY_ELEMENT(1).array;
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,           EXPR_MUL    );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags,      DATA_ARRAY  );
        CU_ASSERT_EQUAL(CryArray_size(GET_ARRAY_ELEMENT(0).array), 2           );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,           EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags,      DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(1).integer,              7           );

        array = GET_ARRAY_ELEMENT(0).array;
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,           EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags,      DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(0).integer,              17          );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,           EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags,      DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(1).integer,              3           );

        // arithmetic:with_parenthesis={ 1 + 17 * 3 / (7 - 2)  % 3 }
        attr  = attr->next;
        expr  = attr->expression;
        array = expr->data->value.array;
        CU_ASSERT_STRING_EQUAL(attr->namespace, "arithmetic"      );
        CU_ASSERT_STRING_EQUAL(attr->name,      "with_parenthesis");

        CU_ASSERT_EQUAL(expr->opcode,                              EXPR_ADD    );
        CU_ASSERT_EQUAL(CryArray_size(array),                      2           );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,           EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags,      DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(0).integer,              1           );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,           EXPR_MOD    );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags,      DATA_ARRAY  );
        CU_ASSERT_EQUAL(CryArray_size(GET_ARRAY_ELEMENT(1).array), 2           );

        array = GET_ARRAY_ELEMENT(1).array;
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,           EXPR_DIV    );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags,      DATA_ARRAY  );
        CU_ASSERT_EQUAL(CryArray_size(GET_ARRAY_ELEMENT(0).array), 2           );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,           EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags,      DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(1).integer,              3           );

        array = GET_ARRAY_ELEMENT(0).array;
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,           EXPR_MUL    );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags,      DATA_ARRAY  );
        CU_ASSERT_EQUAL(CryArray_size(GET_ARRAY_ELEMENT(0).array), 2           );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,           EXPR_SUB    );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags,      DATA_ARRAY  );
        CU_ASSERT_EQUAL(CryArray_size(GET_ARRAY_ELEMENT(1).array), 2           );

        other_array = GET_ARRAY_ELEMENT(0).array;
        array       = GET_ARRAY_ELEMENT(1).array;
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,           EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags,      DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(0).integer,              7           );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,           EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags,      DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(1).integer,              2           );

        array = other_array;
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,           EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags,      DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(0).integer,              17          );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,           EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags,      DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(1).integer,              3           );

        // arithmetic:with_unary = { 1 + 17 *-3 /  7 - 2   % 3 }
        attr  = attr->next;
        expr  = attr->expression;
        array = expr->data->value.array;
        CU_ASSERT_STRING_EQUAL(attr->namespace, "arithmetic");
        CU_ASSERT_STRING_EQUAL(attr->name,      "with_unary");

        CU_ASSERT_EQUAL(expr->opcode,                              EXPR_SUB    );
        CU_ASSERT_EQUAL(CryArray_size(array),                      2           );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,           EXPR_ADD    );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags,      DATA_ARRAY  );
        CU_ASSERT_EQUAL(CryArray_size(GET_ARRAY_ELEMENT(0).array), 2           );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,           EXPR_MOD    );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags,      DATA_ARRAY  );
        CU_ASSERT_EQUAL(CryArray_size(GET_ARRAY_ELEMENT(1).array), 2           );

        other_array = GET_ARRAY_ELEMENT(0).array;
        array       = GET_ARRAY_ELEMENT(1).array;
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,           EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags,      DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(0).integer,              2           );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,           EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags,      DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(1).integer,              3           );

        array = other_array;
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,           EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags,      DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(0).integer,              1           );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,           EXPR_DIV    );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags,      DATA_ARRAY  );
        CU_ASSERT_EQUAL(CryArray_size(GET_ARRAY_ELEMENT(1).array), 2           );

        array = GET_ARRAY_ELEMENT(1).array;
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,           EXPR_MUL    );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags,      DATA_ARRAY  );
        CU_ASSERT_EQUAL(CryArray_size(GET_ARRAY_ELEMENT(0).array), 2           );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,           EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags,      DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(1).integer,              7           );

        array = GET_ARRAY_ELEMENT(0).array;
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,                      EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags,                 DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(0).integer,                         17          );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,                      EXPR_MINUS  );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags,                 DATA_EXPR   );
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(1).expression->opcode,              EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(1).expression->data->flags,         DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(1).expression->data->value.integer, 3           );

        // arithmetic:indiferent_unary = { +1 + 17 *+-+-3 /  7 - 2   % 3 }
        attr  = attr->next;
        expr  = attr->expression;
        array = expr->data->value.array;
        CU_ASSERT_STRING_EQUAL(attr->namespace, "arithmetic"      );
        CU_ASSERT_STRING_EQUAL(attr->name,      "indiferent_unary");

        CU_ASSERT_EQUAL(expr->opcode,                              EXPR_SUB    );
        CU_ASSERT_EQUAL(CryArray_size(array),                      2           );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,           EXPR_ADD    );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags,      DATA_ARRAY  );
        CU_ASSERT_EQUAL(CryArray_size(GET_ARRAY_ELEMENT(0).array), 2           );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,           EXPR_MOD    );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags,      DATA_ARRAY  );
        CU_ASSERT_EQUAL(CryArray_size(GET_ARRAY_ELEMENT(1).array), 2           );

        other_array = GET_ARRAY_ELEMENT(0).array;
        array       = GET_ARRAY_ELEMENT(1).array;
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,           EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags,      DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(0).integer,              2           );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,           EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags,      DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(1).integer,              3           );

        array = other_array;
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,           EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags,      DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(0).integer,              1           );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,           EXPR_DIV    );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags,      DATA_ARRAY  );
        CU_ASSERT_EQUAL(CryArray_size(GET_ARRAY_ELEMENT(1).array), 2           );

        array = GET_ARRAY_ELEMENT(1).array;
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,           EXPR_MUL    );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags,      DATA_ARRAY  );
        CU_ASSERT_EQUAL(CryArray_size(GET_ARRAY_ELEMENT(0).array), 2           );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,           EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags,      DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(1).integer,              7           );

        array = GET_ARRAY_ELEMENT(0).array;
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,           EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags,      DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(0).integer,              17          );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,           EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags,      DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(1).integer,              3           );

        // arithmetic:stupid_unary = {  1 + 17 *-+--3 /  7 - +2   % 3 }
        attr  = attr->next;
        expr  = attr->expression;
        array = expr->data->value.array;
        CU_ASSERT_STRING_EQUAL(attr->namespace,"arithmetic"  );
        CU_ASSERT_STRING_EQUAL(attr->name,     "stupid_unary");

        CU_ASSERT_EQUAL(expr->opcode,                              EXPR_SUB    );
        CU_ASSERT_EQUAL(CryArray_size(array),                      2           );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,           EXPR_ADD    );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags,      DATA_ARRAY  );
        CU_ASSERT_EQUAL(CryArray_size(GET_ARRAY_ELEMENT(0).array), 2           );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,           EXPR_MOD    );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags,      DATA_ARRAY  );
        CU_ASSERT_EQUAL(CryArray_size(GET_ARRAY_ELEMENT(1).array), 2           );

        other_array = GET_ARRAY_ELEMENT(0).array;
        array       = GET_ARRAY_ELEMENT(1).array;
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,           EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags,      DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(0).integer,              2           );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,           EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags,      DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(1).integer,              3           );

        array = other_array;
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,           EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags,      DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(0).integer,              1           );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,           EXPR_DIV    );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags,      DATA_ARRAY  );
        CU_ASSERT_EQUAL(CryArray_size(GET_ARRAY_ELEMENT(1).array), 2           );

        array = GET_ARRAY_ELEMENT(1).array;
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,           EXPR_MUL    );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags,      DATA_ARRAY  );
        CU_ASSERT_EQUAL(CryArray_size(GET_ARRAY_ELEMENT(0).array), 2           );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,           EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags,      DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(1).integer,              7           );

        array = GET_ARRAY_ELEMENT(0).array;
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,                      EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags,                 DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(0).integer,                         17          );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,                      EXPR_MINUS  );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags,                 DATA_EXPR   );
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(1).expression->opcode,              EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(1).expression->data->flags,         DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(1).expression->data->value.integer, 3           );

        // comparable:greater = {  123 > 12 + 3 && 1 >= 23 }
        attr  = attr->next;
        expr  = attr->expression;
        array = expr->data->value.array;
        CU_ASSERT_STRING_EQUAL(attr->namespace, "comparable");
        CU_ASSERT_STRING_EQUAL(attr->name,      "greater"   );

        CU_ASSERT_EQUAL(expr->opcode,                              EXPR_AND  );
        CU_ASSERT_EQUAL(CryArray_size(array),                      2         );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,           EXPR_GT   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags,      DATA_ARRAY);
        CU_ASSERT_EQUAL(CryArray_size(GET_ARRAY_ELEMENT(0).array), 2         );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,           EXPR_GE   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags,      DATA_ARRAY);
        CU_ASSERT_EQUAL(CryArray_size(GET_ARRAY_ELEMENT(1).array), 2         );

        other_array = GET_ARRAY_ELEMENT(0).array;
        array       = GET_ARRAY_ELEMENT(1).array;
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,           EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags,      DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(0).integer,              1           );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,           EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags,      DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(1).integer,              23          );

        array = other_array;
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,           EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags,      DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(0).integer,              123         );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,           EXPR_ADD    );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags,      DATA_ARRAY  );
        CU_ASSERT_EQUAL(CryArray_size(GET_ARRAY_ELEMENT(1).array), 2           );

        array = GET_ARRAY_ELEMENT(1).array;
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,           EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags,      DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(0).integer,              12          );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,           EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags,      DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(1).integer,              3           );

        // comparable:less = {  123 < 12 + 3 || 1 <= 23 }
        attr  = attr->next;
        expr  = attr->expression;
        array = expr->data->value.array;
        CU_ASSERT_STRING_EQUAL(attr->namespace, "comparable");
        CU_ASSERT_STRING_EQUAL(attr->name,      "less"      );

        CU_ASSERT_EQUAL(expr->opcode,                              EXPR_OR   );
        CU_ASSERT_EQUAL(CryArray_size(array),                      2         );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,           EXPR_LT   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags,      DATA_ARRAY);
        CU_ASSERT_EQUAL(CryArray_size(GET_ARRAY_ELEMENT(0).array), 2         );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,           EXPR_LE   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags,      DATA_ARRAY);
        CU_ASSERT_EQUAL(CryArray_size(GET_ARRAY_ELEMENT(1).array), 2         );

        other_array = GET_ARRAY_ELEMENT(0).array;
        array       = GET_ARRAY_ELEMENT(1).array;
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,           EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags,      DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(0).integer,              1           );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,           EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags,      DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(1).integer,              23          );

        array = other_array;
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,           EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags,      DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(0).integer,              123         );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,           EXPR_ADD    );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags,      DATA_ARRAY  );
        CU_ASSERT_EQUAL(CryArray_size(GET_ARRAY_ELEMENT(1).array), 2           );

        array = GET_ARRAY_ELEMENT(1).array;
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,           EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags,      DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(0).integer,              12          );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,           EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags,      DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(1).integer,              3           );

        // comparable:equal_diff = {  123 + 2 * 321 ==  -2 - 123 * -321 != (123 + 2) * 321 }
        attr  = attr->next;
        expr  = attr->expression;
        array = expr->data->value.array;
        CU_ASSERT_STRING_EQUAL(attr->namespace, "comparable");
        CU_ASSERT_STRING_EQUAL(attr->name,      "equal_diff");

        CU_ASSERT_EQUAL(expr->opcode,                              EXPR_NE     );
        CU_ASSERT_EQUAL(CryArray_size(array),                      2           );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,           EXPR_EQ     );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags,      DATA_ARRAY  );
        CU_ASSERT_EQUAL(CryArray_size(GET_ARRAY_ELEMENT(0).array), 2           );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,           EXPR_MUL    );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags,      DATA_ARRAY  );
        CU_ASSERT_EQUAL(CryArray_size(GET_ARRAY_ELEMENT(1).array), 2           );

        other_array = GET_ARRAY_ELEMENT(0).array;
        array       = GET_ARRAY_ELEMENT(1).array;
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,           EXPR_ADD    );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags,      DATA_ARRAY  );
        CU_ASSERT_EQUAL(CryArray_size(GET_ARRAY_ELEMENT(0).array), 2           );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,           EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags,      DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(1).integer,              321         );

        array = GET_ARRAY_ELEMENT(0).array;
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,           EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags,      DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(0).integer,              123         );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,           EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags,      DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(1).integer,              2           );

        array = other_array;
        CU_ASSERT_EQUAL(CryArray_size(array),                      2           );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,           EXPR_ADD    );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags,      DATA_ARRAY  );
        CU_ASSERT_EQUAL(CryArray_size(GET_ARRAY_ELEMENT(0).array), 2           );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,           EXPR_SUB    );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags,      DATA_ARRAY  );
        CU_ASSERT_EQUAL(CryArray_size(GET_ARRAY_ELEMENT(1).array), 2           );

        other_array = GET_ARRAY_ELEMENT(1).array;
        array       = GET_ARRAY_ELEMENT(0).array;
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,           EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags,      DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(0).integer,              123         );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,           EXPR_MUL    );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags,      DATA_ARRAY  );
        CU_ASSERT_EQUAL(CryArray_size(GET_ARRAY_ELEMENT(1).array), 2           );

        array = GET_ARRAY_ELEMENT(1).array;
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,           EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags,      DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(0).integer,              2           );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,           EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags,      DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(1).integer,              321         );

        array = other_array;
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,                      EXPR_MINUS  );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags,                 DATA_EXPR   );
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(0).expression->opcode,              EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(0).expression->data->flags,         DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(0).expression->data->value.integer, 2           );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,                      EXPR_MUL    );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags,                 DATA_ARRAY  );
        CU_ASSERT_EQUAL(CryArray_size(GET_ARRAY_ELEMENT(1).array),            2           );

        array = GET_ARRAY_ELEMENT(1).array;
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,                      EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags,                 DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(0).integer,                         123         );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,                      EXPR_MINUS  );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags,                 DATA_EXPR   );
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(1).expression->opcode,              EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(1).expression->data->flags,         DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(1).expression->data->value.integer, 321         );

        // boolean:not_expr = {  true && ! 2 < 1 + -13 }
        attr  = attr->next;
        expr  = attr->expression;
        array = expr->data->value.array;
        CU_ASSERT_STRING_EQUAL(attr->namespace, "boolean" );
        CU_ASSERT_STRING_EQUAL(attr->name,      "not_expr");

        CU_ASSERT_EQUAL(expr->opcode,                                                      EXPR_AND    );
        CU_ASSERT_EQUAL(CryArray_size(array),                                              2           );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,                                   EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags,                              DATA_BOOLEAN);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(0).boolean,                                      TRUE        );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,                                   EXPR_NOT    );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags,                              DATA_EXPR   );
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(1).expression->opcode,                           EXPR_LT     );
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(1).expression->data->flags,                      DATA_ARRAY  );
        CU_ASSERT_EQUAL(CryArray_size(GET_ARRAY_ELEMENT(1).expression->data->value.array), 2           );

        array = GET_ARRAY_ELEMENT(1).expression->data->value.array;
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,           EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags,      DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(0).integer,              2           );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,           EXPR_ADD    );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags,      DATA_ARRAY  );
        CU_ASSERT_EQUAL(CryArray_size(GET_ARRAY_ELEMENT(1).array), 2           );

        array = GET_ARRAY_ELEMENT(1).array;
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->opcode,                      EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(0)->data->flags,                 DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(0).integer,                         1           );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->opcode,                      EXPR_MINUS  );
        CU_ASSERT_EQUAL(GET_ARRAY_EXPRESSION(1)->data->flags,                 DATA_EXPR   );
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(1).expression->opcode,              EXPR_EVAL   );
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(1).expression->data->flags,         DATA_INTEGER);
        CU_ASSERT_EQUAL(GET_ARRAY_ELEMENT(1).expression->data->value.integer, 13          );

        CU_ASSERT_FALSE(attr->next);
        CryEnv_cleanup(env);
    );

    free_parser_environment(env);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

CU_ErrorCode test_parser(CU_pSuite suite){
    suite = CU_add_suite("Parser Tests (test_parser.c)", before_all_test_parser, after_all_test_parser);
    if(suite == NULL){
       CU_cleanup_registry();
       return CU_get_error();
    }

    if( (CU_add_test(suite, "Parse simple text tag        ", test_simple_text_tag         ) == NULL ) ||
        (CU_add_test(suite, "Parse simple open-close tag  ", test_simple_open_close_tag   ) == NULL ) ||
        (CU_add_test(suite, "Parse simple singleton tag   ", test_simple_singleton_tag    ) == NULL ) ||
        (CU_add_test(suite, "Parse simple html            ", test_simple_html             ) == NULL ) ||
        (CU_add_test(suite, "Parse simple attributes      ", test_simple_attributes       ) == NULL ) ||
        (CU_add_test(suite, "Parse unary simple attributes", test_unary_simple_attributes ) == NULL ) ||
        (CU_add_test(suite, "Parse multiple attributes    ", test_multiple_attributes     ) == NULL ) ||
        (CU_add_test(suite, "Parse array attributes       ", test_array_attributes        ) == NULL ) ||
        (CU_add_test(suite, "Parse function attributes    ", test_function_attributes     ) == NULL ) ||
        (CU_add_test(suite, "Parse native expressions     ", test_native_expressions      ) == NULL ) ){
       CU_cleanup_registry();
       return CU_get_error();
    }

    return CUE_SUCCESS;
}