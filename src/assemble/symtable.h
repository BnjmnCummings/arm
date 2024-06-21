#ifndef ARMV8_31_ASS_SYMTABLE_H
#define ARMV8_31_ASS_SYMTABLE_H

#include "assemble.h"

#define INITIAL_CAP 10
#define MAX_SYMLEN 512
#define SYMBOL_NOTFOUND (-1)

typedef char label[MAX_SYMLEN];

typedef struct {
    int nitems;
    int capacity;
    label *lb;
    int *addr;
} symtable;

extern void store_symbol(char *label, int addr);
extern void init_table(void);
extern int get_address(char *label);
extern void free_table(void);

#endif //ARMV8_31_ASS_SYMTABLE_H