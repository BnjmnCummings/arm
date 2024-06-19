#ifndef ARMV8_31_TOKENIZER_H
#define ARMV8_31_TOKENIZER_H

#include "assemble.h"

#define TOKEN_SIZE 100
#define MAX_TOKENS 5
#define INST_SIZE 5

typedef char token[TOKEN_SIZE];

typedef struct {
    char inst[INST_SIZE];
    token *args;
    int ntokens;
} tokenized_line;

extern tokenized_line *tokenize(char *);
extern void free_tokenized(tokenized_line *);

#endif //ARMV8_31_TOKENIZER_H
