#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <stdint.h>
#include "filewriter.h"
#include "processor.h"

#define BYTESIZE 8
//TODO(IMPLEMENT MACRO FOR INCREMENT PC AND DEFINE WORDSIZE)

// TODO(figure out correct initialisation)
pstate initialPstate = {false, true, false, false};

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

u_int64_t getMask(int start, int end) {
    assert(start <= end);
    return ((u_int64_t) 2 << (end - start)) << start;
}

// binaryFileLoader is a function taking the filename as an argument
// It copies the contents of the file into the CPU's memory
// It returns true on a successful execution, false otherwise
bool binaryFileLoader(char *fileName){

    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        fprintf( stderr, "Can't read given input file\n" );
        return false;
    }

    int currentMemoryAddress = 0;
    u_int32_t ch;
    while( (ch = getc(file)) != EOF ) {
        CPU.memory[currentMemoryAddress] = ch;
        currentMemoryAddress ++;
    }

    fclose(file);
    return true;
}

u_int32_t read32BitModeRegister(u_int32_t registerNumb) {
    assert((0 <= registerNumb) && (registerNumb <= 30));
    u_int64_t bitMask32 = ((u_int64_t) 1 << 32) - 1;
    return (u_int32_t) (CPU.generalPurpose[registerNumb] & bitMask32);
}

u_int64_t readRegister(bool in64BitMode, u_int32_t registerNumb){
    assert((0 <= registerNumb) && (registerNumb <= 30));
    if (in64BitMode) {
        return CPU.generalPurpose[registerNumb];
    }
    return read32BitModeRegister(registerNumb);
}

bool write32BitModeRegister(u_int32_t registerNumb, u_int64_t data) {
    assert((0 <= registerNumb) && (registerNumb <= 30));
    u_int64_t bitMask32 = (((u_int64_t) 1) << 32) - 1;
    CPU.generalPurpose[registerNumb] = data & bitMask32;
    return true;
}

bool writeRegister(bool in64BitMode, u_int32_t registerNumb, u_int64_t data){
    assert((0 <= registerNumb) && (registerNumb <= 30));
    if (in64BitMode) {
        CPU.generalPurpose[registerNumb] = data;
        return true;
    }
    return write32BitModeRegister(registerNumb, data);
}

// read word in little endian TODO(TEST THIS DOESNT NEED REVERSING)
u_int32_t readMemory(u_int64_t memoryAddress){
    assert((0 <= memoryAddress) && (memoryAddress < MEMORYSIZE));
    u_int32_t word = CPU.memory[memoryAddress + 3] << (BYTESIZE * 3);
    for (int i = 2; i >= 0; i--){
        word &= CPU.memory[memoryAddress + i] << (BYTESIZE * i);
    }
    return word;
}

bool writeMemory(bool in64BitMode, u_int64_t memoryAddress, u_int64_t data) {
    assert((0 <= memoryAddress) && (memoryAddress < MEMORYSIZE));
    u_int32_t byteSizeMask = ((0b1 << BYTESIZE) - 1);
    for (int i = 0; i < 4 + (4 * in64BitMode); i ++){
        CPU.memory[memoryAddress + i] = data & (byteSizeMask << (BYTESIZE * i));
    }
    return true;
}

// arithmeticInstruction is a function taking the split instruction (word) as argument
// It completed the instruction on the CPU
// It returns true on a successful execution, false otherwise
bool arithmeticInstruction(u_int32_t splitWord[]){
    u_int32_t operand[] = {
        splitWord[4] << 17,
        ((((1 << 11) - 1) << 5) & splitWord[4]) >> 5,
        splitWord[4] & 0b11111
    };
    uint64_t op2 = operand[1];
    if (operand[0] == 1) {
        op2 = op2 << 12;
    }
    uint64_t rn = readRegister(splitWord[0], operand[2]);

    uint64_t result;
    switch (splitWord[1])
    {
    //add
    case (0b00):
        result = rn + op2;
        break;
    //adds 11 - 110000 - 111111
    case (0b01):
        result = rn + op2;
        uint64_t sigBitShift = (32 + (32 * splitWord[0]) - 1);
        int check = (int) rn + (int) op2;
        CPU.PSTATE.Negative = result >> sigBitShift;
        CPU.PSTATE.Zero = result == 0;
        CPU.PSTATE.Carry = check >= (1 >> sigBitShift);
        CPU.PSTATE.Overflow = CPU.PSTATE.Carry;
        break;
    //sub
    case (0b10):
        result = rn - op2;
        break;
    //subs
    case (0b11):
        result = rn - op2;
        CPU.PSTATE.Negative = result < 0;
        CPU.PSTATE.Zero = result == 0;
        CPU.PSTATE.Carry = 0; //TODO - check for carry
        CPU.PSTATE.Overflow = rn < op2; //TODO - check for overflow
        break;
    default:
        break;
    }

    // write to Rd
    writeRegister(splitWord[0], splitWord[5], result);

    return true;
}

