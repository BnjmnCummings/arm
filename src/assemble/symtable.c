#include "symtable.h"

static symtable *table;

static void resize(void) {
    table->capacity *= 2;
    table->lb = realloc(table->lb, table->capacity * sizeof(label));
    table->addr = realloc(table->addr, table->capacity * sizeof(label));
}

void store_symbol(char *label, int addr) {
    strcpy(table->lb[table->nitems], label);
    table->addr[table->nitems] = addr;
    printf("label: %s, addr: %d\n", table->lb[table->nitems], table->addr[table->nitems]);
    table->nitems++;
    if (table->nitems >= table->capacity) {
        resize();
    }
}

void init_table(void) {
    table = (symtable *) malloc(sizeof(symtable));
    table->nitems = 0;
    table->capacity = INITIAL_CAP;
    table->addr = malloc(INITIAL_CAP * sizeof(int));
    table->lb = malloc(INITIAL_CAP * sizeof(label));
}

int get_address(char *label) {
    for (int i = 0; i < table->nitems; i++) {
        if (strcmp(table->lb[i], label) == 0) {
            return table->addr[i];
        }
    }
    return SYMBOL_NOTFOUND;
}

void free_table(void) {
    free(table->lb);
    free(table->addr);
    free(table);
}