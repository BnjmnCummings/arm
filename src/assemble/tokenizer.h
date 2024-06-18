#ifndef ARMV8_31_TOKENIZER_H
#define ARMV8_31_TOKENIZER_H

#include "assemble.h"

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
