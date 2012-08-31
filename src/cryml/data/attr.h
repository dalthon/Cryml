// Masks
#define ATTR_CLEAN       0x0000
#define ATTR_INVISIBLE   0x0001
#define ATTR_PREDICATE   0x0002

// Structs
struct CryAttrData {
    int     flags;
    char    *name;
    char    *namespace;
    CryExpr expression;
    CryAttr next;
};

// Function Prototypes
CryAttr  CryAttr_init(int flags);
CryAttr  CryAttr_clone(CryAttr attr);
void     CryAttr_set_name(CryAttr attr, char *name);
void     CryAttr_set_namespace(CryAttr attr, char *namespace);
void     CryAttr_set_expression(CryAttr attr, CryExpr expression);
char    *CryAttr_full_name(CryAttr attr);
void     CryAttr_free(CryAttr attr);
