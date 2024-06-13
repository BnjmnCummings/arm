#include <stdio.h>
#define INSTR_SIZE 4 // 4 bytes

#ifndef ARMV8_31_ASS_FILE_WRITER_H
#define ARMV8_31_ASS_FILE_WRITER_H

//static global variable stores pointer to the output file
static FILE *out;

//initialises the output stream as given by cli arguments
extern void initFileWriter(char *);
//invokes fwrite with apporopriate arguments
extern void writeInstruction(const void *);
// size_t fwrite( const void *ptr, size_t size, size_t nmemb, FILE *stream );
extern void wclose();

#endif //ARMV8_31_ASS_FILE_WRITER_H