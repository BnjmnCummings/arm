#include "dataprocess.h"

extern processor CPU;

// stores reg value into mem address or loads value at mem address into reg, based on ldOrSt
static void loadOrStore(bool ldOrSt, bool bitMode, uint32_t regNumb, uint32_t addr) {
    if (ldOrSt) {
        write_register(bitMode, regNumb, read_memory(bitMode, addr));
    } else {
        write_memory(bitMode, addr, read_register(bitMode, regNumb));
    }
}

// loads into given register the value given by the PC + a given offset
static void loadLiteralInstruction(uint32_t splitWord[]){
    int64_t offset = ((int64_t) splitWord[5]) * 4;

    // find the target and source locations and check validity
    uint32_t targetRegister = splitWord[6];
    assert((0 <= targetRegister) && (targetRegister <= 30));
    int64_t loadFromAddress =((((int64_t) ((int32_t) (offset << 13)))) >> 13) + (int64_t) CPU.PC;
    assert((0 <= loadFromAddress) && (loadFromAddress < MEMORYSIZE));

    // load the value into the target register
    loadOrStore(1, splitWord[1], targetRegister, loadFromAddress);
}

// executes a load or store  based on a given decoded instruction
static void singleDataTransferInstruction(const uint32_t splitWord[]){
    uint32_t operand = splitWord[5];
    bool registerMode = splitWord[1];

    uint32_t brokenDownOperand[] = {
            operand >> 18,                      // 0: 0
            0b1 & (operand >> 17),              // 1: L
            ((2 << 11) - 1) & (operand >> 5),   // 2: offset
            0b11111 & operand                   // 3: xn
    };
    assert(brokenDownOperand[0] == 0);
    uint32_t targetRegister = splitWord[6];
    assert((0 <= targetRegister) && (targetRegister <= 30));
    uint32_t xn = brokenDownOperand[3];
    assert((0 <= xn) && (xn <= 30));
    uint64_t xnValue = read_register(1, xn);

    // unsigned immediate offset
    if (splitWord[4]) {
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
        uint32_t offset = brokenDownOperand[2];
        uint32_t brokenDownOffset[] = {
                offset >> 11,                       // bit
                ((0b1 << 9) - 1) & (offset >> 2),   // simm9
                0b1 & (offset >> 1),                // I
                0b1 & offset                        // bit
        };
        // Register offset
        if (brokenDownOffset[0]) {

            uint32_t xm = 0b11111 & (brokenDownOffset[1] >> 4);
            assert((0 <= xm) && (xm <= 30));
            uint64_t xmValue = read_register(1, xm);

            uint64_t addr = xmValue + xnValue;
            assert((0 <= addr) && (addr < MEMORYSIZE));
            loadOrStore(brokenDownOperand[1], registerMode, targetRegister, addr);
        }
        // pre/post indexing
        else {
            int64_t simm9 = (((int64_t) ((int16_t) (brokenDownOffset[1] << 7)))) >> 7;
            // pre indexed
            if (brokenDownOffset[2]) {

                assert((0 <= xnValue) && (xnValue < MEMORYSIZE));
                assert((-256 <= simm9) && (simm9 <= 255));
                uint64_t readWriteValue = xnValue + simm9;

                loadOrStore(brokenDownOperand[1], registerMode, targetRegister, readWriteValue);
                write_register(1, xn, readWriteValue);
            }
            // post indexed
            else {
                assert((0 <= xnValue) && (xnValue < MEMORYSIZE));
                loadOrStore(brokenDownOperand[1], registerMode, targetRegister, xnValue);

                assert((-256 <= simm9) && (simm9 <= 255));
                uint64_t newValue = xnValue + simm9;

                write_register(1, xn, newValue);
            }
        }
    }
}

// decodes load/store instruction into constituent parts, ands passes them into corresponding execution function
bool decode_data_transfer(uint32_t word) {
    uint32_t splitInstruction[] = {
            word >> 31,                         // 0: b31
            0b1  & (word >> 30),                // 1: sf
            0b1 & (word >> 29),                 // 2: b29
            0b1111 & (word >> 25),              // 3: b25-28
            0b1 & (word >> 24),                 // 4: U
            ((0b1 << 18) - 1) & (word >> 5),    // 5: operand
            0b11111 & word                      // 6: rt
    };

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
    increment_pc();
    return true;
}