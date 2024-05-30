#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "filewriter.h"
#include "processor.h"

//TODO(the registers only take <= 64 bits currently this is unchecked)

// TODO(figure out correct initialisation)
pstate initialPstate = {false, true, false, false};

// ZR must be initialised here as it is const
processor CPU = {.ZR = 0};

// TODO(figure out correct initialisation)
// setupCPU is a function taking no arguments
// It initializes the registers in the CPU to the correct starting values
// It returns nothing
void setupCPU(){
    CPU.PC = 0;
    CPU.PSTATE = initialPstate;
}

// binaryFileLoader is a function taking the filename as an argument
// It copies the contents of the file into the CPU's memory
// It returns true on a successful execution, false otherwise
bool binaryFileLoader(char *fileName){

    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        fprintf( stderr, "Can't read given file\n" );
        return false;
    }

    int currentMemoryAddress = 0;
    u_int ch;
    while( (ch = getc(file)) != EOF ) {
        CPU.memory[currentMemoryAddress] = ch;
        currentMemoryAddress ++;
    }

    fclose(file);
    return true;
}

int main(int argc, char **argv) {
    
    setupCPU();
    
    if (argc > 1) {
        if (!binaryFileLoader(argv[1])){
            printf("binary file loader failed on file %s\n", argv[1]);
            exit(1);
        }
    }
    printf("%s", *argv);

    //for file writer
    // Initialize memory array for testing
    for(int i = 0; i < 4; i++) {
        CPU.memory[i] = i + 1; // Example initialization, adjust as needed
    }
//    printNonZeroMemory();
//    int arr[] = {1,2,3,4};
//    printf("%x", combineLittleEndian(arr));
    printCPU(CPU);

    return EXIT_SUCCESS;
}

/* TODO( complete all the below sections for the emulator)
 NEEDS TESTING: binary file loader - create a function that takes in a (binary)
 file location and reads it into memory

 write emulator loop:
    simulator of arm execution cycle - want to simulate the fetch and decode
    of the FDE cycle so something like a:
        DONE: struct for Z/P/S...
        DONE: struct for the CPU initialised once as a global variable - the CPU
        function made to read the instruction indicated by the CPU state
        functions to decode different forms of instructions

    simulate execution of instructions - quite probably a set of functions
    and a massive switch statement that looks at the current instruction and
    just does the necessary operations on the CPU (global instance)

    terminate upon instruction with encoding 0x8a000000 - probably just an added
    condition on the above part

    Upon termination, output the registers values, the PSTATE condition flags, and any non-zero memory to a .out file-
    simply a long print statement -
        function to output the correct formatting of CPU states
        may need some helper functions to clear things up
 */