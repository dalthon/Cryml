// Structs
struct CryObjectData {
    CryScope *scope;
    void     *object;
};

// Function Prototypes
CryScope *CryObject_scope(CryData *data);
CryObject CryObject_init(CryScope *scope, void *object);
CryData  *CryObject_to_data(CryData *data);
CryData  *CryObject_from_data(CryData *data);
CryData  *CryObject_method_call(CryData *data, CryArray arguments);
void      CryObject_traverse(CryData *data, void *unknown);
void      CryObject_free(CryObject object);
int       CryObject_printable(CryData *data);
int       CryObject_print(CryData *data, FILE *output);