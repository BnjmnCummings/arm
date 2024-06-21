#ifndef ARMV8_31_ASS_FILE_WRITER_H
#define ARMV8_31_ASS_FILE_WRITER_H

#include "assemble.h"
#define INSTR_SIZE 4 // 4 bytes

extern void init_file_writer(char *filename);
extern void write_instruction(uint32_t word);
extern void w_close(void);

#endif //ARMV8_31_ASS_FILE_WRITER_H