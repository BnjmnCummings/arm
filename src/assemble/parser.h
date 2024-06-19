#ifndef ARMV8_31_ASS_PARSER_H
#define ARMV8_31_ASS_PARSER_H

#include "assemble.h"
#include "symtable.h"
#include "binbuilder.h"
#include "tokenizer.h"
#include "funtable.h"
#include "filewriter.h"

extern void parse_line(char *line, int addr);

#endif //ARMV8_31_ASS_PARSER_H