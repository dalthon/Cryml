// Structs
struct CryContext {
    int          flags;
    char         error_message[256];
    CryEnv      *environment;
    CryHash      data;
    CryHash      attributes;
    CryHash      extra;
    CryTagStack *tag_stack;
};

CryContext  *CryContext_init(CryEnv *env, CryHash data, CryHash extra);
void         CryContext_init_stack(CryContext *context, CryTag tag);
CryTagStack *CryContext_stack(CryContext *context);
CryData     *CryContext_current_attr(CryContext *context, char *attr_name);
int          CryContext_error(CryContext *context);
void         CryContext_raise(CryContext *context, int error_flag, char *buff, ...);
void         CryContext_push_attribute(CryContext *context, char *attribute_key, CryData *data);
void         CryContext_pop_attribute(CryContext *context, char *attribute_key);
void         CryContext_add_visible_attr(CryContext *context, char *attribute_key, CryData *data);
void         CryContext_pop_stack(CryContext *context);
CryData     *CryContext_data_lookup(CryContext *context, char *attribute_key);
CryData     *CryContext_attribute_lookup(CryContext *context, char *attribute_key);
CryData     *CryContext_tag_attribute_lookup(CryContext *context, char *attribute_key);
void         CryContext_free(CryContext *context);
void         CryContext_cleanup(CryContext *context);