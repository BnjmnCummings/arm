#ifndef ARMV8_31_FUNTABLE_H
#define ARMV8_31_FUNTABLE_H

#include "assemble.h"
#include "binbuilder.h"
#define NUM_FUNCTIONS 38

typedef uint32_t (*fun_pointer)(tokenized_line, int) ;

typedef struct {
    char *mnemonic;
    fun_pointer function;
} pair;

extern fun_pointer get_bin_function(char *mnemonic);

#endif //ARMV8_31_FUNTABLE_H