// wideMoveInstruction is a function taking the split instruction (word) as argument
// It completed the instruction on the CPU
// It returns true on a successful execution, false otherwise
bool wideMoveInstruction(u_int32_t splitWord[]){
    u_int32_t operand[] = {
        splitWord[4] << 16,
        splitWord[4] & ((1 << 15) - 1)
    };
    uint64_t sh = operand[0] * 16;
    int op = operand[1] << sh;
    switch (splitWord[1])
    {
    //movk
    case (0b00):
        writeRegister(splitWord[0], splitWord[5], ~op);
        break;
    //movz
    case (0b10):
        writeRegister(splitWord[0], splitWord[5], op);
        break;
    //movk
    case (0b11):{
        int rd = readRegister(splitWord[0], splitWord[5]);
        uint64_t result = (((rd >> (sh + 15) )<< (sh + 15)) | (operand[1] << sh)) | (rd & ((1 << sh) - 1));
        writeRegister(splitWord[0], splitWord[5], result);
        break;
    }
    default:
        break;
    }
    return true;
}

u_int64_t shift(int val, int inc, int type, int sf) {
    u_int64_t res;
    int maskVal = (32 + (32 * sf) - 1);
    // lsl shift
    if (type == 0b00) {
        res = val << inc;
    }
        // lsr shift
    else if (type == 0b01) {
        res = val >> inc;
    }
    // asr shift
    else if (type == 0b10) {
        res = val >> inc;
        if (val >> maskVal) {
            res += getMask(maskVal - inc, maskVal);
        }
    }
    // ror shift
    else {
        res = val >> inc;
        res += getMask(0, inc) << (maskVal - inc);
    }
    return res;
}

bool registerParser(int opc, int rd, u_int64_t rn, u_int64_t op, bool sf, bool negate) {
    if (negate) {
        op = ~op;
    }
    switch (opc) {
        case 0b00: // Bitwise M
            writeRegister(sf, rd, rn & op);
            break;
        case 0b01:
            writeRegister(sf, rd, rn | op);
            break;
        case 0b10:
            writeRegister(sf, rd, rn ^ op);
            break;
        case 0b11:
            writeRegister(sf, rd, rn & op);
            CPU.PSTATE.Negative = (rn & op) >> (32 + (32 * sf) - 1);
            CPU.PSTATE.Zero = (rn & op) == 0;
            break;
    }
    return true;
}

// multiplyInstruction is a function taking the split instruction (word) as argument
// It completed the instruction on the CPU
// It returns true on a successful execution, false otherwise
bool multiplyInstruction(u_int splitWord[]){
    u_int negate = splitWord[5] & 0x1;
    u_int sf = splitWord[0];

    u_int64_t rn = readRegister(sf, splitWord[8]);
    u_int64_t rm = readRegister(sf, splitWord[6]);

    u_int x = splitWord[7] >> 5;
    u_int ra = splitWord[7] & 0b011111;

    if (x) {
        writeRegister(sf, splitWord[9], ra - (rn * rm));
    } else {
        writeRegister(sf, splitWord[9], ra + (rn * rm));
    }
    return true;
}

