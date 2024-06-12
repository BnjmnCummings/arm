#include <stdio.h>

#ifndef ARMV8_31_ASS_FILE_WRITER_H
#define ARMV8_31_ASS_FILE_WRITER_H

//static global variable stores pointer to the output file
static FILE *out;

//initialises the output stream as given by cli arguments
extern void initfilewriter(FILE *);
//invokes fwrite with apporopriate arguments
extern void writeline(const void *, size_t,  size_t);
// size_t fwrite( const void *ptr, size_t size, size_t nmemb, FILE *stream );
#endif //ARMV8_31_ASS_FILE_WRITER_H