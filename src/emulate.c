#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

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
    u_int memory[MEMORYSIZE];
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

// arithmeticInstruction is a function taking the split instruction (word) as argument
// It completed the instruction on the CPU
// It returns true on a successful execution, false otherwise
bool arithmeticInstruction(u_int splitWord[]){
    //TODO(Implement handling of arithmetic instruction, Note you will need to further split input)
    return true;
}

// wideMoveInstruction is a function taking the split instruction (word) as argument
// It completed the instruction on the CPU
// It returns true on a successful execution, false otherwise
bool wideMoveInstruction(u_int splitWord[]){
    //TODO(Implement handling of a wide move instruction, Note you will need to further split input)
    return true;
}

// multiplyInstruction is a function taking the split instruction (word) as argument
// It completed the instruction on the CPU
// It returns true on a successful execution, false otherwise
bool multiplyInstruction(u_int splitWord[]){
    //TODO(Implement handling of a multiply instruction, Note you will need to further split input)
    return true;
}

// arithmeticShiftInstruction is a function taking the split instruction (word) as argument
// It completed the instruction on the CPU
// It returns true on a successful execution, false otherwise
bool arithmeticShiftInstruction(u_int splitWord[]){
    //TODO(Implement handling of an arithmetic shift instruction, Note you will need to further split input)
    return true;
}

// logicalShiftInstruction is a function taking the split instruction (word) as argument
// It completed the instruction on the CPU
// It returns true on a successful execution, false otherwise
bool logicalShiftInstruction(u_int splitWord[]){
    //TODO(Implement handling of a logical shift instruction, Note you will need to further split input)
    return true;
}

// loadLiteralInstruction is a function taking the split instruction (word) as argument
// It completed the instruction on the CPU
// It returns true on a successful execution, false otherwise
bool loadLiteralInstruction(u_int splitWord[]){
    //TODO(Implement handling of a load literal instruction, Note you will need to further split input)
    return true;
}
// singleDataTransferInstruction is a function taking the split instruction (word) as argument
// It completed the instruction on the CPU
// It returns true on a successful execution, false otherwise
bool singleDataTransferInstruction(u_int splitWord[]){
    //TODO(Implement handling of a single data transfer instruction, Note you will need to further split input)
    // Advise that this checks the offset and then calls the respective function
    // Make these funcitons yourself so you can set them up how you want
    return true;
}

// unconditionalBranch is a function taking the split instruction (word) as argument
// It completed the instruction on the CPU
// It returns true on a successful execution, false otherwise
bool unconditionalBranch(u_int splitWord[]){
    //TODO(Implement handling of an unconditional branch instruction, Note you will need to further split input)
    return true;
}

// registerBranch is a function taking the split instruction (word) as argument
// It completed the instruction on the CPU
// It returns true on a successful execution, false otherwise
bool registerBranch(u_int splitWord[]){
    //TODO(Implement handling of a register branch instruction, Note you will need to further split input)
    return true;
}