// logicalShiftInstruction is a function taking the split instruction (word) as argument
// It completed the instruction on the CPU
// It returns true on a successful execution, false otherwise
bool logicalShiftInstruction(u_int splitWord[]) {
    u_int opc = splitWord[1];
    u_int shiftMask = (splitWord[5] >> 2) & 0x3;
    u_int negate = splitWord[5] & 0x1;
    u_int sf = splitWord[0];
    u_int64_t firstOp = splitWord[7];
    u_int64_t rn = readRegister(sf, splitWord[8]);
    u_int64_t rm = readRegister(sf, splitWord[6]);

    if (!sf) {
        firstOp &= 31;
    }

    u_int64_t secOp = shift(rm, firstOp, shiftMask, sf);

    registerParser(opc, splitWord[9], rn, secOp, sf, negate);

    CPU.PSTATE.Carry = 0;
    CPU.PSTATE.Overflow = 0;
    return true;
}

// arithmeticShiftInstruction is a function taking the split instruction (word) as argument
// It completed the instruction on the CPU
// It returns true on a successful execution, false otherwise
bool arithmeticShiftInstruction(u_int splitWord[]){
    logicalShiftInstruction(splitWord);
    //TODO(set arithmetic PSTATE flags)
    return true;
}

// loadLiteralInstruction is a function taking the split instruction (word) as argument
// It completed the instruction on the CPU
// It returns true on a successful execution, false otherwise
bool loadLiteralInstruction(u_int32_t splitWord[]){
    // Decode to: bit (1 bit), sf (1 bit), bit (1 bit), op0+ (4 bit), U (1 bit), 'operand' (19 bit), rt (5 bit)
    // Note this may need sign extended to 64 bit manually
    int offset = splitWord[5] * 4;

    // find the target and source locations and check validity
    u_int32_t targetRegister = splitWord[6];
    assert((0 <= targetRegister) && (targetRegister <= 30));
    int loadFromAddress = CPU.PC + offset;
    assert((0 <= loadFromAddress) && (loadFromAddress < MEMORYSIZE));

    // load the value into the target register and return true
    writeRegister(splitWord[1], targetRegister, readMemory(loadFromAddress));
    return true;
}

// singleDataTransferInstruction is a function taking the split instruction (word) as argument
// It completed the instruction on the CPU
// It returns true on a successful execution, false otherwise
bool singleDataTransferInstruction(const u_int32_t splitWord[]){
    //TODO(lots of casts here uint to int especially when reading xm ,xn. These def need a check)

    // Decode to: bit (1 bit), sf (1 bit), bit (1 bit), op0+ (4 bit), U (1 bit), 'operand' (19 bit), rt (5 bit)
    u_int32_t operand = splitWord[5];
    bool registerMode = splitWord[1];

    // Broken down into: 1 zero bits, L (1 bit), offset (12 bit), xn (4 bit)
    u_int32_t brokenDownOperand[] = {
            operand >> 18,
            ((0b1 << 17) & operand) >> 17,
            ((((2 << 12) - 1) << 5) & operand) >> 5,
            (0b11111) & operand
    };
    assert(brokenDownOperand[0] == 0);
    u_int32_t targetRegister = splitWord[6];
    assert((0 <= targetRegister) && (targetRegister <= 30));
    u_int32_t xn = brokenDownOperand[3];
    assert((0 <= xn) && (xn <= 30));
    u_int64_t xnValue = readRegister(registerMode, xn);

    // U == 1 then unsigned immediate offset
    if (splitWord[4]) {
        u_int32_t offset = brokenDownOperand[2] * 4;
        // 64 bit mode
        if (splitWord[1]) {
            offset *= 2;
        }
        // 32 bit mode requires no change

        u_int64_t loadFromAddress = xnValue + offset;
        assert((0 <= loadFromAddress) && (loadFromAddress < MEMORYSIZE));
        writeRegister(registerMode, targetRegister, readMemory(loadFromAddress));
    }
    else {
        u_int32_t offset = brokenDownOperand[2];
        // broken down into: bit (1 bit), simm9? (9 bit), I (1 bit), bit (1 bit)
        u_int32_t brokenDownOffset[] = {
                operand >> 11,
                ((((2 << 8) - 1) << 2) & offset) >> 2,
                ((0b1 << 1) & operand) >> 1,
                (0b1) & offset
        };
        assert(brokenDownOffset[0] == brokenDownOffset[3]);
        // Register offset
        if (brokenDownOffset[0]) {
            assert((0b1111 & brokenDownOffset[1]) == 6);

            u_int32_t xm = (((0b11111) << 4) & brokenDownOffset[1]) >> 4;
            assert((0 <= xm) && (xm <= 30));
            u_int64_t xmValue = readMemory(xm);

            u_int64_t loadFromAddress = xmValue + xnValue;
            assert((0 <= loadFromAddress) && (loadFromAddress < MEMORYSIZE));
            writeRegister(registerMode, targetRegister, readMemory(loadFromAddress));
        }
        // must be pre/post indexing
        else {
            int simm9 = brokenDownOffset[1];
            // I == 1 so pre indexed
            if (brokenDownOffset[2]) {
                assert((0 <= xnValue) && (xnValue < MEMORYSIZE));
                assert((-256 <= simm9) && (simm9 <= 255));
                u_int64_t readWriteValue = xnValue + simm9;

                writeRegister(registerMode, targetRegister, readMemory(readWriteValue));
                writeMemory(registerMode, xnValue, readWriteValue);
            }
                // I must be 0 so post indexed
            else {
                assert((0 <= xnValue) && (xnValue < MEMORYSIZE));
                writeRegister(registerMode, targetRegister, readMemory(xnValue));

                assert((-256 <= simm9) && (simm9 <= 255));
                u_int64_t newValue = xnValue + simm9;

                writeMemory(registerMode, xnValue, newValue);
            }
        }
        // in this situation a pattern must be matched so error message needed for nothing matching
    }
    return true;
}

