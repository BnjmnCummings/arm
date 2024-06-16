#include "tokenizer.h"

tokenized_line *tokenize(char *line) {
    // memory allocation
    tokenized_line *tokenized = malloc(sizeof(tokenized_line));
    if (tokenized == NULL) {
        return NULL;
    }
    tokenized->token_list = malloc(sizeof(token) * MAX_TOKENS);
    if (tokenized->token_list == NULL) {
        free(tokenized);
        return NULL;
    }

    // store the instruction type
    char *token = strtok(line, " ,");
    if (token == NULL) {
        free(tokenized->token_list);
        free(tokenized);
        return NULL;
    }
    strcpy(tokenized->type, token);

    // tokenize the tokens
    int i = 0;
    while (i < MAX_TOKENS && (token = strtok(NULL, " ,")) != NULL) {
        strcpy(tokenized->token_list[i], token);
        i++;
    }

    // null-terminate token_list if exact count is known
    if (i < MAX_TOKENS) {
        tokenized->token_list[i][0] = '\0'; // mark end of tokens
    }

    return tokenized;
}

// frees a tokenized line (do after finishing conversion to bin)
void free_tokenized(tokenized_line *tokenized) {
    free(tokenized->token_list);
    free(tokenized);
}