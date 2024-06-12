#include <stdio.h>
#include "filewriter.h"

static FILE *out;

void initfilewriter(FILE *fp) { out = fp; }

void writeline(const void *ptr , size_t size,  size_t nmemb) {
    //TODO
    size_t nwritten = fwrite(ptr, size,  nmemb, out);

    //error handle with nwritten. 
    //if nwritten < nmemb then exception has occured
    if(nwritten < nmemb) {
        //todo handle exception
    }
    
}