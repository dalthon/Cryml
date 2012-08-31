// Structs
struct CryStackData {
    CryData  *data;
    CryStack next;
};

// Function Prototypes
CryStack  CryStack_init();
void      CryStack_push(CryStack stack, CryData *data);
CryData  *CryStack_pop(CryStack stack);
CryData  *CryStack_clean_pop(CryStack stack);
CryData  *CryStack_top(CryStack stack);
CryStack  CryStack_next(CryStack stack);
void      CryStack_free(CryStack stack);