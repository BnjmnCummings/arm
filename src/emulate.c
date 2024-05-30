#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#define MEMORYSIZE 2 << 20
#define NUMBERGENERALREGISTERS 31
#define BYTESIZE 8

//TODO(the registers only take <= 64 bits currently this is unchecked)

// Contains condition flags about the last result
// Used for the Processor State Register
struct pstate{
    bool Negative; // Negative condition flag
    bool Zero; // Zero condition flag
    bool Carry; // Carry condition flag
    bool Overflow; // Overflow condition flag
}

// TODO(figure out correct initialisation)
initialPstate = {false, true, false, false};
// The processor structure stores the registers and memory
// Used for the CPU
typedef struct {
    u_int8_t memory[MEMORYSIZE];
    int generalPurpose[NUMBERGENERALREGISTERS]; // general purpose registers
    const int ZR; // zero register
    int PC; // program counter
    // int SP; // stack pointer (Un-needed)
    struct pstate PSTATE; // Processor State Register
} processor;
// ZR must be initialised here as it is const
processor CPU = {.ZR = 0};

// TODO(figure out correct initialisation)
// setupCPU is a function taking no arguments
// It initializes the registers in the CPU to the correct starting values
// It returns nothing
void setupCPU( void ){
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

    bool halt = false;
    while (!halt){

        // Fetch
        int pcValue = CPU.PC;

        // Throw error code 2 if pc value points to outside memory
        //TODO(if pc becomes unsigned int remove the <0 check)
        if (pcValue > (MEMORYSIZE - 3) || pcValue < 0){
            printf("fetch failed on nonexistent memory location with pc value: %d\n", pcValue);
            exit(2);
        }

        // Throw error code 3 if pc points to a memory location not at the start of a word
        if (pcValue % 4 != 0){
            printf("fetch failed on nonaligned memory location with pc value: %d\n", pcValue);
            exit(3);
        }

        // read word in little endian
        u_int word = CPU.memory[pcValue + 3];
        for (int i = 1; i < 4; i++){
            word &= CPU.memory[pcValue + 3 - i] << (BYTESIZE * i);
        }

        // check if the instruction is a halt (Untested)
        if (word == 0x8a000000){
            halt = true;
        }

        // Decode:
        //TODO(Add check for termination input)
        u_int op0 = ((15 << 24) & word) >> 24;
        // 101x -> Branch group
        if ((op0 & 14) == 10){
            //TODO(Decode then execute as branch)
        }
        // 100x -> Data processing (immediate) group
        else if ((op0 & 14) == 8) {
            //TODO(Decode then execute as data processing immediate)
        }
        // x101 -> Data processing (register) group
        else if ((op0 & 7) == 5) {
            //TODO(Decode then execute as data processing register)
        }
        // x1x0 -> Loads and stores group
        else if ((op0 & 5) == 4) {
            //TODO(Decode then execute as loads and stores)
        }
        // No matching group so throw error code 4
        else {
            printf("decode failed on non-matching op0 value: %d, with word value: %d\n", op0 , word);
            exit(4);
        }

        // Temp ending ensurance TODO(remove when termination tested)
        halt = true;
    }

    return EXIT_SUCCESS;
}

/* TODO( complete all the below sections for the emulator)
 DONE: binary file loader - create a function that takes in a (binary)
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