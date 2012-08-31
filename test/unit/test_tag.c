#include "../../src/cryml/cryml.h"
#include "unit.h"

#define EXECUTE_TWICE(x) x ; x
/* I'm not as stupid as it sounds...          */
/* I'm evaluating twice to ensure that each   */
/* expression can be evaluated more than once */

int before_all_test_tag(void){ return 0; }
int after_all_test_tag(void){  return 0; }

CryTag init_sample_tag(char *tag_namespace, char *tag_name){
    CryTag tag;

    tag = CryTag_init();

    if(tag_namespace != NULL) CryTag_set_namespace(tag, tag_namespace);
    if(tag_name      != NULL) CryTag_set_name(tag, tag_name);

    CryTag_set_flag(tag, TAG_CLEAN);

    tag->eval               = CryExpr_init(EXPR_ENV, NULL);
    tag->eval->tag_function = &CryRender_tag;

    return tag;
}

CryTag init_sample_text_tag(int flags){
    CryTag tag;

    tag        = CryTag_init();
    tag->flags = TAG_TEXT | flags;
    tag->text  = CryText_init();

    tag->eval               = CryExpr_init(EXPR_ENV, NULL);
    tag->eval->tag_function = &CryRender_text_tag;

    return tag;
}

CryAttr init_sample_attr(int flags, char *attr_namespace, char *attr_name, CryData *data){
    CryAttr attr;

    attr = CryAttr_init(flags);
    if(attr_namespace != NULL) CryAttr_set_namespace(attr, attr_namespace);
    if(attr_name      != NULL) CryAttr_set_name(attr, attr_name);

    attr->expression = CryExpr_init(EXPR_EVAL, data);

    return attr;
}

char *render_tag(CryTag tag, CryContext *context, int string_size){
    int    file_descriptors[2];
    FILE   *input, *output;
    char   *string;

    string = malloc(string_size*sizeof(char));

    pipe(file_descriptors);
    input  = fdopen(file_descriptors[0], "r");
    output = fdopen(file_descriptors[1], "w");

    CryRender_render_tag(NULL, tag, context, output);
    fclose(output);
    close(file_descriptors[1]);

    fgets(string, string_size, input);
    fclose(input);
    close(file_descriptors[0]);


    return string;
}

