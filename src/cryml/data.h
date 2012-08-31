#pragma once

// Global Variables
int data_allocated;

// Masks
#define DATA_BOOLEAN_MASK      0x0100
#define DATA_NUMERIC           0x0200
#define DATA_ENUMERABLE        0x0400
#define DATA_IMEDIATE          0x1000
#define DATA_NATIVE            0x2000
#define DATA_EXTERNAL          0x4000
#define DATA_PRINTABLE         0x8000

// Data Types
#define DATA_NULL              0xb000
#define DATA_BOOLEAN           0xb100
#define DATA_INTEGER           0xb201
#define DATA_DOUBLE            0xb602
#define DATA_STRING            0xa000
#define DATA_TEXT              0xa001
#define DATA_TAG               0x2002
#define DATA_ATTR              0x2003
#define DATA_HASH              0xa004
#define DATA_TREE              0x2005
#define DATA_STACK             0xa006
#define DATA_ARRAY             (0xa007 | DATA_ENUMERABLE)
#define DATA_EXPR              0x2008
#define DATA_FUNC_LIB          0x2009
#define DATA_OBJECT            0x4001
#define DATA_ITERATOR          (0x0000 | DATA_ENUMERABLE)

// Structs
union CryDataValue {
    void           *unknown;
    int             boolean;
    long int        integer;
    long double     cdouble;
    char           *string;      // TODO: Missing string's tests!
    CryTag          tag;
    CryAttr         attribute;    // TODO: Missing attr's tests!
    CryText         text;         // TODO: Missing text's tests!
    CryHash         hash;
    CryTree         tree;
    CryStack        stack;
    CryArray        array;
    CryExpr         expression;
    CryFunctionLib *library;
    CryObject       object;
    CryIterable     iterable;
};

struct CryData {
    int          flags;
    int          references;
    CryDataValue value;
};

// Function Prototypes
CryData *CryData_null();
CryData *CryData_boolean(int boolean_value);
CryData *CryData_integer(long int integer_value);
CryData *CryData_double(long double double_value);
CryData *CryData_string(char *string_value);
CryData *CryData_tag(CryTag tag);
CryData *CryData_stack();
CryData *CryData_array(int bucket_size);
CryData *CryData_expression(CryExpr expression);
CryData *CryData_funclib(CryFunctionLib *library);
void     CryData_referenced(CryData *data);
int      CryData_is_true(CryData *data);
CryData *CryData_to_numeric(CryData *data);
int      CryData_printable(CryData *data);
int      CryData_print(CryData *data, FILE *output);
int      CryData_escaped_print(CryData *data, FILE *output);
void     CryData_free(CryData *data);
CryIterable CryData_to_iterable(CryData *data);
