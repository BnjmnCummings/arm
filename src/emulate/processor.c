#include "processor.h"

// global variable representing CPU state
processor CPU = {.ZR = 0};
// starting PSTATE values
static pstate initialPstate = {false, true, false, false};

// initialise CPU
void setup_cpu( void ){
    CPU.PC = 0;
    CPU.PSTATE = initialPstate;
}

// increment PC to next instruction
void increment_pc(void) {
    CPU.PC += 4;
}

// return given register value read in 32 bit mode
static uint32_t read32BitModeRegister(uint32_t registerNumb) {
    assert((0 <= registerNumb) && (registerNumb <= 30));
    uint64_t bitMask32 = ((uint64_t) 1 << 32) - 1;
    return (uint32_t) (CPU.generalPurpose[registerNumb] & bitMask32);
}

// return given register value read in 64 or 32 bit mode, based on in64BitMode
uint64_t read_register(bool in64BitMode, uint32_t registerNumb){
    if (registerNumb == 31) { return 0; }
    assert((0 <= registerNumb) && (registerNumb <= 30));
    if (in64BitMode) {
        return CPU.generalPurpose[registerNumb];
    }
    return read32BitModeRegister(registerNumb);
}

// write data to register in 32 bit mode
static void write32BitModeRegister(uint32_t registerNumb, uint64_t data) {
    assert((0 <= registerNumb) && (registerNumb <= 30));
    uint64_t bitMask32 = (((uint64_t) 1) << 32) - 1;
    CPU.generalPurpose[registerNumb] = data & bitMask32;
}

// write data to register in 64 or 32 bit mode, based on in64BitMode
void write_register(bool in64BitMode, uint32_t registerNumb, uint64_t data){
    if (registerNumb == 31) { return; }
    assert((0 <= registerNumb) && (registerNumb <= 30));
    if (in64BitMode) {
        CPU.generalPurpose[registerNumb] = data;
        return;
    }
    write32BitModeRegister(registerNumb, data);
}

// read 64 or 32 bit value from memory
uint64_t read_memory(bool in64BitMode, uint64_t memoryAddress){
    assert((0 <= memoryAddress) && (memoryAddress < MEMORYSIZE));
    uint64_t word = 0;
    for (int i = 0; i < 4 + (4 * in64BitMode); i++) {
        word += ( (uint64_t) CPU.memory[memoryAddress + i]) << BYTESIZE * i;
    }
    return word;
}

// write 64 or 32 bit data to memory
bool write_memory(bool in64BitMode, uint64_t memoryAddress, uint64_t data) {
    assert((0 <= memoryAddress) && (memoryAddress < MEMORYSIZE));
    uint32_t byteSizeMask = ((2 << BYTESIZE) - 1);
    for (int i = 0; i < 4 + (4 * in64BitMode); i ++){
        CPU.memory[memoryAddress + i] = (data >> (BYTESIZE * i)) & byteSizeMask;
    }
    return true;
}