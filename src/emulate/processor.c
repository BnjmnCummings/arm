#include "processor.h"

processor CPU = {.ZR = 0};
pstate initialPstate = {false, true, false, false};

void setupCPU( void ){
    CPU.PC = 0;
    CPU.PSTATE = initialPstate;
}

void incrementPC(void) {
    CPU.PC += 4;
}

static u_int32_t read32BitModeRegister(u_int32_t registerNumb) {
    assert((0 <= registerNumb) && (registerNumb <= 30));
    u_int64_t bitMask32 = ((u_int64_t) 1 << 32) - 1;
    return (u_int32_t) (CPU.generalPurpose[registerNumb] & bitMask32);
}

u_int64_t readRegister(bool in64BitMode, u_int32_t registerNumb){
    if (registerNumb == 31) { return 0; }
    assert((0 <= registerNumb) && (registerNumb <= 30));
    if (in64BitMode) {
        return CPU.generalPurpose[registerNumb];
    }
    return read32BitModeRegister(registerNumb);
}

static void write32BitModeRegister(u_int32_t registerNumb, u_int64_t data) {
    assert((0 <= registerNumb) && (registerNumb <= 30));
    u_int64_t bitMask32 = (((u_int64_t) 1) << 32) - 1;
    CPU.generalPurpose[registerNumb] = data & bitMask32;
}

void writeRegister(bool in64BitMode, u_int32_t registerNumb, u_int64_t data){
    if (registerNumb == 31) { return; }
    assert((0 <= registerNumb) && (registerNumb <= 30));
    if (in64BitMode) {
        CPU.generalPurpose[registerNumb] = data;
        return;
    }
    write32BitModeRegister(registerNumb, data);
}

uint64_t readMemory(bool in64BitMode, u_int64_t memoryAddress){
    assert((0 <= memoryAddress) && (memoryAddress < MEMORYSIZE));
    uint64_t word = 0;
    for (int i = 0; i < 4 + (4 * in64BitMode); i++) {
        // printf("word: %lu, add: %lu\n", word, ( (uint64_t) CPU.memory[memoryAddress + i]) << BYTESIZE * i);
        word += ( (uint64_t) CPU.memory[memoryAddress + i]) << BYTESIZE * i;
    }
    return word;
}

bool writeMemory(bool in64BitMode, u_int64_t memoryAddress, u_int64_t data) {
    assert((0 <= memoryAddress) && (memoryAddress < MEMORYSIZE));
    u_int32_t byteSizeMask = ((2 << BYTESIZE) - 1);
    for (int i = 0; i < 4 + (4 * in64BitMode); i ++){
        CPU.memory[memoryAddress + i] = (data >> (BYTESIZE * i)) & byteSizeMask;
    }
    return true;
}