#include "parser.h"
#include <stdlib.h>

void parse_line(char *line, int addr) {
    printf("got to parse line for %s\n", line);
    //invoke tokeniser
    tokenized_line *tline = tokenize(line);

    //get lables
    //replace lables with address (int) as a string with # char before it
    for(int i = 0; i<MAX_TOKENS; i++) { //TODO: alter the tokenizer.c to get the number of tokens
        int labelAddress = get_address(tline->token_list[i]);
        if(labelAddress != -1) {
            token buffer;
            sprintf(buffer, "#%d", labelAddress);
            strcpy(tline->token_list[i], buffer);
        }
    }

    //invoke function from map and pass into filewriter
    (getFunction(tline->type))(tline->type, tline->token_list, addr); // TODO RENAME "type"
}