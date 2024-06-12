#include <stdio.h>
#include "filewriter.h"
#define INSTR_SIZE 4 // 4 bytes

static FILE *out;

void initFileWriter(FILE *fp) { out = fp; }

void writeInstruction(const void *ptr) {
    //TODO
    //it's likely that  size and nmemb are constants
    size_t fcode = fwrite(ptr, INSTR_SIZE,  1, out);

    //error handle with fcode. 
    //if fcode < 1 then exception has occured
    if(fcode < 1) {
        //todo handle exception
    }
    
}