#include "dataprocess.h"

extern processor CPU;

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

        // if ((int64_t) op2 >= 0) {
        // CPU.PSTATE.Carry = result < rn;
        // CPU.PSTATE.Overflow = result < rn;
        // } else {
        // CPU.PSTATE.Carry = result > rn;
        // CPU.PSTATE.Overflow = result > rn;
        // }

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

        // if ((int64_t) op2 >= 0) {
        //     CPU.PSTATE.Overflow = result > rn;
        //     CPU.PSTATE.Carry = result >= 0;
        // } else {
        //     CPU.PSTATE.Overflow = result < rn;
        //     CPU.PSTATE.Carry = result <= 0;
        // }
        
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

// arithmeticInstruction is a function taking the split instruction (word) as argument
// It completed the instruction on the CPU
// It returns true on a successful execution, false otherwise
static void arithmeticInstruction(u_int32_t splitWord[]){
    u_int32_t operand[] = {
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

// wideMoveInstruction is a function taking the split instruction (word) as argument
// It completed the instruction on the CPU
// It returns true on a successful execution, false otherwise
static void wideMoveInstruction(u_int32_t splitWord[]){
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

static u_int64_t shift(uint64_t val, uint64_t inc, uint64_t type, bool sf) {
    u_int64_t res;
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
    printf("result: %lx\n", res);
    if (sf) {
        return res;
    } else {
        return (uint32_t) res;
    }
    // return res;
}

static void registerParser(int opc, int rd, u_int64_t rn, u_int64_t op, bool sf, bool negate) {
    if (negate) {
        op = ~op;
    }
    switch (opc) {
        case 0b00: // Bitwise M
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

// multiplyInstruction is a function taking the split instruction (word) as argument
// It completed the instruction on the CPU
// It returns true on a successful execution, false otherwise
static void multiplyInstruction(u_int splitWord[]){
    u_int sf = splitWord[0];

    int64_t rn = read_register(sf, splitWord[8]);
    int64_t rm = read_register(sf, splitWord[6]);

    bool x = splitWord[7] >> 5;
    int64_t ra = read_register(sf, splitWord[7] & 0b11111);
    int64_t mul = rn * rm;
    // printf("x: %d, operand: %u\n", x, splitWord[7]);
    if (x) {
        write_register(sf, splitWord[9], ra - mul);
    } else {
        write_register(sf, splitWord[9], ra + mul);
    }
}

// logicalShiftInstruction is a function taking the split instruction (word) as argument
// It completed the instruction on the CPU
// It returns true on a successful execution, false otherwise
static void logicalShiftInstruction(u_int splitWord[]) {
    u_int opc = splitWord[1];
    u_int shiftMask = (splitWord[5] >> 1) & 0b11;
    u_int negate = splitWord[5] & 0x1;
    u_int sf = splitWord[0];
    u_int64_t firstOp = splitWord[7];
    u_int64_t rn = read_register(sf, splitWord[8]);
    u_int64_t rm = read_register(sf, splitWord[6]);

    if (!sf) {
        firstOp &= ((uint64_t) 2 << 32) - 1;
    }

    u_int64_t secOp = shift(rm, firstOp, shiftMask, sf);

    registerParser(opc, splitWord[9], rn, secOp, sf, negate);
}

// arithmeticShiftInstruction is a function taking the split instruction (word) as argument
// It completed the instruction on the CPU
// It returns true on a successful execution, false otherwise
static void arithmeticShiftInstruction(u_int splitWord[]){
    u_int shiftMask = (splitWord[5] >> 1) & 0b11;
    u_int sf = splitWord[0];
    u_int64_t firstOp = splitWord[7];
    u_int64_t rn = read_register(sf, splitWord[8]);
    u_int64_t rm = read_register(sf, splitWord[6]);
    uint64_t op2 = shift(rm, firstOp, shiftMask, sf);

    processArithmetic(splitWord[1], rn, op2, splitWord[0], splitWord[9]);
}

bool decode_data_immediate(uint32_t word) {
    printf("Entering Data processing (immediate) group with word: %u\n" ,word);
    // Decode to: sf, opc, 100, opi, operand, rd
    u_int32_t splitInstruction[] = {
            word >> 31,
            0b11 & (word >> 29),
            0b111 & (word >> 26),
            0b111 & (word >> 23),
            ((2 << 17) - 1) & (word >> 5),
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
    increment_pc();
    return true;
}

bool decode_data_register(uint32_t word) {
    printf("Entering Data processing (register) group with word: %u\n" ,word);
    // Decode to: sf, opc, M, 10, 1, opr, rm, operand, rn, rd
    u_int32_t splitInstruction[] = {
            word >> 31,
            0b11 & (word >> 29),
            0b1 & (word >> 28),
            0b11 & (word >> 26),
            0b1 & (word >> 25),
            0b1111 & (word >> 21),
            0b11111 & (word >> 16),
            ((1 << 6) - 1) & (word >> 10),
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