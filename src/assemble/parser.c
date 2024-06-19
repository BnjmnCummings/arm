#include "parser.h"
#include "tokenizer.h"
#include "funtable.h"

void parse_line(char *line, int addr) {
    //invoke tokeniser
    tokenized_line *tline = tokenize(line);
    //get labels
    //replace lables with address (int) as a string with # char before it
    for(int i = 0; i<tline->ntokens; i++) {
        int labelAddress = get_address(tline->args[i]);
        if(labelAddress != SYMBOL_NOTFOUND) {
            token buffer;
            sprintf(buffer, "#%d", labelAddress);
            strcpy(tline->args[i], buffer);
        }
    }

    //invoke function from map and pass into filewriter
    uint32_t bin = (getFunction(tline->inst))(*tline, addr);
    write_instruction(bin);
}