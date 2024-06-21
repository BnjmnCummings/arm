#include "filewriter.h"

static FILE *out;

//initialises the output stream as given by cli arguments
void init_file_writer(char *filename) {
    out = fopen(filename, "wb");
    if(out == NULL) {
        fprintf( stderr,"Error: can't open %s\n", filename );
        exit(EXIT_FAILURE);
    }
}

//invokes 'fwrite()' with appropriate arguments
void write_instruction(uint32_t word) {
    fwrite(&word, INSTR_SIZE,  1, out);
    if( ferror(out) ) {
        fprintf( stderr, "Error occurred writing to file\n");
        exit(EXIT_FAILURE);
    }
}

//invokes 'fclose()' on write-file pointer
void w_close(void) {
    fclose(out);
}
