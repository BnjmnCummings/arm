#include "parser.h"
#include "tokenizer.h"
#include "funtable.h"

//writes the binary instruction associated with a given
// string instruction, 'line' and it's address
void parse_line(char *line, int addr) {
    //invoke tokeniser
    tokenized_line *tline = tokenize(line);

    //get labels and replace with address as a formatted string
    for(int i = 0; i<tline->ntokens; i++) {
        int label_address = get_address(tline->args[i]);
        if(label_address != SYMBOL_NOTFOUND) {
            token buffer;
            sprintf(buffer, "#%d", label_address);
            strcpy(tline->args[i], buffer);
        }
    }

    //invoke function from map and pass into file-writer
    uint32_t bin = (get_bin_function(tline->inst))(*tline, addr);
    write_instruction(bin);

    //free the heap allocated tokenized_line type
    free_tokenized(tline);
}