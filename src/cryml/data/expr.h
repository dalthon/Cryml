// OpCode Masks
#define EXPR_DATA_MASK        0x0001
#define EXPR_NON_DATA_MASK    0xFFF0
#define EXPR_ATTR_MASK        0x0010
#define EXPR_ARITH_MASK       0x0020
#define EXPR_BOOL_MASK        0x0040
#define EXPR_COMPARE_MASK     0x0080
#define EXPR_ARRAY_MASK       0x0100
#define EXPR_HASH_MASK        0x0200
#define EXPR_SCOPED_MASK      0x0400
#define EXPR_FUNC_MASK        0x0800
#define EXPR_CUSTOM_MASK      0x1000
#define EXPR_METHOD_MASK      0x2000
#define EXPR_ENV_MASK         0x8000

// OpCode Definitions
#define EXPR_EVAL             0x0001

#define EXPR_ATTR             0x0010
#define EXPR_DATA             0x0011

#define EXPR_ADD              0x0020
#define EXPR_SUB              0x0021
#define EXPR_MUL              0x0022
#define EXPR_DIV              0x0023
#define EXPR_MOD              0x0024
#define EXPR_MINUS            0x0025

#define EXPR_NOT              0x0040
#define EXPR_AND              0x0041
#define EXPR_OR               0x0042

#define EXPR_EQ               0x0080
#define EXPR_NE               0x0081
#define EXPR_LT               0x0082
#define EXPR_LE               0x0083
#define EXPR_GT               0x0084
#define EXPR_GE               0x0085

#define EXPR_ARRAY            0x0100

#define EXPR_HASH             0x0200

#define EXPR_SCOPED_CALL      0x0400

#define EXPR_FUNC             0x0800

#define EXPR_INDEX            0x1000
#define EXPR_INTER            0x1001

#define EXPR_METHOD           0x2000
#define EXPR_METHOD_ARG       0x2001
#define EXPR_METHOD_CALL      0x2002

#define EXPR_ENV              0x8000

// TODO: Think about grouping function and tag pointer functions
//       to structs on unions. As it is, we spending so much
//       unnecessary memory on expression that have NULL at all
//       of those attributes.
//       This is a major refactor, so we must think before do it.
// Structs
struct CryExprData {
    int                        opcode;
    CryFunctionLookupCallback  function_lookup_callback;
    CryTagLookupCallback       tag_lookup_callback;
    CryFunctionLookup          function_lookup;
    CryTagLookup               tag_lookup;
    CryTagFunction             tag_function;
    CryFunction                function;
    CryScope                  *scope;
    CryData                   *data;
};

// Function Prototypes
CryExpr  CryExpr_init(int opcode, CryData *data);
CryExpr  CryExpr_clone(CryExpr expr);
CryData *CryExpr_eval(CryContext *context, CryExpr expr);
CryData *CryExpr_get_numeric(CryContext *context, CryData *data);
CryData *CryExpr_get_boolean(CryContext *context, CryData *data);
CryData *CryExpr_get_comparable(CryContext *context, CryData *data);
CryData *CryExpr_eval_minus_arithmetic(CryData *data);
CryData *CryExpr_eval_arithmetic(CryContext *context, CryData *data1, int opcode, CryData *data2);
CryData *CryExpr_eval_boolean(CryData *data1, int opcode, CryData *data2);
CryData *CryExpr_eval_comparable(CryData *data1, int opcode, CryData *data2);
CryData *CryExpr_eval_function(CryContext *context, CryExpr expr, CryData *function_name, CryData *data);
CryData *CryExpr_eval_array(CryContext *context, CryExpr expr);
void     CryExpr_free(CryExpr expr);
