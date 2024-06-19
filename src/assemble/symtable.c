#include "symtable.h"

static symtable *table;

//doubles the capacity of the symbol table and reallocates members
static void resize(void) {
    table->capacity *= 2;
    table->lb = realloc(table->lb, table->capacity * sizeof(label));
    table->addr = realloc(table->addr, table->capacity * sizeof(label));
}

//stores a given symbol/address pair into the table
void store_symbol(char *label, int addr) {
    strcpy(table->lb[table->nitems], label);
    table->addr[table->nitems] = addr;
    table->nitems++;
    if (table->nitems >= table->capacity) {
        resize();
    }
}

//initialises symbol tables with malloc
void init_table(void) {
    table = (symtable *) malloc(sizeof(symtable));
    table->nitems = 0;
    table->capacity = INITIAL_CAP;
    table->addr = malloc(INITIAL_CAP * sizeof(int));
    table->lb = malloc(INITIAL_CAP * sizeof(label));
}

//retrieves address associated with 'label' parameter
int get_address(char *label) {
    for (int i = 0; i < table->nitems; i++) {
        if (strcmp(table->lb[i], label) == 0) {
            return table->addr[i];
        }
    }
    return SYMBOL_NOTFOUND;
}

//frees the allocations
void free_table(void) {
    free(table->lb);
    free(table->addr);
    free(table);
}