void test_render_simple_tag(void){
    char       *string;
    CryEnv     *env;
    CryTag      tag;
    CryContext *context;
    CryHash     ctx_data, ctx_extra;

    env       = CryEnv_init();
    ctx_data  = CryHash_init(3);
    ctx_extra = CryHash_init(3);
    context   = CryContext_init(env, ctx_data, ctx_extra);
    tag       = init_sample_tag("html", "div");

    string = CryTag_full_name(tag);
    CU_ASSERT_STRING_EQUAL(string, "html:div");
    free(string);

    EXECUTE_TWICE(
        string = render_tag(tag, context, 50);
        CU_ASSERT_STRING_EQUAL(string, "<html:div></html:div>");
        free(string);
    );

    CryHash_free(ctx_data);
    CryHash_free(ctx_extra);
    CryContext_free(context);
    CryEnv_free(env);
    CryTag_free(tag);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_render_simple_tag_with_inner_tags(void){
    char       *string;
    CryEnv     *env;
    CryTag      tag, aux_tag;
    CryContext *context;
    CryHash     ctx_data, ctx_extra;

    env              = CryEnv_init();
    ctx_data         = CryHash_init(3);
    ctx_extra        = CryHash_init(3);
    context          = CryContext_init(env, ctx_data, ctx_extra);
    tag              = init_sample_tag("html", "div");
    tag->first_child = init_sample_tag(NULL, "p");

    EXECUTE_TWICE(
        string = CryTag_full_name(tag);
        CU_ASSERT_STRING_EQUAL(string, "html:div");
        free(string);
    );

    EXECUTE_TWICE(
        string = CryTag_full_name(tag->first_child);
        CU_ASSERT_STRING_EQUAL(string, "p");
        free(string);
    );

    EXECUTE_TWICE(
        string = render_tag(tag, context, 50);
        CU_ASSERT_STRING_EQUAL(string, "<html:div><p></p></html:div>");
        free(string);
    );

    aux_tag       = tag->first_child;
    aux_tag->next = init_sample_tag(NULL, "b");

    EXECUTE_TWICE(
        string = render_tag(tag, context, 50);
        CU_ASSERT_STRING_EQUAL(string, "<html:div><p></p><b></b></html:div>");
        free(string);
    );

    aux_tag       = aux_tag->next;
    aux_tag->next = init_sample_tag(NULL, "br");
    CryTag_set_flag(aux_tag->next, TAG_SINGLETON);

    EXECUTE_TWICE(
        string = render_tag(tag, context, 50);
        CU_ASSERT_STRING_EQUAL(string, "<html:div><p></p><b></b><br/></html:div>");
        free(string);
    );

    aux_tag = init_sample_text_tag(0);
    CryText_append(aux_tag->text, "Text!", TEXT_NON_BLANK);
    tag->first_child->first_child = aux_tag;

    EXECUTE_TWICE(
        string = render_tag(tag, context, 100);
        CU_ASSERT_STRING_EQUAL(string, "<html:div><p>Text!</p><b></b><br/></html:div>");
        free(string);
    );

    CryHash_free(ctx_data);
    CryHash_free(ctx_extra);
    CryContext_free(context);
    CryEnv_free(env);
    CryTag_free(tag);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_render_simple_tag_with_inner_text(void){
    char       *string;
    CryTag      tag;
    CryEnv     *env;
    CryContext *context;
    CryHash     ctx_data, ctx_extra;

    env       = CryEnv_init();
    ctx_data  = CryHash_init(3);
    ctx_extra = CryHash_init(3);
    context   = CryContext_init(env, ctx_data, ctx_extra);
    tag       = init_sample_tag("html", "div");

    tag->first_child = init_sample_text_tag(0);

    EXECUTE_TWICE(
        string = CryTag_full_name(tag);
        CU_ASSERT_STRING_EQUAL(string, "html:div");
        free(string);
    );

    EXECUTE_TWICE(
        string = render_tag(tag, context, 50);
        CU_ASSERT_STRING_EQUAL(string, "<html:div></html:div>");
        free(string);
    );

    CryText_append(tag->first_child->text, "Test", TEXT_NON_BLANK);
    EXECUTE_TWICE(
        string = render_tag(tag, context, 50);
        CU_ASSERT_STRING_EQUAL(string, "<html:div>Test</html:div>");
        free(string);
    );

    CryText_append(tag->first_child->text, "   ", TEXT_BLANK);
    EXECUTE_TWICE(
        string = render_tag(tag, context, 50);
        CU_ASSERT_STRING_EQUAL(string, "<html:div>Test </html:div>");
        free(string);
    );

    CryTag_set_flag(tag->first_child, TAG_UNCOMPRESSED_TEXT);
    EXECUTE_TWICE(
        string = render_tag(tag, context, 50);
        CU_ASSERT_STRING_EQUAL(string, "<html:div>Test   </html:div>");
        free(string);
    );

    CryText_append(tag->first_child->text, "Text", TEXT_NON_BLANK);
    EXECUTE_TWICE(
        string = render_tag(tag, context, 50);
        CU_ASSERT_STRING_EQUAL(string, "<html:div>Test   Text</html:div>");
        free(string);
    );

    CryTag_remove_flag(tag->first_child, TAG_UNCOMPRESSED_TEXT);
    EXECUTE_TWICE(
        string = render_tag(tag, context, 50);
        CU_ASSERT_STRING_EQUAL(string, "<html:div>Test Text</html:div>");
        free(string);
    );

    CryHash_free(ctx_data);
    CryHash_free(ctx_extra);
    CryContext_free(context);
    CryEnv_free(env);
    CryTag_free(tag);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_render_simple_tag_with_attributes(void){
    char       *string;
    CryTag      tag;
    CryEnv     *env;
    CryContext *context;
    CryHash     ctx_data, ctx_extra;

    env       = CryEnv_init();
    ctx_data  = CryHash_init(3);
    ctx_extra = CryHash_init(3);
    context   = CryContext_init(env, ctx_data, ctx_extra);
    tag       = init_sample_tag(NULL, "p");

    tag->attributes = init_sample_attr(0, NULL, "class", CryData_string("attr_value"));
    EXECUTE_TWICE(
        string = render_tag(tag, context, 50);
        CU_ASSERT_STRING_EQUAL(string, "<p class=\"attr_value\"></p>")
        free(string);
    );

    tag->attributes->next = init_sample_attr(0, NULL, "id", CryData_integer(42));
    EXECUTE_TWICE(
        string = render_tag(tag, context, 50);
        CU_ASSERT_STRING_EQUAL(string, "<p class=\"attr_value\" id=\"42\"></p>");
        free(string);
    );

    tag->attributes->next->next = init_sample_attr(ATTR_INVISIBLE, NULL, "hidden", CryData_string("can't see me!"));
    EXECUTE_TWICE(
        string = render_tag(tag, context, 50);
        CU_ASSERT_STRING_EQUAL(string, "<p class=\"attr_value\" id=\"42\"></p>");
        free(string);
    );

    tag->attributes->next->next->next = init_sample_attr(0, NULL, "escaped", CryData_string("escaped \"value\""));
    EXECUTE_TWICE(
        string = render_tag(tag, context, 70);
        CU_ASSERT_STRING_EQUAL(string, "<p class=\"attr_value\" id=\"42\" escaped=\"escaped \\\"value\\\"\"></p>");
        free(string);
    );

    CryHash_free(ctx_data);
    CryHash_free(ctx_extra);
    CryContext_free(context);
    CryEnv_free(env);
    CryTag_free(tag);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_merge_same_default_attributes(void){
    CryTag  tag,  default_tag;
    CryAttr attr, default_attr;

    tag         = init_sample_tag(NULL, "p");
    default_tag = init_sample_tag(NULL, "p");

    attr            = init_sample_attr(0, NULL, "class", CryData_string("CLASS!"));
    tag->attributes = attr;
    attr->next      = init_sample_attr(0, "html", "style", CryData_string("STYLE!"));
    attr            = attr->next;
    attr->next      = init_sample_attr(0, NULL, "id", CryData_integer(1));

    default_attr            = init_sample_attr(0, NULL, "class", CryData_string("default_class"));
    default_tag->attributes = default_attr;
    default_attr->next      = init_sample_attr(0, "html", "style", CryData_string("default_style"));
    default_attr            = default_attr->next;
    default_attr->next      = init_sample_attr(0, NULL, "id", CryData_integer(42));

    CryTag_merge_default_attributes_and_predicates(tag, default_tag);

    attr = tag->attributes;
    CU_ASSERT(attr != NULL);
    CU_ASSERT_EQUAL(attr->namespace, NULL);
    CU_ASSERT_STRING_EQUAL(attr->name, "class");
    CU_ASSERT_STRING_EQUAL(attr->expression->data->value.string, "CLASS!");
    attr = attr->next;

    CU_ASSERT(attr != NULL);
    CU_ASSERT_STRING_EQUAL(attr->namespace, "html");
    CU_ASSERT_STRING_EQUAL(attr->name, "style");
    CU_ASSERT_STRING_EQUAL(attr->expression->data->value.string, "STYLE!");
    attr = attr->next;

    CU_ASSERT(attr != NULL);
    CU_ASSERT_EQUAL(attr->namespace, NULL);
    CU_ASSERT_STRING_EQUAL(attr->name, "id");
    CU_ASSERT_EQUAL(attr->expression->data->value.integer, 1);
    CU_ASSERT_EQUAL(attr->next, NULL);

    CryTag_free(tag);
    CryTag_free(default_tag);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_merge_empty_default_attributes(void){
    CryTag  tag,  default_tag;
    CryAttr attr, default_attr;

    tag         = init_sample_tag(NULL, "p");
    default_tag = init_sample_tag(NULL, "p");

    attr            = init_sample_attr(0, NULL, "class", CryData_string("CLASS!"));
    tag->attributes = attr;
    attr->next      = init_sample_attr(0, "html", "style", CryData_string("STYLE!"));
    attr            = attr->next;
    attr->next      = init_sample_attr(0, NULL, "id", CryData_integer(1));

    CryTag_merge_default_attributes_and_predicates(tag, default_tag);

    attr = tag->attributes;
    CU_ASSERT(attr != NULL);
    CU_ASSERT_EQUAL(attr->namespace, NULL);
    CU_ASSERT_STRING_EQUAL(attr->name, "class");
    CU_ASSERT_STRING_EQUAL(attr->expression->data->value.string, "CLASS!");
    attr = attr->next;

    CU_ASSERT(attr != NULL);
    CU_ASSERT_STRING_EQUAL(attr->namespace, "html");
    CU_ASSERT_STRING_EQUAL(attr->name, "style");
    CU_ASSERT_STRING_EQUAL(attr->expression->data->value.string, "STYLE!");
    attr = attr->next;

    CU_ASSERT(attr != NULL);
    CU_ASSERT_EQUAL(attr->namespace, NULL);
    CU_ASSERT_STRING_EQUAL(attr->name, "id");
    CU_ASSERT_EQUAL(attr->expression->data->value.integer, 1);
    CU_ASSERT_EQUAL(attr->next, NULL);

    CryTag_free(tag);
    CryTag_free(default_tag);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_merge_default_empty_attributes(void){
    CryTag  tag,  default_tag;
    CryAttr attr, default_attr;

    tag         = init_sample_tag(NULL, "p");
    default_tag = init_sample_tag(NULL, "p");

    default_attr            = init_sample_attr(0, NULL, "class", CryData_string("default_class"));
    default_tag->attributes = default_attr;
    default_attr->next      = init_sample_attr(0, "html", "style", CryData_string("default_style"));
    default_attr            = default_attr->next;
    default_attr->next      = init_sample_attr(0, NULL, "id", CryData_integer(42));

    CryTag_merge_default_attributes_and_predicates(tag, default_tag);

    attr = tag->attributes;
    CU_ASSERT(attr != NULL);
    CU_ASSERT_EQUAL(attr->namespace, NULL);
    CU_ASSERT_STRING_EQUAL(attr->name, "class");
    CU_ASSERT_STRING_EQUAL(attr->expression->data->value.string, "default_class");
    attr = attr->next;

    CU_ASSERT(attr != NULL);
    CU_ASSERT_STRING_EQUAL(attr->namespace, "html");
    CU_ASSERT_STRING_EQUAL(attr->name, "style");
    CU_ASSERT_STRING_EQUAL(attr->expression->data->value.string, "default_style");
    attr = attr->next;

    CU_ASSERT(attr != NULL);
    CU_ASSERT_EQUAL(attr->namespace, NULL);
    CU_ASSERT_STRING_EQUAL(attr->name, "id");
    CU_ASSERT_EQUAL(attr->expression->data->value.integer, 42);
    CU_ASSERT_EQUAL(attr->next, NULL);

    CryTag_free(tag);
    CryTag_free(default_tag);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

void test_merge_some_default_attributes(void){
    CryTag  tag,  default_tag;
    CryAttr attr, default_attr;

    tag         = init_sample_tag(NULL, "p");
    default_tag = init_sample_tag(NULL, "p");

    attr            = init_sample_attr(0, NULL, "class", CryData_string("CLASS!"));
    tag->attributes = attr;
    attr->next      = init_sample_attr(0, "html", "style", CryData_string("STYLE!"));

    default_attr            = init_sample_attr(0, "html", "style", CryData_string("default_style"));
    default_tag->attributes = default_attr;
    default_attr->next      = init_sample_attr(0, NULL, "id", CryData_integer(42));

    CryTag_merge_default_attributes_and_predicates(tag, default_tag);

    attr = tag->attributes;
    CU_ASSERT(attr != NULL);
    CU_ASSERT_EQUAL(attr->namespace, NULL);
    CU_ASSERT_STRING_EQUAL(attr->name, "class");
    CU_ASSERT_STRING_EQUAL(attr->expression->data->value.string, "CLASS!");
    attr = attr->next;

    CU_ASSERT(attr != NULL);
    CU_ASSERT_STRING_EQUAL(attr->namespace, "html");
    CU_ASSERT_STRING_EQUAL(attr->name, "style");
    CU_ASSERT_STRING_EQUAL(attr->expression->data->value.string, "STYLE!");
    attr = attr->next;

    CU_ASSERT(attr != NULL);
    CU_ASSERT_EQUAL(attr->namespace, NULL);
    CU_ASSERT_STRING_EQUAL(attr->name, "id");
    CU_ASSERT_EQUAL(attr->expression->data->value.integer, 42);
    CU_ASSERT_EQUAL(attr->next, NULL);

    CryTag_free(tag);
    CryTag_free(default_tag);
    CU_ASSERT_EQUAL(data_allocated, 0);
}

CU_ErrorCode test_tag(CU_pSuite suite){
    suite = CU_add_suite("CryTag Tests (test_tag.c)", before_all_test_tag, after_all_test_tag);
    if(suite == NULL){
       CU_cleanup_registry();
       return CU_get_error();
    }

    if( (CU_add_test(suite, "Render simple tag                ", test_render_simple_tag                ) == NULL ) ||
        (CU_add_test(suite, "Render simple tag with inner tag ", test_render_simple_tag_with_inner_tags) == NULL ) ||
        (CU_add_test(suite, "Render simple tag with inner text", test_render_simple_tag_with_inner_text) == NULL ) ||
        (CU_add_test(suite, "Render simple tag with attributes", test_render_simple_tag_with_attributes) == NULL ) ||
        (CU_add_test(suite, "Merge same default attributes    ", test_merge_same_default_attributes    ) == NULL ) ||
        (CU_add_test(suite, "Merge empty default attributes   ", test_merge_empty_default_attributes   ) == NULL ) ||
        (CU_add_test(suite, "Merge default empty attributes   ", test_merge_default_empty_attributes   ) == NULL ) ||
        (CU_add_test(suite, "Merge some default attributes    ", test_merge_some_default_attributes    ) == NULL ) ){
       CU_cleanup_registry();
       return CU_get_error();
    }

    return CUE_SUCCESS;
}