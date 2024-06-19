#include "tokenizer.h"

//parses string 'line' into a tokenized_line
// containing its mnemonic, a list of arguments and the number of arguments
tokenized_line *tokenize(char *line) {
    // memory allocation
    tokenized_line *tokenized = malloc(sizeof(tokenized_line));
    if (tokenized == NULL) {
        return NULL;
    }
    tokenized->args = malloc(sizeof(token) * MAX_TOKENS);
    if (tokenized->args == NULL) {
        free(tokenized);
        return NULL;
    }

    // store the instruction type
    char *token = strtok(line, " ");
    if (token == NULL) {
        free_tokenized(tokenized);
        return NULL;
    }
    strcpy(tokenized->inst, token);

    // tokenize the tokens
    int i = 0;
    while (i < MAX_TOKENS && (token = strtok(NULL, ", ")) != NULL) {
        strcpy(tokenized->args[i], token);
        i++;
    }

    // null-terminate token_list if exact count is known
    if (i < MAX_TOKENS) {
        // mark end of tokens
        tokenized->args[i][0] = '\0';
    }
    tokenized->ntokens = i;
    return tokenized;
}

// frees a tokenized line (do after finishing conversion to bin)
void free_tokenized(tokenized_line *tokenized) {
    free(tokenized->args);
    free(tokenized);
}