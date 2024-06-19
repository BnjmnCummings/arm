#ifndef ARMV8_31_ASS_UTIL_H
#define ARMV8_31_ASS_UTIL_H
//This is a utility file that was created to house a toLittleEndian function.
//add any useful functions which are reused project-wide
#include "assemble.h"

#define FIRST_BYTE  0x000000FF
#define SECOND_BYTE 0x0000FF00
#define THIRD_BYTE  0x00FF0000
#define FOURTH_BYTE 0xFF000000
#define BYTE_SIZE   8

extern void printBits( uint32_t x, int nbits );
extern int32_t toLittleEndian( int32_t word );
extern uint32_t reg_to_bin(char *reg);
extern uint32_t calc_num(bool signd, int num_bits, char *numstr);
extern bool check_sf(char *r1, char *r2);
extern uint32_t calc_offset(bool signd, int num_bits, char *numstr, int addr);

#endif //ARMV8_31_ASS_UTIL_H