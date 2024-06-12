#include "dataprocess.h"

extern processor CPU;

bool decodeDataImmediate(uint32_t word) {
    printf("Entering Data processing (immediate) group with word: %u\n" ,word);
    // Decode to: sf, opc, 100, opi, operand, rd
    // TODO(Replace with hashmap if possible)
    u_int32_t splitInstruction[] = {
            word >> 31,
            0b11 & (word >> 29),
            0b111 & (word >> 26),
            0b111 & (word >> 23),
            ((0b1 << 19) - 1) & (word >> 5),
            0b11111 & word
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
            return false;
        }
    }
    // TODO(Replace with macro "increment PC" when in separate file)
    incrementPC();
    return true;
}

bool decodeDataRegister(uint32_t word) {
    printf("Entering Data processing (register) group with word: %u" ,word);
    // Decode to: sf, opc, M, 10, 1, opr, rm, operand, rn, rd
    // TODO(Replace with hashmap if possible)
    u_int32_t splitInstruction[] = {
            word >> 31,
            0b11 & (word >> 29),
            0b1 & (word >> 28),
            0b11 & (word >> 26),
            0b1 & (word >> 25),
            0b1111 & (word >> 21),
            0b11111 & (word >> 16),
            ((1 << 7) - 1) & (word >> 10),
            0b11111 & (word >> 5),
            0b11111 & word
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
            return false;
        }
    }
        // no matching instruction throws error code 6
    else {
        // TODO( Use the variable being switched on instead of recalculating)
        printf("No data processing (register) instruction matching M and opr values: %d, %d\n", splitInstruction[2], splitInstruction[5]);
        return false;
    }
    // TODO(Replace with macro "increment PC" when in separate file)
    incrementPC();
    return true;
}

// arithmeticInstruction is a function taking the split instruction (word) as argument
// It completed the instruction on the CPU
// It returns true on a successful execution, false otherwise
bool arithmeticInstruction(u_int32_t splitWord[]){
    u_int32_t operand[] = {
        splitWord[4] << 17,
        ((1 << 12) - 1) & (splitWord[4] >> 5),
        splitWord[4] & 0b11111
    };
    uint64_t op2 = operand[1];
    if (operand[0] == 0) {
        op2 = op2 << 12;
    }
    uint64_t rn = readRegister(splitWord[0], operand[2]);

    uint64_t result;
    uint64_t sigBitShift = (32 + (32 * splitWord[0]) - 1);
    switch (splitWord[1])
    {
    //add
    case (0b00):
        result = rn + op2;
        break;
    //adds 11 - 110000 - 111111
    case (0b01):
        result = rn + op2;
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
        CPU.PSTATE.Negative = result >> sigBitShift;
        CPU.PSTATE.Zero = result == 0;
        CPU.PSTATE.Carry = 0; //TODO - check for carry
        CPU.PSTATE.Overflow = rn < op2; //TODO - check for overflow
        break;
    default:
        result = 0;
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

static u_int64_t shift(int val, int inc, int type, int sf) {
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

static bool registerParser(int opc, int rd, u_int64_t rn, u_int64_t op, bool sf, bool negate) {
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
            CPU.PSTATE.Carry = 0;
            CPU.PSTATE.Overflow = 0;
            break;
        default:
            return false;
    }
    return true;
}

// multiplyInstruction is a function taking the split instruction (word) as argument
// It completed the instruction on the CPU
// It returns true on a successful execution, false otherwise
bool multiplyInstruction(u_int splitWord[]){
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

    return true;
}

// arithmeticShiftInstruction is a function taking the split instruction (word) as argument
// It completed the instruction on the CPU
// It returns true on a successful execution, false otherwise
bool arithmeticShiftInstruction(u_int splitWord[]){
    u_int opc = splitWord[1];
    u_int shiftMask = (splitWord[5] >> 2) & 0x3;
    u_int sf = splitWord[0];
    u_int64_t firstOp = splitWord[7];
    u_int64_t rn = readRegister(sf, splitWord[8]);
    u_int64_t rm = readRegister(sf, splitWord[6]);
    uint64_t op2 = shift(rm, firstOp, shiftMask, sf);

    uint64_t result;
    uint64_t sigBitShift = (32 + (32 * sf) - 1);
    switch (opc)
    {
    //add
    case (0b00):
        result = rn + op2;
        break;
    //adds
    case (0b01):
        result = rn + op2;
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
        CPU.PSTATE.Negative = result >> sigBitShift;
        CPU.PSTATE.Zero = result == 0;
        CPU.PSTATE.Carry = 0; //TODO - check for carry
        CPU.PSTATE.Overflow = rn < op2; //TODO - check for overflow
        break;
    default:
        result = 0;
        break;
    }

    // write to Rd
    writeRegister(sf, splitWord[9], result);

    return true;

    //TODO(set arithmetic PSTATE flags)
    return true;
}