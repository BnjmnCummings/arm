#ifndef ARMV8_31_ASS_FILEREADER_H
#define ARMV8_31_ASS_FILEREADER_H

#include "assemble.h"
#include "parser.h"
#include "symtable.h"

#define MAX_LINE_LENGTH 80

extern void init_file_reader(char *filename);
extern void read_symbol(char *buffer, int *addr);
extern void read_line(char *buffer, int *addr);
extern void read_file(void (*read_func)(char *, int *));
extern void r_close(void);

#endif //ARMV8_31_ASS_FILEREADER_H