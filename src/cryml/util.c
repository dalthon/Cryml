#include "cryml.h"

int Util_numeric_comparison(CryData *data1, CryData *data2){
    long double double1, double2;

    if(data1->flags == DATA_INTEGER && data2->flags == DATA_INTEGER){
        return data1->value.integer - data2->value.integer;
    }else{
        if(data1->flags == DATA_INTEGER){
            double1 = 0.0 + data1->value.integer;
        }else{
            double1 = 0.0 + data1->value.cdouble;
        }

        if(data2->flags == DATA_INTEGER){
            double2 = 0.0 + data2->value.integer;
        }else{
            double2 = 0.0 + data2->value.cdouble;
        }

        if(fabs(double1 - double2) < CRYML_FLOAT_PRECISION){
            return 0;
        }else{
            if(double1 > double2){
                return 1;
            }else{
                return -1;
            }
        }

    }
}