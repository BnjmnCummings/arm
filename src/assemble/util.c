#include "util.h"

//converts a 32 bit binary number to little endian form
//I don't know if this will work with unsigned ints
int32_t toLittleEndian( int32_t word ) {
    int32_t firstByte  = (FIRST_BYTE & word)  << (3 * BYTE_SIZE);
    int32_t secondByte = (SECOND_BYTE & word) << BYTE_SIZE;
    int32_t thirdByte  = (THIRD_BYTE & word)  >> BYTE_SIZE;
    int32_t fourthByte = (FOURTH_BYTE & word) >> (3 * BYTE_SIZE);

    return (firstByte | secondByte | thirdByte | fourthByte) ;
}

void printBits( uint32_t x, int nbits ) {
  uint32_t mask = 1 << (nbits-1);
  for( int i=0; i<nbits; i++ ) {
    int bit = (x & mask) != 0;
    putchar( '0' + bit );
    mask >>= 1;      
  }
  putchar('\n');
}

uint32_t reg_to_bin(char *reg) {
    return strtol(reg + 1, NULL, 10)
}

uint32_t calc_num(bool signd, int num_bits, char *numstr) {
    int num;
    if (strncmp(numstr, "#0x", 3)) {
        num = strtol(numstr + 1, NULL, 16);
    } else {
        num = strtol(numstr + 1, NULL, 10);
    }

    if (!signd || num >= 0) {
        return num;
    } else {
        uint32_t snum = num;
        snum &= (2 << num_bits) - 1;
        return snum;
    }
}
