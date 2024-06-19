#include "util.h"

uint32_t reg_to_bin(char *reg) {
    if (strcmp(reg + 1, "zr") == 0) {
        return 0b11111;
    } else {
    return strtol(reg + 1, NULL, 10);
    }
}

uint32_t calc_num(bool is_signed, int num_bits, char *num_literal) {
    int num;
    if (strncmp(num_literal, "0x", 2) == 0) {
        num = strtol(num_literal, NULL, 16);
    } else {
        num = strtol(num_literal, NULL, 10);
    }

    if (!is_signed || num >= 0) {
        return num;
    } else {
        uint32_t snum = num;
        snum &= (1 << num_bits) - 1;
        return snum;
    }
}

uint32_t calc_offset(bool is_signed, int num_bits, char *num_literal, int addr) {
    int32_t num = calc_num(is_signed, num_bits, num_literal);
    return ((uint32_t) (num - addr) / 4) & ((1 << num_bits) - 1);
}

bool check_sf(char *r1, char *r2) {
    return (r1[0] == 'x') || (r2[0] == 'x');
}
