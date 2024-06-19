#ifndef ARMV8_31_ASS_UTIL_H
#define ARMV8_31_ASS_UTIL_H
//This is a utility file that was created to house a to_little_endian function.
//add any useful functions which are reused project-wide
#include "assemble.h"

#define HEX_BASE 16
#define DEC_BASE 10

extern uint32_t reg_to_bin(char *reg);
extern uint32_t calc_num(bool is_signed, int num_bits, char *num_str);
extern bool check_sf(char *r1, char *r2);
extern uint32_t calc_offset(bool is_signed, int num_bits, char *num_str, int addr);

#endif //ARMV8_31_ASS_UTIL_H