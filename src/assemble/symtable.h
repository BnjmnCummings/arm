#ifndef ARMV8_31_ASS_SYMTABLE_H
#define ARMV8_31_ASS_SYMTABLE_H
#include <stdlib.h>
#include <string.h>
#include "parser.h"

#define INITIALCAP 10
#define MAXSYMLEN 512

typedef char label[MAXSYMLEN];

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