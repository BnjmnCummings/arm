#ifndef ARMV8_31_ASS_SYMTABLE_H
#define ARMV8_31_ASS_SYMTABLE_H
//code
#define INITIALCAP 10
#define MAXSYMLEN 64

typedef char[MAXSYMLEN] label;

typedef struct {
    int nitems;
    int capacity;
    label *lb;
    int *addr;
} symtable;

void store_symbol(char *line, int addr);
void init_table(void);
int get_address(char *label);
void free_table(void);

#endif //ARMV8_31_ASS_SYMTABLE_H