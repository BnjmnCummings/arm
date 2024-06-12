#include <stdio.h>
#include <stdlib.h>
#include "filereader.h"

void initFileReader(char * filename) {
    in = fopen(filename, "r");
    if(in == NULL) {
        fprintf( stderr, "Error: can’t open %s\n", filename );
        exit(1);
    }
}

void read() {
    
    char *buffer = malloc(MAX_LINE_LENGTH);

    while( fgets(buffer, MAX_LINE_LENGTH, in) != NULL ) {
        //TODO
        //pass buffer into parser/tokeniser
        //parsing/tokenising MUST terminate after reading \0 character
    }

    free(buffer);
}

void rclose() {
    fclose(in);
}
