#ifndef ARMV8_31_FUNTABLE_H
#define ARMV8_31_FUNTABLE_H
#include "assemble.h"
#define NUM_FUNCTIONS 38

typedef uint32_t (*fun_pointer)(char *, char **, int) ;

typedef struct {
    char *mnemonic;
    fun_pointer function;
} pair;

typedef pair *ftable_t;

extern fun_pointer getFunction(char *mnemonic);

#endif //ARMV8_31_FUNTABLE_H
