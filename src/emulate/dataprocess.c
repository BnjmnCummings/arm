#include "dataprocess.h"

extern processor CPU;

// adds or subracts immediate value op2 from value in rn, setting flags if needed.
static void processArithmetic(uint64_t opc, uint64_t rn, uint64_t op2, bool sf, uint32_t rd) {
    uint64_t result;
    int64_t sresult;
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
        sresult = ((int64_t) rn) + ((int64_t) op2);
        CPU.PSTATE.Negative = (result >> sigBitShift) & 0b1;
        CPU.PSTATE.Zero = result == 0;

        /*  in case the below functions should not be used,
        this code also works for setting overflow and carry flags, but fails 2 test cases for the carry.

        if ((int64_t) op2 >= 0) {
        CPU.PSTATE.Carry = result < rn;
        CPU.PSTATE.Overflow = result < rn;
        } else {
        CPU.PSTATE.Carry = result > rn;
        CPU.PSTATE.Overflow = result > rn;
        }
        */

        if (sf) {
            CPU.PSTATE.Carry =  __builtin_uaddl_overflow(rn, op2, &result);
            CPU.PSTATE.Overflow =  __builtin_saddl_overflow((int64_t) rn, (int64_t) op2, &sresult);
        } else {
            uint32_t r = (uint32_t) rn + (uint32_t) op2;
            int32_t sr = (int32_t) rn + (int32_t) op2;
            CPU.PSTATE.Carry =  __builtin_uadd_overflow((uint32_t) rn, (uint32_t) op2, &r);
            CPU.PSTATE.Overflow =  __builtin_sadd_overflow((int32_t) rn, (int32_t) op2, &sr);
        }
        break;
    //sub
    case (0b10):
        result = rn - op2;
        break;
    //subs
    case (0b11):
        result = rn - op2;
        sresult = (int64_t) rn + (int64_t) op2;
        CPU.PSTATE.Negative = (result >> sigBitShift) & 0b1;
        CPU.PSTATE.Zero = result == 0;

        /* as above.

        if ((int64_t) op2 >= 0) {
            CPU.PSTATE.Overflow = result > rn;
            CPU.PSTATE.Carry = result >= 0;
        } else {
            CPU.PSTATE.Overflow = result < rn;
            CPU.PSTATE.Carry = result <= 0;
        }
        */
        
        if (sf) {
            CPU.PSTATE.Carry = ! __builtin_usubl_overflow(rn, op2, &result);
            CPU.PSTATE.Overflow =  __builtin_ssubl_overflow((int64_t) rn, (int64_t) op2, &sresult);
        } else {
            uint32_t r = (uint32_t) rn - (uint32_t) op2;
            int32_t sr = (int32_t) rn - (int32_t) op2;
            CPU.PSTATE.Carry = !__builtin_usub_overflow((uint32_t) rn, (uint32_t) op2, &r);
            CPU.PSTATE.Overflow =  __builtin_ssub_overflow((int32_t) rn, (int32_t) op2, &sr);
        }
        break;
    default:
        result = 0;
        break;
    }

    // write to Rd
    write_register(sf, rd, result);
}

// executes data processing (immediate) instruction by calling processArithmetic on rn and (possible shifted) op2
static void arithmeticInstruction(uint32_t splitWord[]){
    uint32_t operand[] = {
        splitWord[4] >> 17,
        ((1 << 12) - 1) & (splitWord[4] >> 5),
        splitWord[4] & 0b11111
    };
    uint64_t op2 = operand[1];
    if (operand[0]) {
        op2 = op2 << 12;
    }
    int64_t rn = read_register(splitWord[0], operand[2]);
    processArithmetic(splitWord[1], rn, op2, splitWord[0], splitWord[5]);
}

// executes wide move instruction by copying immediate value into register, following given instruction rules
static void wideMoveInstruction(uint32_t splitWord[]){
    uint64_t operand[] = {
        splitWord[4] >> 16,
        splitWord[4] & ((2 << 15) - 1)
    };
    int sh = operand[0] * 16;
    uint64_t op = operand[1] << sh;
    uint64_t result;
    switch (splitWord[1])
    {
    //movk
    case (0b00):
        result = ~op;
        break;
    //movz
    case (0b10):
        result = op;
        break;
    //movk
    case (0b11):{
        uint64_t rd = read_register(splitWord[0], splitWord[5]);
        result = (rd & get_mask(sh + 16, 63)) + op + (rd & get_mask(0, sh - 1));
        break;
    }
    default:
        result = 0;
        break;
    }
    write_register(splitWord[0], splitWord[5], result);
}

// shifts val using given shift type by inc bits
static uint64_t shift(uint64_t val, uint64_t inc, uint64_t type, bool sf) {
    uint64_t res;
    uint64_t maskVal = (32 + (32 * sf) - 1);
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
        if ((val >> maskVal) & 0b1) {
            res += get_mask(maskVal - (inc - 1), maskVal);
        }
    }
    // ror shift
    else {
        res = val >> inc;
        res += (val & get_mask(0, inc - 1)) << (maskVal - (inc - 1));
    }
    if (sf) {
        return res;
    } else {
        return (uint32_t) res;
    }
}

