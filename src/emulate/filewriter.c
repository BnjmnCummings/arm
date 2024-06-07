#include "filewriter.h"
#include "processor.h"
#include <stdio.h>
#include <stdbool.h>
#define REGISTERWIDTH 16
#define INDEXWIDTH 2
#define MEMORYWIDTH 8

//FILE-WRITER ---

//takes in a pointer to the LSB
//returns the value of the 4-byte long integer
static u_int combineLittleEndian(u_int index, processor *CPU) {

    u_int total = 0;

    for(int i = 0; i<4; i++) {
//        total += (*arr++) << (i*8);
        total += (CPU->memory[index + i]) << (i * 8);
         //dereference the pointer and shift by
        //(number of bits = 8*index relative to where we started)
        //finally increment the pointer
    }

    return total;
}

static void printPState(FILE *filePath, processor *CPU) {

    char pStateStr[] = "NZCV";

    bool stateList[] = {
            CPU->PSTATE.Negative,
            CPU->PSTATE.Zero,
            CPU->PSTATE.Carry,
            CPU->PSTATE.Overflow
    };

    for(int i = 0; i<4; i++) {
        if (!stateList[i]) { pStateStr[i] = '-';}
    }

    fprintf(filePath, "PSTATE : %s\n", pStateStr);
}

//register Printer:
static void printRegisters(FILE *filePath, processor *CPU) {
    //general purpose registers:
    for(int i = 0; i < NUMBERGENERALREGISTERS; i++) {
        fprintf(filePath,"X%0*d    = %0*lx\n", INDEXWIDTH, i, REGISTERWIDTH, CPU->generalPurpose[i]);
    }

    //Program counter
    fprintf(filePath,"PC     = %0*lx\n", REGISTERWIDTH, CPU->PC);

}

static void printNonZeroMemory(FILE *filePath, processor *CPU) {
    for (int i = 0; i < MEMORYSIZE; i += 4) {
        uint word = combineLittleEndian(i, CPU);
        if (word) {
            fprintf(filePath,"0x%0*x : %0*x\n", MEMORYWIDTH, i, MEMORYWIDTH, word);
        }
    }
}

void writeCPU(FILE *filePath, processor *CPU) {
    //General Purpose and PC Registers
    fprintf(filePath, "Registers:\n");
    printRegisters(filePath, CPU);
    //P-STATE
    printPState(filePath, CPU);
    //Non-Zero Memory
    fprintf(filePath, "Non-Zero Memory:\n");
    printNonZeroMemory(filePath, CPU);
}
