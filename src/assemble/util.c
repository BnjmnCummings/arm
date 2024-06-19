#include "util.h"

//used for testing purposes
void print_bits( uint32_t x, int nbits ) {
  uint32_t mask = 1 << (nbits-1);
  for( int i=0; i<nbits; i++ ) {
    int bit = (x & mask) != 0;
    putchar( '0' + bit );
    mask >>= 1;      
  }
  putchar('\n');
}

uint32_t reg_to_bin(char *reg) {
    if (strcmp(reg + 1, "zr") == 0) {
        return 0b11111;
    } else {
    return strtol(reg + 1, NULL, 10);
    }
}

uint32_t calc_num(bool signd, int num_bits, char *numstr) {
    int num;
    if (strncmp(numstr, "0x", 2) == 0) {
        num = strtol(numstr, NULL, 16);
    } else {
        num = strtol(numstr, NULL, 10);
    }

    if (!signd || num >= 0) {
        return num;
    } else {
        uint32_t snum = num;
        snum &= (1 << num_bits) - 1;
        return snum;
    }
}

uint32_t calc_offset(bool signd, int num_bits, char *numstr, int addr) {
    int32_t num = calc_num(signd, num_bits, numstr);
    return ((uint32_t) (num - addr) / 4) & ((1 << num_bits) - 1);
}

bool check_sf(char *r1, char *r2) {
    return (r1[0] == 'x') || (r2[0] == 'x');
}
