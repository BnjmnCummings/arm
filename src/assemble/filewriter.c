#include <stdio.h>
#include <stdlib.h>
#include "filewriter.h"

void initFileWriter(char *filename) { 
    out = fopen(filename, "wb");
    if(out == NULL) {
        fprintf( stderr,"Error: can’t open %s\n", filename );
        exit(1);
    }
}

void writeInstruction(const void *ptr) {
    fwrite(ptr, INSTR_SIZE,  1, out);
    if( ferror(out) ) {
        fprintf( stderr, "Error occurred writing to file\n");
        exit(1);
    }
}

void wclose() {
    fclose(out);
}
