// Structs
struct CryScope {
    char *name;
    void *extra;

    CryTagFunction                     tag_function;
    CryFunction                        function;
    CrymlScopedDataConversionCallback  to_data;
    CrymlScopedDataConversionCallback  from_data;
    CrymlScopedEvalCallback            eval;
    CrymlScopedMethodCallback          method_call;
    CrymlScopedInitCallback            init;
    CrymlScopedTraverseCallback        traverse;
    CrymlScopedFreeCallback            free_data;
    CrymlScopedCheckPrintableCallback  printable;
    CrymlScopedPrintCallback           print;

    CryScope *next;
};