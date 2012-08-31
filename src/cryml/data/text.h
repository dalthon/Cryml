// Text's Flags
#define TEXT_NON_BLANK       0x0000
#define TEXT_BLANK           0x0001

// Structs
struct CryTextData {
    int            flags;
    CryTextElement *first_element;
    CryTextElement *last_element;
};

struct CryTextElement {
    int            flags;
    char           *text;
    CryTextElement *next;
};

// Function Prototypes
CryText         CryText_init();
CryTextElement *CryTextElement_init(char *string, int flags);
void            CryText_append(CryText text, char *string, int flags);
char           *CryText_to_string(CryText text);
int             CryText_print(CryText text, FILE *output, int flags);
void            CryTextElement_free(CryTextElement *text_element);
void            CryText_free(CryText text);