// conditionalBranch is a function taking the split instruction (word) as argument
// It completed the instruction on the CPU
// It returns true on a successful execution, false otherwise
bool conditionalBranch(u_int splitWord[]){
    //TODO(Implement handling of a conditional branch instruction, Note you will need to further split input)
    return true;
}
int main(int argc, char **argv) {
    
    setupCPU();

    // Read from the file
    if (argc > 1) {
        if (!binaryFileLoader(argv[1])){
            printf("binary file loader failed on file %s\n", argv[1]);
            exit(1);
        }
    }

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
        if ((op0 & 14) == 10) {
            // Decode to: sf, bit, op0+, 'operand'
            // TODO(Replace with hashmap if possible)
            u_int splitInstruction[] = {
                    word >> 31,
                    ((1 << 30) & word) >> 30,
                    ((15 << 26) & word) >> 26,
                    ((7 << 23) & word) >> 23,
                    ((((2 << 19) - 1) << 5) & word) >> 5,
                    ((2 << 26) - 1) & word
            };
            assert(splitInstruction[2] == 5);

            if ((splitInstruction[0] || splitInstruction[1]) == 0) {
                unconditionalBranch(splitInstruction);
            }
            else if ((splitInstruction[0] && splitInstruction[1]) == 1) {
                registerBranch(splitInstruction);
            }
            else if (splitInstruction[0] == 0 && splitInstruction[1] == 1) {
                conditionalBranch(splitInstruction);
            }
            // no matching instruction throws error code 4
            else {
                printf("No branch instruction matching bit 31 and bit 30 values: %d, %d\n", splitInstruction[0], splitInstruction[1]);
                exit(4);
            }
        }
        // 100x -> Data processing (immediate) group
        else if ((op0 & 14) == 8) {
            // Decode to: sf, opc, 100, opi, operand, rd
            // TODO(Replace with hashmap if possible)
            u_int splitInstruction[] = {
                    word >> 31,
                    ((3 << 29) & word) >> 29,
                    ((7 << 26) & word) >> 26,
                    ((7 << 23) & word) >> 23,
                    ((((2 << 19) - 1) << 5) & word) >> 5,
                    31 & word
            };
            assert(splitInstruction[2] == 4);

            //Switch on opi
            switch (splitInstruction[3]){
                // opi is 010, then the instruction is an arithmetic instruction
                case (2): {
                    arithmeticInstruction(splitInstruction);
                    break;
                }
                // opi is 101, then the instruction is a wide move
                case (5): {
                    wideMoveInstruction(splitInstruction);
                    break;
                }
                // no matching instruction throws error code 5
                default: {
                    // TODO( Use the variable being switched on instead of recalculating)
                    printf("No data processing (immediate) instruction matching opi value: %d\n", splitInstruction[3]);
                    exit(5);
                }
            }
        }
        // x101 -> Data processing (register) group
        else if ((op0 & 7) == 5) {
            // Decode to: sf, opc, M, 10, 1, opr, rm, operand, rn, rd
            // TODO(Replace with hashmap if possible)
            u_int splitInstruction[] = {
                    word >> 31,
                    ((3 << 29) & word) >> 29,
                    ((1<<28) & word) >> 28,
                    ((3 << 26) & word) >> 26,
                    ((1<<25) & word) >> 25,
                    ((((2 << 5) - 1) << 21) & word) >> 21,
                    ((((2 << 6) - 1) << 16) & word) >> 16,
                    ((((2 << 7) - 1) << 10) & word) >> 10,
                    ((((2 << 5) - 1) << 5) & word) >> 5,
                    31 & word
            };
            assert(splitInstruction[3] == 2);
            assert(splitInstruction[4] == 1);

            if (splitInstruction[2] == 1 && splitInstruction[5] == 8) {
                multiplyInstruction(splitInstruction);
            }
            else if (splitInstruction[2] == 0) {
                if ((splitInstruction[5] >> 3) == 0) {
                    logicalShiftInstruction(splitInstruction);
                }
                else if ((9 & splitInstruction[5]) == 8) {
                    arithmeticShiftInstruction(splitInstruction);
                }
                // no matching instruction throws error code 6
                else {
                    // TODO(make a more general error code thrower to stop repeats like this)
                    printf("No data processing (register) instruction matching M and opr values: %d, %d\n", splitInstruction[2], splitInstruction[5]);
                    exit(6);
                }
            }
            // no matching instruction throws error code 6
            else {
                // TODO( Use the variable being switched on instead of recalculating)
                printf("No data processing (register) instruction matching M and opr values: %d, %d\n", splitInstruction[2], splitInstruction[5]);
                exit(6);
            }
        }
        // x1x0 -> Loads and stores group
        else if ((op0 & 5) == 4) {
            // Decode to: bit, sf, bit, op0+, U, 'operand', rt
            // TODO(Replace with hashmap if possible)
            u_int splitInstruction[] = {
                    word >> 31,
                    ((1 << 30) & word) >> 30,
                    ((1 << 29) & word) >> 29,
                    ((((2 << 5) - 1) << 25) & word) >> 25,
                    ((1 << 24) & word) >> 24,
                    ((((2 << 19) - 1) << 5) & word) >> 5,
                    31 & word
            };
            assert(splitInstruction[3] == 12);

            if ((splitInstruction[0] && splitInstruction[2]) == 1) {
                loadLiteralInstruction(splitInstruction);
            }
            else if ((splitInstruction[0] || splitInstruction[2] || splitInstruction[4]) == 0) {
               singleDataTransferInstruction(splitInstruction);
            }
            // no matching instruction throws error code 7
            else {
                printf("No load or store instruction matching bit 31, 29 and U values: %d, %d, %d\n", splitInstruction[0], splitInstruction[2], splitInstruction[4]);
                exit(7);
            }
        }
        // No matching group so throw error code 8
        else {
            printf("decode failed on non-matching op0 value: %d, with word value: %d\n", op0 , word);
            exit(8);
        }

        // Temp ending insurance TODO(remove when termination tested)
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