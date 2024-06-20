#include "filewriter.h"

#define REGISTERWIDTH 16
#define INDEXWIDTH 2
#define MEMORYWIDTH 8

extern processor CPU;


//takes in a pointer to the LSB
//returns the value of the 4-byte integer
static uint combineLittleEndian(int index) {

    uint total = 0;
    for(int i = 0; i<4; i++) {
        total += (CPU.memory[index + i]) << (i * 8);
    }

    return total;
}

// write PSTATE to file
static void printPState(FILE *filePath) {

    char pStateStr[] = "NZCV";

    bool stateList[] = {
            CPU.PSTATE.Negative,
            CPU.PSTATE.Zero,
            CPU.PSTATE.Carry,
            CPU.PSTATE.Overflow
    };

    for(int i = 0; i<4; i++) {
        if (!stateList[i]) { pStateStr[i] = '-';}
    }

    fprintf(filePath, "PSTATE : %s\n", pStateStr);
}

// write register states to file
static void printRegisters(FILE *filePath) {
    //general purpose registers:
    for(int i = 0; i < NUMBERGENERALREGISTERS; i++) {
        fprintf(filePath,"X%0*d    = %0*lx\n", INDEXWIDTH, i, REGISTERWIDTH, CPU.generalPurpose[i]);
    }

    //Program counter
    fprintf(filePath,"PC     = %0*lx\n", REGISTERWIDTH, CPU.PC);

}

// find and write non-zero memory to file
static void printNonZeroMemory(FILE *filePath) {
    for (int i = 0; i < MEMORYSIZE; i += 4) {
        uint64_t word = combineLittleEndian(i);
        if (word) {
            fprintf(filePath,"0x%0*x : %0*lx\n", MEMORYWIDTH, i, MEMORYWIDTH, word);
        }
    }
}

// write CPU state to file
void write_cpu(FILE *filePath) {
    //General Purpose and PC Registers
    fprintf(filePath, "Registers:\n");
    printRegisters(filePath);
    //P-STATE
    printPState(filePath);
    //Non-Zero Memory
    fprintf(filePath, "Non-Zero Memory:\n");
    printNonZeroMemory(filePath);
}
