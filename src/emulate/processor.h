#ifndef ARMV8_31_PROCESSOR_H
#define ARMV8_31_PROCESSOR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "emulate.h"

#define MEMORYSIZE 2 << 20
#define NUMBERGENERALREGISTERS 31

typedef struct {
    bool Negative;  // Negative condition flag
    bool Zero;      // Zero condition flag
    bool Carry;     // Carry condition flag
    bool Overflow;  // Overflow condition flag
} pstate;

typedef struct {
    uint8_t memory[MEMORYSIZE];                         // memory
    u_int64_t generalPurpose[NUMBERGENERALREGISTERS];   // general purpose registers
    const uint64_t ZR;                                  // zero register
    u_int64_t PC;                                       // program counter
    pstate PSTATE;                                      // Processor State Register
} processor;

extern void setup_cpu(void);
extern void increment_pc(void);
extern u_int64_t read_register(bool in64BitMode, u_int32_t registerNumb);
extern void write_register(bool in64BitMode, u_int32_t registerNumb, u_int64_t data);
extern uint64_t read_memory(bool in64BitMode, u_int64_t memoryAddress);
extern bool write_memory(bool in64BitMode, u_int64_t memoryAddress, u_int64_t data);

#endif //ARMV8_31_PROCESSOR_H
