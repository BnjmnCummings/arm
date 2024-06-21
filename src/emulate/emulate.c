#include "emulate.h"
#include "filewriter.h"
#include "processor.h"
#include "dataprocess.h"
#include "datatransfer.h"
#include "branch.h"

extern processor CPU;

// returns a mask of 1 bits from start bit to end bit
uint64_t get_mask(int start, int end) {
    if (start >= end) { return 0; }
    return (((uint64_t) 2 << (end - start)) - 1) << start;
}

// loads binary data from given filename into memory
static bool binaryFileLoader(char *fileName){

    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        fprintf( stderr, "Can't read given input file\n" );
        return false;
    }
    int currentMemoryAddress = 0;
    uint ch;
    while( (ch = getc(file)) != EOF ) {
        CPU.memory[currentMemoryAddress] = ch;
        currentMemoryAddress ++;
    }

    fclose(file);
    return true;
}


// simulates the FDE cycle, return 0 on a successful halt and an error code on failure
static int fDECycle(void){
    while (true){
        // Fetch
        uint32_t pcValue = CPU.PC;

        // Throw error code 2 if pc value points to outside memory
        if (pcValue > ((MEMORYSIZE) - 3)){
            printf("fetch failed on nonexistent memory location with pc value: %u\n", pcValue);
            return 2;
        }

        // read word in little endian
        uint32_t word = read_memory(0, pcValue);

        // check if the instruction is a halt
        if (word == 0x8a000000){
            break;
        }

        // Decode:
        uint32_t op0 = 0b1111 & (word >> 25);

        // 101x -> Branch group
        if ((op0 & 0b1110) == 0b1010) {
            if (!decode_branch(word)) {
                return 4;
            }
        }
        // 100x -> Data processing (immediate) group
        else if ((op0 & 0b1110) == 0b1000) {
            if (!decode_data_immediate(word)) {
                return 5;
            }
        }
        // x101 -> Data processing (register) group
        else if ((op0 & 0b111) == 0b101) {
            if (!decode_data_register(word)) {
                return 6;
            }
        }
        // x1x0 -> Loads and stores group
        else if ((op0 & 0b101) == 0b100) {
            if (!decode_data_transfer(word)) {
                return 7;
            }
        }
            // No matching group so throw error code 8
        else {
            printf("decode failed on non-matching op0 value: %u, with word value: %u\n", op0 , word);
            return 8;
        }
    }
    return 0;
}

// sets up CPU, loads binary data from given file into memory,
// runs the FDE cycle, then prints the CPU state to given file (or stdout)
int main(int argc, char **argv) {

    setup_cpu();

    // Read from the file
    if (argc > 1) {
        if (!binaryFileLoader(argv[1])){
            printf("binary file loader failed on file %s\n", argv[1]);
            exit(1);
        }
    }

    // run FDE cycle
    int e;
    if ((e = fDECycle()) != 0){
        printf("FDE cycle failed with error code %d\n", e);
        write_cpu(stdout);
        exit(2);
    }

    // print CPU state to file or stdout
    if (argc > 2) {
        FILE *file = fopen(argv[2], "w");

        if (file == NULL) {
            fprintf( stderr, "Can't read given output file\n" );
            exit(11);
        }
        write_cpu(file);
    }
    else {
        write_cpu(stdout);
    }

    return EXIT_SUCCESS;
}
