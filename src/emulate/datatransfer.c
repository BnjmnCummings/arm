#include "dataprocess.h"

extern processor CPU;

static void loadOrStore(bool ldOrSt, bool bitMode, uint32_t regNumb, uint32_t addr) {
    if (ldOrSt) {
        uint64_t x = readMemory(bitMode, addr);
        printf("loading %lu in mode %u at addr %u\n", x, bitMode, addr);
        writeRegister(bitMode, regNumb, readMemory(bitMode, addr));
    } else {
        uint64_t x = readRegister(bitMode, regNumb);
        printf("storing %lu from reg %u in mode %u at addr %u \n", x, regNumb, bitMode, addr);
        writeMemory(bitMode, addr, readRegister(bitMode, regNumb));
    }
}

// loadLiteralInstruction is a function taking the split instruction (word) as argument
// It completed the instruction on the CPU
// It returns true on a successful execution, false otherwise
static void loadLiteralInstruction(u_int32_t splitWord[]){
    // Decode to: bit (1 bit), sf (1 bit), bit (1 bit), op0+ (4 bit), U (1 bit), 'operand' (19 bit), rt (5 bit)
    // Note this may need sign extended to 64 bit manually
    int64_t offset = ((int64_t) splitWord[5]) * 4;

    // find the target and source locations and check validity
    u_int32_t targetRegister = splitWord[6];
    assert((0 <= targetRegister) && (targetRegister <= 30));
    int64_t loadFromAddress =((((int64_t) ((int32_t) (offset << 13)))) >> 13) + (int64_t) CPU.PC;
    assert((0 <= loadFromAddress) && (loadFromAddress < MEMORYSIZE));

    // load the value into the target register and return true
    loadOrStore(1, splitWord[1], targetRegister, loadFromAddress);
}

// singleDataTransferInstruction is a function taking the split instruction (word) as argument
// It completed the instruction on the CPU
// It returns true on a successful execution, false otherwise
static void singleDataTransferInstruction(const u_int32_t splitWord[]){
// Decode to: bit (1 bit), sf (1 bit), bit (1 bit), op0+ (4 bit), U (1 bit), 'operand' (19 bit), rt (5 bit)
    u_int32_t operand = splitWord[5];
    bool registerMode = splitWord[1];

    // Broken down into: 1 zero bits, L (1 bit), offset (12 bit), xn (4 bit)
    u_int32_t brokenDownOperand[] = {
            operand >> 18,                      // 0: 0
            0b1 & (operand >> 17),              // 1: L
            ((2 << 11) - 1) & (operand >> 5),   // 2: offset
            0b11111 & operand                   // 3: xn
    };
    assert(brokenDownOperand[0] == 0);
    u_int32_t targetRegister = splitWord[6];
    assert((0 <= targetRegister) && (targetRegister <= 30));
    u_int32_t xn = brokenDownOperand[3];
    assert((0 <= xn) && (xn <= 30));
    u_int64_t xnValue = readRegister(1, xn);

    // U == 1 then unsigned immediate offset
    if (splitWord[4]) {
        printf("U = 1\n");
        uint64_t offset = brokenDownOperand[2] * 4;
        // 64 bit mode
        if (splitWord[1]) {
            offset *= 2;
        }
        // 32 bit mode requires no change

        uint32_t addr = xnValue + offset;
        assert((0 <= addr) && (addr < MEMORYSIZE));
        loadOrStore(brokenDownOperand[1], registerMode, targetRegister, addr);
    }
    else {
        u_int32_t offset = brokenDownOperand[2];
        // broken down into: bit (1 bit), simm9? (9 bit), I (1 bit), bit (1 bit)
        u_int32_t brokenDownOffset[] = {
                offset >> 11,
                ((0b1 << 9) - 1) & (offset >> 2),
                0b1 & (offset >> 1),
                0b1 & offset
        };
        // Register offset
        if (brokenDownOffset[0]) {
            printf("register offset\n");
            // assert((0b1111 & brokenDownOffset[1]) == 0b0110);

            u_int32_t xm = 0b11111 & (brokenDownOffset[1] >> 4);
            assert((0 <= xm) && (xm <= 30));
            u_int64_t xmValue = readRegister(1, xm);

            u_int64_t addr = xmValue + xnValue;
            assert((0 <= addr) && (addr < MEMORYSIZE));
            loadOrStore(brokenDownOperand[1], registerMode, targetRegister, addr);
        }
        // must be pre/post indexing
        else {
            int64_t simm9 = (((int64_t) ((int16_t) (brokenDownOffset[1] << 7)))) >> 7;
            printf("%u to %ld\n", brokenDownOffset[1], simm9);
            // I == 1 so pre indexed
            if (brokenDownOffset[2]) {

                printf("pre-indexing\n");
                assert((0 <= xnValue) && (xnValue < MEMORYSIZE));
                assert((-256 <= simm9) && (simm9 <= 255));
                u_int64_t readWriteValue = xnValue + simm9;

                loadOrStore(brokenDownOperand[1], registerMode, targetRegister, readWriteValue);
                writeRegister(1, xn, readWriteValue);
            }
                // I must be 0 so post indexed
            else {
                printf("post-indexing\n");
                assert((0 <= xnValue) && (xnValue < MEMORYSIZE));
                loadOrStore(brokenDownOperand[1], registerMode, targetRegister, xnValue);

                assert((-256 <= simm9) && (simm9 <= 255));
                u_int64_t newValue = xnValue + simm9;

                writeRegister(1, xn, newValue);
            }
        }
        // in this situation a pattern must be matched so error message needed for nothing matching
    }
}

bool decodeDataTransfer(uint32_t word) {
    printf("Entering loads and stores group with word: %u\n" ,word);
    // Decode to: bit (1 bit), sf (1 bit), bit (1 bit), op0+ (4 bit), U (1 bit), 'operand' (19 bit), rt (5 bit)
    u_int32_t splitInstruction[] = {
            word >> 31,                         // 0: b31
            0b1  & (word >> 30),                // 1: sf
            0b1 & (word >> 29),                 // 2: b29
            0b1111 & (word >> 25),              // 3: b25-28
            0b1 & (word >> 24),                 // 4: U
            ((0b1 << 18) - 1) & (word >> 5),    // 5: operand
            0b11111 & word                      // 6: rt
    };
    // for (int i = 0; i <= 6; i ++ ){
    //     printf("splitInstruction at %d is: %d\n", i, splitInstruction[i]);
    // }

    if ((splitInstruction[0] && splitInstruction[2]) == 1) {
        singleDataTransferInstruction(splitInstruction);
    }
    else if ((splitInstruction[0] || splitInstruction[2] || splitInstruction[4]) == 0) {
        loadLiteralInstruction(splitInstruction);
    }
        // no matching instruction throws error code 7
    else {
        printf("No load or store instruction matching bit 31, 29 and U values: %d, %d, %d\n", splitInstruction[0], splitInstruction[2], splitInstruction[4]);
        return false;
    }
    incrementPC();
    return true;
}