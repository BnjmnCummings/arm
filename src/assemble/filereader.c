#include "filereader.h"

static FILE *in;

void initFileReader(char * filename) {
    in = fopen(filename, "r");
    if(in == NULL) {
        fprintf( stderr, "Error: can’t open %s\n", filename );
        exit(1);
    }
}

void readAssFile() {
    
    char *buffer = malloc(MAX_LINE_LENGTH);

    while( fgets(buffer, MAX_LINE_LENGTH, in) != NULL ) {
        //TODO
        //pass buffer into parser/tokeniser
        //parsing/tokenising MUST terminate after reading \0 character
    }

    free(buffer);
}

void first_pass() {
    char *buffer = malloc(MAX_LINE_LENGTH);

    while( fgets(buffer, MAX_LINE_LENGTH, in) != NULL ) {
        //TODO
        //pass buffer into parser/tokeniser
        // pass tokens into symbol table
        //parsing/tokenising MUST terminate after reading \0 character
    }

    free(buffer);
    //TODO
    // grab pointer to completed symtable
    // close and reopen file
}

void rclose() {
    fclose(in);
}
