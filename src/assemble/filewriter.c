#include "filewriter.h"
#include "util.h"

static FILE *out;

void initFileWriter(char *filename) { 
    out = fopen(filename, "wb");
    if(out == NULL) {
        fprintf( stderr,"Error: can’t open %s\n", filename );
        exit(1);
    }
}

void writeInstruction(int32_t word) {
//    printf("word passed in: ");
//    printBits(word, 32);
//    int32_t le_word = toLittleEndian(word);
//    printf("word in little endian: ");
//    printBits(le_word, 32);
    fwrite(&word, INSTR_SIZE,  1, out);
    if( ferror(out) ) {
        fprintf( stderr, "Error occurred writing to file\n");
        exit(1);
    }
}

void wclose() {
    fclose(out);
}
