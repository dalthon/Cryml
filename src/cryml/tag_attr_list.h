// Structs
struct CryTagAttrList {
    char           *full_name;
    CryData        *data;
    CryTagAttrList *next;
};

CryTagAttrList *CryTagAttrList_init(char *attribute_key, CryData *data);
void            CryTagAttrList_free(CryTagAttrList *attr);