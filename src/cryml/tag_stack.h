// Structs
struct CryTagStack {
    int             flags;
    CryHash         attributes;
    CryStack        attribute_names;
    CryTagAttrList *attr_list;
    CryTagAttrList *last_attr;
    CryTag          tag;
    CryTagStack    *parent;
};

CryTagStack *CryTagStack_init(CryContext *context, CryTag tag);
void         CryTagStack_add_visible_attr(CryTagStack *tag_stack, char *attribute_key, CryData *data);
void         CryTagStack_pop(CryTagStack *tag_stack, CryContext *context);
void         CryTagStack_free(CryTagStack *tag_stack);