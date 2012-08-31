#include "../cryml.h"

CryAttr CryAttr_init(int flags){
    CryAttr attr;

    attr             = malloc(sizeof(CryAttrData));
    attr->flags      = flags;
    attr->name       = NULL;
    attr->namespace  = NULL;
    attr->expression = NULL;
    attr->next       = NULL;

    return attr;
}

CryAttr CryAttr_clone(CryAttr attr){
    CryAttr cloned_attr;

    cloned_attr = CryAttr_init(attr->flags);

    if(attr->name       != NULL) CryAttr_set_name(cloned_attr, attr->name);
    if(attr->namespace  != NULL) CryAttr_set_namespace(cloned_attr, attr->namespace);
    if(attr->expression != NULL) CryAttr_set_expression(cloned_attr, attr->expression);

    return cloned_attr;
}

void CryAttr_set_name(CryAttr attr, char *name){
    attr->name = strdup(name);
}

void CryAttr_set_namespace(CryAttr attr, char *namespace){
    attr->namespace = strdup(namespace);
}

void CryAttr_set_expression(CryAttr attr, CryExpr expression){
    if(attr->expression != NULL) CryExpr_free(attr->expression);
    attr->expression = CryExpr_clone(expression);
}

char *CryAttr_full_name(CryAttr attr){
    int   full_name_lenght;
    char *full_name;

    if(attr->namespace == NULL){
        full_name = strdup(attr->name);
    }else{
        full_name_lenght = 2+strlen(attr->name)+strlen(attr->namespace);
        full_name        = malloc((full_name_lenght)*sizeof(char));
        sprintf(full_name, "%s:%s", attr->namespace, attr->name);
        full_name[full_name_lenght] = '\0';
    }

    return full_name;
}

void CryAttr_free(CryAttr attr){
    if(attr->name       != NULL) free(attr->name);
    if(attr->namespace  != NULL) free(attr->namespace);
    if(attr->expression != NULL) CryExpr_free(attr->expression);
    if(attr->next       != NULL) CryAttr_free(attr->next);
    free(attr);
}
