#include "symtable.h"
#include <stdlib.h>

static symtable *table;

static void resize(void) {
    table->capacity *= 2;
    table->lb = realloc(table->lb, table->capacity * sizeof(label));
    table->addr = realloc(table->addr, table->capacity * sizeof(label));
}

void store_symbol(char *label, int addr) {
    table->lb[table->nitems] = label;
    table->addr[table->nitems] = abbr;
    table->nitems++;

    if (table->nitems >= table->capacity) {
        resize();
    }
}

void init_table(void) {
    table = (symtable *) malloc(sizeof(symtable));
    table->nitems = 0;
    table->capacity = MAXSYMBOLS;
    table->addr = malloc(MAXSYMBOLS * sizeof(int));
    table->lb = malloc(MAXSYMBOLS * sizeof(label));
}

int get_address(char *label) {
    for (int i = 0; i < MAXSYMBOLS; i++) {
        if (strncmp(table->lb[i], label, MAXSYMLEN) == 0) {
            return table->addr[i];
        }
    }
    return -1;
}

void free_table(void) {
    free(table->lb);
    free(table->addr);

    free(table);
}