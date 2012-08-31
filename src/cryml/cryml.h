#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>
#include <math.h>
#include <pthread.h>

typedef struct CryParseContext CryParseContext;

// Cryml Struct Prototypes
// env.h
typedef struct CryLexStateStack CryLexStateStack;
typedef struct CryLexStateStack *CryLexState;
typedef struct CryEnv CryEnv;

// scope.h
typedef struct CryScope CryScope;

// context.h
typedef struct CryContext CryContext;

// tag_stack.h
typedef struct CryTagStack CryTagStack;

// tag_attr_list.h
typedef struct CryTagAttrList CryTagAttrList;

// function_lib.h
typedef struct CryFunctionLib CryFunctionLib;

// data.h
typedef union  CryDataValue CryDataValue;
typedef struct CryData CryData;

// data/tag.h
typedef struct CryTagNode CryTagNode;
typedef struct CryTagNode *CryTag;

// data/attr.h
typedef struct CryAttrData CryAttrData;
typedef struct CryAttrData *CryAttr;

// data/expr.h
typedef struct CryExprData CryExprData;
typedef struct CryExprData *CryExpr;

// data/hash.h
typedef struct CryHashData CryHashData;
typedef struct CryHashData *CryHash;

// data/text.h
typedef struct CryTextElement CryTextElement;
typedef struct CryTextData CryTextData;
typedef struct CryTextData *CryText;

// data/tree.h
typedef struct CryTreeNode CryTreeNode;
typedef struct CryTreeNode *CryTree;

// datat/array.h
typedef struct CryArrayData CryArrayData;
typedef struct CryArrayData *CryArray;

typedef struct CryArrayIteratorData CryArrayIteratorData;
typedef struct CryArrayIteratorData *CryArrayIterator;

// data/stack.h
typedef struct CryStackData CryStackData;
typedef struct CryStackData *CryStack;

// data/object.h
typedef struct CryObjectData CryObjectData;
typedef struct CryObjectData *CryObject;

// data/iterable.h
typedef struct CryIterableData CryIterableData;
typedef struct CryIterableData *CryIterable;

// data/iterator.h
typedef struct CryIteratorData CryIteratorData;
typedef struct CryIteratorData *CryIterator;

// Function Pointers
typedef CryData *(*CryFunction)(CryContext *, CryData *, CryData *);
typedef int      (*CryFunctionLookupCallback)(CryExpr, CryEnv *, CryExpr);
typedef int      (*CryFunctionLookup)(CryEnv *, CryExpr);
typedef int      (*CryTagFunction)(CryTag, CryTag, CryContext *, FILE *);
typedef void     (*CryTagLookupCallback)(CryTag, CryEnv *, CryExpr);
typedef int      (*CryTagLookup)(CryEnv *, CryTag);
typedef CryIterator (*CryIterableStart)(CryIterable);
typedef CryData *(*CryIteratorNext)(CryIterator);
typedef void     (*CryIteratorFree)(CryIterator);

typedef CryData *(*CrymlScopedDataConversionCallback)(CryData *);
typedef CryData *(*CrymlScopedEvalCallback)(char *);
typedef CryData *(*CrymlScopedMethodCallback)(CryData *, CryArray);
typedef void    *(*CrymlScopedInitCallback)(CryObject, void *);
typedef void     (*CrymlScopedTraverseCallback)(CryData *, void *);
typedef void     (*CrymlScopedFreeCallback)(CryObject);
typedef int      (*CrymlScopedCheckPrintableCallback)(CryData *);
typedef int      (*CrymlScopedPrintCallback)(CryData *, FILE *);


// Cryml General Definitions
#define TRUE  1
#define FALSE 0

#include "env.h"
#include "util.h"
#include "data.h"
#include "lexer.h"
#include "scope.h"
#include "parser.h"
#include "render.h"
#include "context.h"
#include "tag_stack.h"
#include "function_lib.h"
#include "tag_attr_list.h"
#include "data/tag.h"
#include "data/attr.h"
#include "data/expr.h"
#include "data/hash.h"
#include "data/text.h"
#include "data/tree.h"
#include "data/array.h"
#include "data/iterable.h"
#include "data/iterator.h"
#include "data/stack.h"
#include "data/object.h"
#include "data/string.h"
#include "stdlib/core.h"

/* Error Codes */
#define ERROR_LEXICAL                   0x1001
#define ERROR_SYNTACTIC                 0x1002

#define ERROR_UNPRINTABLE               0x2001
#define ERROR_UNKNOWN_DATA_TYPE         0x2002
#define ERROR_EXECUTION_ERROR           0x2003

#define ERROR_MISSING_ATTRIBUTE         0x4001
#define ERROR_MISSING_DATA_ATTRIBUTE    0x4002
#define ERROR_DIVISION_BY_ZERO          0x4003
#define ERROR_ARGUMMENT_ERROR           0x4004
#define ERROR_UNKNOWN_RENDERING         0x4FFF

#define ERROR_UNKNOWN_ERROR             0x8FFF