// unconditionalBranch is a function taking the split instruction (word) as argument
// It applies an offset to the current PC value
// It returns true on a successful execution, false otherwise
bool unconditionalBranch(const u_int32_t splitWord[]){
    // Decoded to: sf (1 bit), bit (1 bit), op0+ (4 bit), 'operand' (26 bit)
    // Note this may need sign extended to 64 bit manually
    int offset = splitWord[3] * 4;

    // find the new PC value and check validity
    int newPCAddressLocation = CPU.PC + offset;
    assert((0 <= newPCAddressLocation) && (newPCAddressLocation < MEMORYSIZE));

    // update the PC value and return true
    CPU.PC = readMemory(CPU.PC + offset);
    return true;
}

// registerBranch is a function taking the split instruction (word) as argument
// It changes the PC to a value specified in a specified register
// It returns true on a successful execution, false otherwise
bool registerBranch(u_int32_t splitWord[]){
    // Decoded to: sf (1 bit), bit (1 bit), op0+ (4 bit), 'operand' (26 bit)
    // Note this may need sign extended to 64 bit manually
    u_int32_t registerNumb = (((2 << 5) - 1) << 5) & splitWord[3];

    // find the new PC value and check validity
    assert((0 <= registerNumb) && (registerNumb <= 30));
    u_int32_t newPCAddressLocation = readMemory(registerNumb);
    assert((0 <= newPCAddressLocation) && (newPCAddressLocation < MEMORYSIZE));

    // update the PC value and return true
    CPU.PC = newPCAddressLocation;
    return true;
}

