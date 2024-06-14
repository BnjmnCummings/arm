#include "util.h"

//converts a 32 bit binary number to little endian form
int32_t toLittleEndian( int32_t word ) {
    int32_t firstByte  = (FIRST_BYTE & word)  << 24;
    int32_t secondByte = (SECOND_BYTE & word) << 8;
    int32_t thirdByte  = (THIRD_BYTE & word)  >> 8;
    int32_t fourthByte = (FOURTH_BYTE & word) >> 24;

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