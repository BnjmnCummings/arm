#include "util.h"
#define MASK_OF_SIZE(num_bits) ((1 << num_bits) - 1)

//converts a given register as an unsigned binary integer
uint32_t reg_to_bin(char *reg) {
    if (strcmp(reg + 1, "zr") == 0) {
        return 0b11111;
    } else {
    return strtol(reg + 1, NULL, DEC_BASE);
    }
}

//calculates binary value of a hexadecimal or decimal literal
uint32_t calc_num(bool is_signed, int num_bits, char *num_str) {
    int num;
    //handles hex and binary cases
    if (strncmp(num_str, "0x", 2) == 0) {
        num = strtol(num_str, NULL, HEX_BASE);
    } else {
        num = strtol(num_str, NULL, DEC_BASE);
    }

    if (!is_signed || num >= 0) {
        return num;
    } else {
        //for negative numbers, only take the first 'num_bits' of bits
        return num  & MASK_OF_SIZE(num_bits);
    }
}

//calculates offset between the location of a branch stored in 'num_str'
// and the current address 'addr'.
uint32_t calc_offset(bool is_signed, int num_bits, char *num_str, int addr) {
    int32_t num = calc_num(is_signed, num_bits, num_str);
    return ((uint32_t) (num - addr) / 4) & MASK_OF_SIZE(num_bits);
}

//returns true if either argument is a 32 bit register
bool check_sf(char *r1, char *r2) {
    return (r1[0] == 'x') || (r2[0] == 'x');
}
