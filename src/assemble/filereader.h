#ifndef ARMV8_31_ASS_FILEREADER_H
#define ARMV8_31_ASS_FILEREADER_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include "assemble.h"
#include "parser.h"
#include "symtable.h"

#define MAX_LINE_LENGTH 80

//passes each line into the parser
extern void read_symbol(char *, int *);
extern void read_line(char *, int *);
extern void read_file(void (*f)(char *, int *));
extern void initFileReader(char *);
extern void rclose();

#endif //ARMV8_31_ASS_FILEREADER_H