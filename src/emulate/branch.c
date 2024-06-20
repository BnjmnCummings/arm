#include "branch.h"

extern processor CPU;

// offsets Program Counter by literal value
static void unconditionalBranch(const uint32_t splitWord[]){
    // simm26 value
    int64_t offset = splitWord[3] * 4;

    // find the new PC value and check validity
    int64_t newPCAddressLocation = (int64_t) CPU.PC + offset;
    assert((0 <= newPCAddressLocation) && (newPCAddressLocation < MEMORYSIZE));

    // update the PC value
    CPU.PC += offset;
}

// set the PC to the value in a specified register
static void registerBranch(uint32_t splitWord[]){
    // xn value
    uint32_t registerNumb = 0b11111 & (splitWord[3] >> 5);

    // find the new PC value and check validity
    assert((0 <= registerNumb) && (registerNumb <= 31));
    uint64_t newPCAddressLocation = read_register(splitWord[0], registerNumb);
    assert((0 <= newPCAddressLocation) && (newPCAddressLocation < MEMORYSIZE));

    // update the PC value
    CPU.PC = newPCAddressLocation;
}

// checks and returns if specified condition is satisfied by PSTATE values
static bool conditionalBranchConditionCheck(uint32_t conditionCode){
    pstate currentPSTATE = CPU.PSTATE;
    bool negativeEqOverflow = (currentPSTATE.Negative == currentPSTATE.Overflow);

    switch (conditionCode) {
        case (0b0000):
            return (currentPSTATE.Zero);
        case (0b0001):
            return !(currentPSTATE.Zero);
        case (0b1010):
            return negativeEqOverflow;
        case (0b1011):
            return !negativeEqOverflow;
        case (0b1100):
            return ((!currentPSTATE.Zero) && negativeEqOverflow);
        case (0b1101):
            return !((!currentPSTATE.Zero) && negativeEqOverflow);
        case (0b1110):
            return true;
        default: {
            fprintf(stderr, "In conditionalBranchConditionCheck undefined condition encoding given with value: %d", conditionCode);
            exit(10);
        }
    }
}

// applies offset the the PC given by a literal if the given condition is satisfied
static void conditionalBranch(const uint32_t splitWord[]){
    uint32_t operand = splitWord[3];

    uint32_t brokenDownOperand[] = {
            operand >> 24,                      // 00
            ((2 << 18) - 1) & (operand >> 5),   // simm19
            0b1 & (operand >> 4),               // 0
            0b1111 & operand                    // condition encoding
    };
    assert(brokenDownOperand[0] == 0 && brokenDownOperand[2] == 0);

    if (conditionalBranchConditionCheck(brokenDownOperand[3])){

        int64_t offset = ((int64_t) ((int32_t) (brokenDownOperand[1] << 13))) >> 11;

        // find the new PC value and check validity
        uint32_t newPCAddressLocation = CPU.PC + offset;
        assert((0 <= newPCAddressLocation) && (newPCAddressLocation < MEMORYSIZE));

        // update the PC value
        CPU.PC += offset;
    } else {
        increment_pc();
    }
}

// breaks instruction into constituent parts, and passes them into the corresponding execution function
bool decode_branch(uint32_t word) {
    uint32_t splitInstruction[] = {
            word >> 31,                 // sf
            0b1  & (word >> 30),        // bit
            0b1111 & (word >> 26),      // op0+
            ((2 << 25) - 1) & word      // operand
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
        return false;
    }
    return true;
    // No PC increment as these instructions are all branches
}