#include "../cryml.h"

CryStack CryStack_init(){
    CryStack stack;

    stack       = malloc(sizeof(CryStackData));
    stack->data = NULL;
    stack->next = NULL;

    return stack;
}

void CryStack_push(CryStack stack, CryData *data){
    CryStack new_stack_element;

    new_stack_element       = malloc(sizeof(CryStackData));
    new_stack_element->data = data;
    new_stack_element->next = stack->next;
    stack->next             = new_stack_element;
    CryData_referenced(data);
}

// Must free data!
CryData *CryStack_pop(CryStack stack){
    CryData  *data;

    if(stack->next == NULL) return NULL;

    data = CryStack_clean_pop(stack);
    data->references--;
    return data;
}

CryData *CryStack_clean_pop(CryStack stack){
    CryStack popping_element;
    CryData  *data;

    if(stack->next == NULL) return NULL;

    popping_element = stack->next;
    stack->next     = popping_element->next;
    data            = popping_element->data;

    free(popping_element);
    return data;
}

CryData *CryStack_top(CryStack stack){
    if(stack->next == NULL) return NULL;
    return stack->next->data;
}

CryStack CryStack_next(CryStack stack){
    return stack->next;
}

void CryStack_free(CryStack stack){
    if(stack != NULL){
        if(stack->data != NULL) CryData_free(stack->data);
        CryStack_free(stack->next);
        free(stack);
    }
}