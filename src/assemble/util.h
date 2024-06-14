#ifndef ARMV8_31_ASS_UTIL_H
#define ARMV8_31_ASS_UTIL_H
//This is a utility file that was created to house a toLittleEndian function.
//add any useful functions which are reused project-wide
#define FIRST_BYTE  0x000000FF
#define SECOND_BYTE 0x0000FF00
#define THIRD_BYTE  0x00FF0000
#define FOURTH_BYTE 0xFF000000

extern void printBits( uint32_t x, int nbits );
extern int32_t toLittleEndian( int32_t word )

#endif //ARMV8_31_ASS_UTIL_H