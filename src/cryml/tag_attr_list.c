#include "cryml.h"

CryTagAttrList *CryTagAttrList_init(char *attribute_key, CryData *data){
    CryTagAttrList *attr;

    attr            = malloc(sizeof(CryTagAttrList));
    attr->full_name = strdup(attribute_key);
    attr->data      = data;
    attr->next      = NULL;

    CryData_referenced(data);

    return attr;
}

void CryTagAttrList_free(CryTagAttrList *attr){
    if(attr->next != NULL) CryTagAttrList_free(attr->next);
    CryData_free(attr->data);
    free(attr->full_name);
    free(attr);
}