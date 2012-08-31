#include "../cryml.h"

// TODO: Missing text's tests!

CryText CryText_init(){
    CryText text;

    text                = malloc(sizeof(CryTextData));
    text->first_element = NULL;
    text->last_element  = NULL;

    return text;
}

CryTextElement *CryTextElement_init(char *string, int flags){
    CryTextElement *text_element;

    text_element        = malloc(sizeof(CryTextElement));
    text_element->text  = strdup(string);
    text_element->next  = NULL;
    text_element->flags = flags;

    return text_element;
}

void CryText_append(CryText text, char *string, int flags){
    CryTextElement *text_element;

    text_element = CryTextElement_init(string, flags);

    if(text->first_element != NULL){
        text->last_element->next = text_element;
    }else{
        text->first_element = text_element;
    }

    text->last_element = text_element;
}

char *CryText_to_string(CryText text){
    int             length;
    char           *string;
    CryTextElement *text_element;

    length = 1;
    for(text_element = text->first_element; text_element != NULL; text_element = text_element->next){
        length += strlen(text_element->text);
    }

    string    = malloc(length*sizeof(char));
    string[0] = '\0';
    length    = 0;
    for(text_element = text->first_element; text_element != NULL; text_element = text_element->next){
        strcpy(&(string[length]), text_element->text);
        length += strlen(text_element->text);
    }
    string[length] = '\0';

    return string;
}

int CryText_print(CryText text, FILE *output, int flags){
    CryTextElement *text_element;

    for(text_element = text->first_element; text_element != NULL; text_element = text_element->next){
        if(!(flags & TAG_UNCOMPRESSED_TEXT) && (text_element->flags & TEXT_BLANK) ){
            fprintf(output, " ");
        }else{
            fprintf(output, "%s", text_element->text);
        }
    }
    return 0;
}

void CryTextElement_free(CryTextElement *text_element){
    if(text_element != NULL){
        CryTextElement_free(text_element->next);
        if(text_element->text != NULL) free(text_element->text);
        free(text_element);
    }
}

void CryText_free(CryText text){
    CryTextElement_free(text->first_element);
    free(text);
}