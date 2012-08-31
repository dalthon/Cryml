// Constants
#define TAG_EVALUATED_ATTR_BUCKET_SIZE 5

// Tag's Flags
#define TAG_CLEAN                0x0000
#define TAG_SINGLETON            0x0001
#define TAG_PRE_RENDER           0x0002
#define TAG_ATTR_TAG             0x0004
#define TAG_DATA_TAG             0x0008

#define TAG_TEXT                 0x0100
#define TAG_UNCOMPRESSED_TEXT    0x0200
#define TAG_UNPARSED_TEXT        0x0400

// Structs
struct CryTagNode {
    int     flags;
    char    *namespace;
    char    *name;
    CryAttr attributes;
    CryText text;
    CryTag  first_child;
    CryTag  next;
    CryExpr eval;
};

// Function Prototypes
CryTag CryTag_init();
void   CryTag_set_flag(CryTag tag, int flag);
void   CryTag_remove_flag(CryTag tag, int flag);
void   CryTag_set_namespace(CryTag tag, char *namespace);
void   CryTag_set_name(CryTag tag, char *name);
char  *CryTag_full_name(CryTag tag);
int    CryTag_eval_tag_function(CryTag parent_tag, CryTag tag, CryContext *context, FILE * output);
void   CryTag_merge_default_attributes_and_predicates(CryTag tag, CryTag definee_tag);
void   CryTag_default_tag_lookup_callback(CryTag tag, CryEnv *env, CryExpr expression);
void   CryTag_free(CryTag tag);