// conditionalBranchConditionCheck is a function taking the condition on the branch as argument
// It checks if the encoded condition is true for the current PSTATE
// It returns whether the condition is true or not
bool conditionalBranchConditionCheck(u_int32_t conditionCode){
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
bool conditionalBranch(u_int32_t splitWord[]){
    // Decoded to: sf (1 bit), bit (1 bit), op0+ (4 bit), 'operand' (26 bit)
    u_int32_t operand = splitWord[3];

    // Broken down into: 2 zero bits, simm 19 (19 bit), 0 bit, condition (4 bit)
    u_int32_t brokenDownOperand[] = {
            operand >> 24,
            ((((2 << 19) - 1) << 5) & operand) >> 5, // simm19
            ((1 << 4) & operand) >> 4,
            (16 - 1) & operand // condition encoding
    };
    assert(brokenDownOperand[0] == 0 && brokenDownOperand[2] == 0);

    if (conditionalBranchConditionCheck(brokenDownOperand[3])){

        // Note this may need sign extended to 64 bit manually
        int offset = brokenDownOperand[1] * 4;

        // find the new PC value and check validity
        int newPCAddressLocation = CPU.PC + offset;
        assert((0 <= newPCAddressLocation) && (newPCAddressLocation < MEMORYSIZE));

        // update the PC value
        CPU.PC = readMemory(CPU.PC + offset);
    }
    // successful execution no matter the state of condition so return true
    return true;
}

// fDECycle is a function that takes no inputs
// It simulates the FDE cycle using other helper functions for each instruction
// It returns 0 on a successful run or an error code (0 < num < 8) on a fail with a relevant message
int fDECycle(void){
    while (true){
        // Fetch
        int pcValue = CPU.PC;

        // Throw error code 2 if pc value points to outside memory
        //TODO(if pc becomes unsigned int remove the <0 check)
        if (pcValue > ((MEMORYSIZE) - 3) || pcValue < 0){
            printf("fetch failed on nonexistent memory location with pc value: %d\n", pcValue);
            return 2;
        }

        // Throw error code 3 if pc points to a memory location not at the start of a word
        if (pcValue % 4 != 0){
            printf("fetch failed on nonaligned memory location with pc value: %d\n", pcValue);
            return 3;
        }

        // read word in little endian
        u_int32_t word = readMemory(pcValue);

        // check if the instruction is a halt (Untested)
        if (word == 0x8a000000){
            break;
        }

        // Decode:
        u_int32_t op0 = ((0b1111 << 24) & word) >> 24;

        // 101x -> Branch group
        if ((op0 & 0b1110) == 0b1100) {
            // Decode to: sf (1 bit), bit (1 bit), op0+ (4 bit), 'operand' (26 bit)
            // TODO(Replace with hashmap if possible)
            // unsure if changing this to int is correct vs uint
            u_int32_t splitInstruction[] = {
                    word >> 31,
                    ((1 << 30) & word) >> 30,
                    ((15 << 26) & word) >> 26,
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
                return 4;
            }

            // No PC increment as these instructions are all branches
        }
            // 100x -> Data processing (immediate) group
        else if ((op0 & 0b1110) == 0b1000) {
            // Decode to: sf, opc, 100, opi, operand, rd
            // TODO(Replace with hashmap if possible)
            u_int32_t splitInstruction[] = {
                    word >> 31,
                    ((0b11 << 29) & word) >> 29,
                    ((0b111 << 26) & word) >> 26,
                    ((0b111 << 23) & word) >> 23,
                    ((((2 << 19) - 1) << 5) & word) >> 5,
                    0b1111 & word
            };
            assert(splitInstruction[2] == 4);

            //Switch on opi
            switch (splitInstruction[3]){
                // opi is 010, then the instruction is an arithmetic instruction
                case (0b010): {
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
                    return 5;
                }
            }
            // TODO(Replace with macro "increment PC" when in separate file)
            CPU.PC += 4;
        }
        // x101 -> Data processing (register) group
        else if ((op0 & 7) == 5) {
            // Decode to: sf, opc, M, 10, 1, opr, rm, operand, rn, rd
            // TODO(Replace with hashmap if possible)
            u_int32_t splitInstruction[] = {
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
                    return 6;
                }
            }
                // no matching instruction throws error code 6
            else {
                // TODO( Use the variable being switched on instead of recalculating)
                printf("No data processing (register) instruction matching M and opr values: %d, %d\n", splitInstruction[2], splitInstruction[5]);
                return 6;
            }
            // TODO(Replace with macro "increment PC" when in separate file)
            CPU.PC += 4;
        }
        // x1x0 -> Loads and stores group
        else if ((op0 & 5) == 4) {
            // Decode to: bit (1 bit), sf (1 bit), bit (1 bit), op0+ (4 bit), U (1 bit), 'operand' (19 bit), rt (5 bit)
            // TODO(Replace with hashmap if possible)
            u_int32_t splitInstruction[] = {
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
                return 7;
            }
            // TODO(Replace with macro "increment PC" when in separate file)
            CPU.PC += 4;
        }
            // No matching group so throw error code 8
        else {
            printf("decode failed on non-matching op0 value: %d, with word value: %d\n", op0 , word);
            return 8;
        }
    }
    return 0;
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

    int e;
    if ((e = fDECycle()) != 0){
        printf("FDE cycle failed with error code %d\n", e);
        exit(2);
    }

    if (argc > 2) {
        FILE *file = fopen(argv[2], "r");

        if (file == NULL) {
            fprintf( stderr, "Can't read given output file\n" );
            exit(11);
        }
        writeCPU(file, &CPU);
    }
    else {
        writeCPU(stdout, &CPU);
    }

    return EXIT_SUCCESS;
}
