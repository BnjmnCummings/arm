#ifndef ARMV8_31_ASS_FILEREADER_H
#define ARMV8_31_ASS_FILEREADER_H

#include "assemble.h"
#include "parser.h"
#include "symtable.h"

#define MAX_LINE_LENGTH 80

//passes each line into the parser
extern void read_symbol(char *, int *);
extern void read_line(char *, int *);
extern void read_file(void (*f)(char *, int *));
extern void init_file_reader(char *);
extern void r_close();

#endif //ARMV8_31_ASS_FILEREADER_H