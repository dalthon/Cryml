#include "../cryml.h"

CryScope *CryObject_scope(CryData *data){
    return data->value.object->scope;
}

CryObject CryObject_init(CryScope *scope, void *param){
    CryObject object;

    object         = malloc(sizeof(CryObjectData));
    object->scope  = scope;
    object->object = scope->init(object, param);

    return object;
}

CryData *CryObject_to_data(CryData *data){
    return CryObject_scope(data)->to_data(data);
}

CryData *CryObject_from_data(CryData *data){
    return CryObject_scope(data)->from_data(data);
}

CryData *CryObject_method_call(CryData *data, CryArray arguments){
    return CryObject_scope(data)->method_call(data, arguments);
}

void CryObject_traverse(CryData *data, void *unknown){
    CryObject_scope(data)->traverse(data, unknown);
}

void CryObject_free(CryObject object){
    object->scope->free_data(object);
}

int CryObject_printable(CryData *data){
    return CryObject_scope(data)->printable(data);
}

int CryObject_print(CryData *data, FILE *output){
    if(output){
        return CryObject_scope(data)->print(data, output);
    }else{
        return 0;
    }
}