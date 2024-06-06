#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define MEMORYSIZE 2 << 20
#define NUMBERGENERALREGISTERS 31

#ifndef ARMV8_31_PROCESSOR_H
#define ARMV8_31_PROCESSOR_H

typedef struct {
    bool Negative; // Negative condition flag
    bool Zero; // Zero condition flag
    bool Carry; // Carry condition flag
    bool Overflow; // Overflow condition flag
} pstate;

typedef struct {
    u_int memory[MEMORYSIZE];
    long long generalPurpose[NUMBERGENERALREGISTERS]; // general purpose registers
    const int ZR; // zero register
    int PC; // program counter
    // int SP; // stack pointer (Un-needed)
    pstate PSTATE; // Processor State Register
} processor;


#endif //ARMV8_31_PROCESSOR_H
