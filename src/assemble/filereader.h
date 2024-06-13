#ifndef ARMV8_31_ASS_FILEREADER_H
#define ARMV8_31_ASS_FILEREADER_H

#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE_LENGTH 80

//passes each line into the parser 
extern void readAssFile();
extern void initFileReader(char *);
extern void rclose();

#endif //ARMV8_31_ASS_FILEREADER_H