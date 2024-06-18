//
// Created by Bruce Chen on 6/14/24.
//

#ifndef ARMV8_31_TOKENIZER_H
#define ARMV8_31_TOKENIZER_H
#include "symtable.h"
#include <string.h>
#include <stdlib.h>

#define TOKEN_SIZE 100 // set size to max token size
#define MAX_TOKENS 5

typedef char token[TOKEN_SIZE];

typedef struct {
    char *type; // type of instruction being handled
    token *token_list; // list of operators
} tokenized_line;



tokenized_line *tokenize(char *);
void free_tokenized(tokenized_line *);

#endif //ARMV8_31_TOKENIZER_H
