#ifndef ARMV8_31_ASS_FILE_WRITER_H
#define ARMV8_31_ASS_FILE_WRITER_H

#include "assemble.h"
#define INSTR_SIZE 4 // 4 bytes

//initialises the output stream as given by cli arguments
extern void initFileWriter(char *);
//invokes fwrite with apporopriate arguments
extern void writeInstruction(int32_t word);
// size_t fwrite( const void *ptr, size_t size, size_t nmemb, FILE *stream );
extern void wclose();

#endif //ARMV8_31_ASS_FILE_WRITER_H