#include "filewriter.h"

static FILE *out;

void initFileWriter(char *filename) { 
    out = fopen(filename, "wb");
    if(out == NULL) {
        fprintf( stderr,"Error: can’t open %s\n", filename );
        exit(EXIT_FAILURE);
    }
}

void writeInstruction(int32_t word) {
    fwrite(&word, INSTR_SIZE,  1, out);
    if( ferror(out) ) {
        fprintf( stderr, "Error occurred writing to file\n");
        exit(EXIT_FAILURE);
    }
}

void wclose() {
    fclose(out);
}
