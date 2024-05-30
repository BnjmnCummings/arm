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
static u_int combineLittleEndian(u_int *arr) {

    u_int total = 0;

    for(int i = 0; i<4; i++) {
        total += (*arr++) << (i*8);
        //dereference the pointer and shift by
        //(number of bits = 8*index relative to where we started)
        //finally increment the pointer
    }

    return total;
}

static void printPState(processor CPU) {

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

    printf("PSTATE : %s\n", pStateStr);
}

//register Printer:
static void printRegisters(processor CPU) {
    //general purpose registers:
    for(int i = 0; i < NUMBERGENERALREGISTERS; i++) {
        printf("X%0*d = %0*x\n", INDEXWIDTH, i, REGISTERWIDTH, CPU.generalPurpose[i]);
    }

    //Program counter
    printf("PC = %0*x\n", REGISTERWIDTH, CPU.PC);

}

static void printNonZeroMemory(processor CPU) {

    u_int *ip = CPU.memory;

    //TODO: Find a better way to do this condition
    while (*ip != NULL) {
        //print in format:
        //address: hex value
        printf("%p: 0x%0*x\n", ip, MEMORYWIDTH, combineLittleEndian(ip));
        ip += 4;
    }
}

void printCPU(processor CPU) {
    //General Purpose and PC Registers
    printf("Registers:\n");
    printRegisters(CPU);
    //P-STATE
    printPState(CPU);
    //Non-Zero Memory
    printf("Non-Zero memory:\n");
    printNonZeroMemory(CPU);
}

//int main() {
//    pstate initialPstate = {false, true, false, false};
//
//    processor CPU = {.ZR = 0};
//    CPU.PC = 0;
//    CPU.PSTATE = initialPstate;
//
//    for(int i = 0; i < 4; i++) {
//        CPU.memory[i] = i + 1; // Example initialization, adjust as needed
//    }
//
//    printCPU(CPU);
//    return 0;
//}