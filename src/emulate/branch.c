#include "branch.h"

extern processor CPU;

// unconditionalBranch is a function taking the split instruction (word) as argument
// It applies an offset to the current PC value
// It returns true on a successful execution, false otherwise
static void unconditionalBranch(const u_int32_t splitWord[]){
    // Decoded to: sf (1 bit), bit (1 bit), op0+ (4 bit), 'operand' (26 bit)
    // Note this may need sign extended to 64 bit manually
    int64_t offset = splitWord[3] * 4;

    // find the new PC value and check validity
    int64_t newPCAddressLocation = (int64_t) CPU.PC + offset;
    assert((0 <= newPCAddressLocation) && (newPCAddressLocation < MEMORYSIZE));

    // update the PC value and return true
    CPU.PC += offset;
}

// registerBranch is a function taking the split instruction (word) as argument
// It changes the PC to a value specified in a specified register
// It returns true on a successful execution, false otherwise
static void registerBranch(u_int32_t splitWord[]){
    // Decoded to: sf (1 bit), bit (1 bit), op0+ (4 bit), 'operand' (26 bit)
    // Note this may need sign extended to 64 bit manually
    u_int32_t registerNumb = 0b11111 & (splitWord[3] >> 5);

    // find the new PC value and check validity
    assert((0 <= registerNumb) && (registerNumb <= 31));
    u_int64_t newPCAddressLocation = readRegister(splitWord[0], registerNumb);
    assert((0 <= newPCAddressLocation) && (newPCAddressLocation < MEMORYSIZE));

    // update the PC value and return true
    CPU.PC = newPCAddressLocation;
}

// conditionalBranchConditionCheck is a function taking the condition on the branch as argument
// It checks if the encoded condition is true for the current PSTATE
// It returns whether the condition is true or not
static bool conditionalBranchConditionCheck(u_int32_t conditionCode){
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

// conditionalBranch is a function taking the split instruction (word) as argument
// It checks an encoded condition and if true, it applies an indicated offset to the PC
// It returns true on a successful execution, false otherwise
static void conditionalBranch(const u_int32_t splitWord[]){
    // Decoded to: sf (1 bit), bit (1 bit), op0+ (4 bit), 'operand' (26 bit)
    u_int32_t operand = splitWord[3];

    // Broken down into: 2 zero bits, simm 19 (19 bit), 0 bit, condition (4 bit)
    u_int32_t brokenDownOperand[] = {
            operand >> 24,
            ((2 << 18) - 1) & (operand >> 5), // simm19
            0b1 & (operand >> 4),
            0b1111 & operand // condition encoding
    };
    assert(brokenDownOperand[0] == 0 && brokenDownOperand[2] == 0);

    if (conditionalBranchConditionCheck(brokenDownOperand[3])){

        // Note this may need sign extended to 64 bit manually
        int64_t offset = ((int64_t) ((int32_t) (brokenDownOperand[1] << 13))) >> 11;

        // find the new PC value and check validity
        uint32_t newPCAddressLocation = CPU.PC + offset;
        printf("from offset %ld and simm19 %u, new PC address location: %u\n", offset, brokenDownOperand[1], newPCAddressLocation);
        assert((0 <= newPCAddressLocation) && (newPCAddressLocation < MEMORYSIZE));

        // update the PC value
        CPU.PC += offset;
    } else {
        CPU.PC += 4;
    }
}

bool decodeBranch(uint32_t word) {
    printf("Entering branch group with word: %u\n" ,word);
    // Decode to: sf (1 bit), bit (1 bit), op0+ (4 bit), 'operand' (26 bit)
    // unsure if changing this to int is correct vs uint
    u_int32_t splitInstruction[] = {
            word >> 31,
            0b1  & (word >> 30),
            0b1111 & (word >> 26),
            ((2 << 25) - 1) & word
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