// executes given logical instruction on values rn and op, setting flags if needed
static void registerParser(int opc, int rd, uint64_t rn, uint64_t op, bool sf, bool negate) {
    if (negate) {
        op = ~op;
    }
    switch (opc) {
        case 0b00:
            write_register(sf, rd, rn & op);
            break;
        case 0b01:
            write_register(sf, rd, rn | op);
            break;
        case 0b10:
            write_register(sf, rd, rn ^ op);
            break;
        case 0b11:
            write_register(sf, rd, rn & op);
            CPU.PSTATE.Negative = (rn & op) >> (32 + (32 * sf) - 1);
            CPU.PSTATE.Zero = (rn & op) == 0;
            CPU.PSTATE.Carry = 0;
            CPU.PSTATE.Overflow = 0;
            break;
        default:
            break;
    }
}

// executes multiply instruction by multiplying values in rn & rm, and add/sub from ra
static void multiplyInstruction(uint32_t splitWord[]){
    bool sf = splitWord[0];

    int64_t rn = read_register(sf, splitWord[8]);
    int64_t rm = read_register(sf, splitWord[6]);

    bool x = splitWord[7] >> 5;
    int64_t ra = read_register(sf, splitWord[7] & 0b11111);
    int64_t mul = rn * rm;
    if (x) {
        write_register(sf, splitWord[9], ra - mul);
    } else {
        write_register(sf, splitWord[9], ra + mul);
    }
}

// executes logical shift instruction by shifting given values and passing into registerParser
static void logicalShiftInstruction(uint32_t splitWord[]) {
    uint64_t opc = splitWord[1];
    uint64_t shiftMask = (splitWord[5] >> 1) & 0b11;
    uint64_t negate = splitWord[5] & 0x1;
    bool sf = splitWord[0];
    uint64_t firstOp = splitWord[7];
    uint64_t rn = read_register(sf, splitWord[8]);
    uint64_t rm = read_register(sf, splitWord[6]);

    if (!sf) {
        firstOp &= ((uint64_t) 2 << 32) - 1;
    }

    uint64_t secOp = shift(rm, firstOp, shiftMask, sf);

    registerParser(opc, splitWord[9], rn, secOp, sf, negate);
}

// executes arithmetc shift instruction by reading given values and passing into processArithmetic
static void arithmeticShiftInstruction(uint32_t splitWord[]){
    uint64_t shiftMask = (splitWord[5] >> 1) & 0b11;
    bool sf = splitWord[0];
    uint64_t firstOp = splitWord[7];
    uint64_t rn = read_register(sf, splitWord[8]);
    uint64_t rm = read_register(sf, splitWord[6]);
    uint64_t op2 = shift(rm, firstOp, shiftMask, sf);

    processArithmetic(splitWord[1], rn, op2, splitWord[0], splitWord[9]);
}

// decodes data processing (immediate) instruction and passes result into corresponding execution function
bool decode_data_immediate(uint32_t word) {
    uint32_t splitInstruction[] = {
            word >> 31,                     // sf
            0b11 & (word >> 29),            // opc
            0b111 & (word >> 26),           // 100
            0b111 & (word >> 23),           // opi
            ((2 << 17) - 1) & (word >> 5),  // operand
            0b11111 & word                  // rd
    };
    assert(splitInstruction[2] == 4);

    //Switch on opi
    switch (splitInstruction[3]){
        // arithmetic instruction
        case (0b010): {
            arithmeticInstruction(splitInstruction);
            break;
        }
            // wide move
        case (0b101): {
            wideMoveInstruction(splitInstruction);
            break;
        }
            // no matching instruction throws error code 5
        default: {
            printf("No data processing (immediate) instruction matching opi value: %d\n", splitInstruction[3]);
            return false;
        }
    }
    increment_pc();
    return true;
}

// decodes data processing (register) instruction and passes result into corresponding execution function
bool decode_data_register(uint32_t word) {
    uint32_t splitInstruction[] = {
            word >> 31,                     // sf
            0b11 & (word >> 29),            // opc
            0b1 & (word >> 28),             // M
            0b11 & (word >> 26),            // 10
            0b1 & (word >> 25),             // 1
            0b1111 & (word >> 21),          // opr
            0b11111 & (word >> 16),         // rm
            ((1 << 6) - 1) & (word >> 10),  // operand
            0b11111 & (word >> 5),          // rn
            0b11111 & word                  // rd
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
            printf("No data processing (register) instruction matching M and opr values: %d, %d\n", splitInstruction[2], splitInstruction[5]);
            return false;
        }
    }
        // no matching instruction throws error code 6
    else {
        printf("No data processing (register) instruction matching M and opr values: %d, %d\n", splitInstruction[2], splitInstruction[5]);
        return false;
    }
    increment_pc();
    return true;
}