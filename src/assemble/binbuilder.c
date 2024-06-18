#include "binbuilder.h"

static uint32_t load_literal(char *rt, char *literal, int addr) {
    uint32_t result = 0;
    result += (rt[0] == 'x') << 30;  // adds sf bit
    result += reg_to_bin(rt); // adds rt bits
    result += (calc_num(true, 19, literal) - addr) << 5; // adds simm19
    result += 0b11 << 27;  // adds extra bits

    return result;
}

static uint32_t multiply(char *inst, char *rd, char *rn, char *rm, char *ra) {
    uint32_t result = 0;
    result += (rd[0] == 'x') << 31; // adds sf bit
    result += reg_to_bin(rd);  // adds rd bits
    result += reg_to_bin(rn) << 5;  // adds rn bits
    result += reg_to_bin(rm) << 16;  // adds rm bits
    result += reg_to_bin(ra) << 10;  // adds ra bits
    result += strcmp(inst, "madd") << 15; // adds x bit
    result += 0b11011 << 24;  // adds extra bits
    return result;
}

static uint32_t immediate_arith(uint32_t isSub, bool flags, char *rd, char *rn, bool shift, char *num) {
    uint32_t result = 0;
    result += reg_to_bin(rd);  // adds rd bits
    result += reg_to_bin(rn) << 5;  // adds rn bits
    result += calc_num(false, 12, num) << 10; // adds imm12 bits
    result += shift << 22;  // adds sh bit
    result += 0b100010 << 23;  // adds opi and extra bits
    result += ((isSub << 1) + flags) << 29;  // adds opc bits
    result += (rd[0] == 'x') << 31;  // adds sf bit
    return result;
}

static uint32_t reg_arith(char *inst, char *rd, char *rn, char *rm, char *shift, bool flags) {
    uint32_t opr = 0b1000;
    uint32_t operand = 0;

    if (strncmp(shift, "lsr", sizeof(char)) == 0) {
        operand = calc_num(false, 6, shift + 4);
        if (strncmp(shift, "lsr", 3 * sizeof(char)) == 0) {
            opr += 0b100;
        } else if (strncmp(shift, "asr", 3 * sizeof(char)) == 0) {
            opr += 0b10;
        }
    }

        uint32_t result = 0;
    result += reg_to_bin(rd);  // adds rd bits
    result += reg_to_bin(rn) << 5;  // adds rn bits
    result += reg_to_bin(rm) << 16;
    result += operand << 10;
    result += opr << 21;
    result += 0b101 << 25;  // adds extra bits
    result += (((strcmp(inst, "sub") == 0) << 1) + flags) << 29;  // adds opc bits
    result += (rd[0] == 'x') << 31;  // adds sf bit
    return result;
}

static uint32_t reg_logic(char *inst, char *rd, char *rn, char *rm, char *shift, bool flags) {
    uint32_t operand = 0;
    uint32_t opr = 0;
    uint32_t opc = 0;

    if (strcmp(inst, "bic") == 0) {
        opc = 0b00;
        opr += 1   ;
    } else if (strcmp(inst, "orr") == 0) {
        opc = 0b01;
    } else if (strcmp(inst, "orn") == 0) {
        opc = 0b01;
        opr += 1;
    } else if (strcmp(inst, "eor") == 0) {
        opc = 0b10;
    } else if (strcmp(inst, "eon") == 0) {
        opc = 0b10;
        opr += 1;
    }
    opc += 0b11 * flags;

    if (strncmp(shift, "lsr", sizeof(char)) == 0) {
        operand = calc_num(false, 6, shift + 4);
        if (strncmp(shift, "lsr", 3 * sizeof(char)) == 0) {
            opr += 0b100;
        } else if (strncmp(shift, "asr", 3 * sizeof(char)) == 0) {
            opr += 0b10;
        } else if (strncmp(shift, "lsr", 3 * sizeof(char)) == 0) {
            opr += 0b110;
        }
    }

    uint32_t result = 0;
    result += reg_to_bin(rd);  // adds rd bits
    result += reg_to_bin(rn) << 5;  // adds rn bits
    result += reg_to_bin(rm) << 16;  // adds rm bits
    result += operand << 10;
    result += opr << 21;
    result += 0b101 << 25;
    result += opc << 29;
    result += (rd[0] == 'x') << 31;
    return result;
}

static uint32_t wide_move(char *inst, char *rd, char *shift, char *numstr) {
    uint32_t opc = 0;
    uint32_t hw = 0;

    if (strcmp(shift, "") != 0) {
        hw = calc_num(false, 64, shift + 4) / 16;
    }
    if (strcmp(inst, "movk") == 0) {
        opc = 0;
    } else if (strcmp(inst, "movn") == 0) {
        opc = 0b10;
    } else if (strcmp(inst, "movz") == 0) {
        opc = 0b11;
    }

    uint32_t result = 0;
    result += reg_to_bin(rd);
    result += calc_num(false, 16, numstr) << 5;
    result += hw << 21;
    result += 0b100101 << 23;
    result += opc << 29;
    result += (rd[0] == 'x') << 31;
    return result;
}

uint32_t data_process(char *inst, char **args, int addr) {
    bool flags = false;
    if (inst[strlen(inst) - 1] == 's') {
        inst[strlen(inst) - 1] = '\0';
        flags = true;
    }

    int num_args = sizeof(args)/sizeof(args[0]);
    char shift[10] = "";
    if (args[num_args - 1][3] == ' ') {
        strcpy(shift, args[num_args - 1]);
        num_args--;
    }

    switch (num_args)
    {
    case 2:
        if (strcmp(inst, "cmp") == 0) {
            return immediate_arith(1, true, "rzr", args[0], (strcmp(shift, "") != 0), args[1]);
        } else if (strcmp(inst, "cmn") == 0) {
            return immediate_arith(0, true, "rzr", args[0], (strcmp(shift, "") != 0), args[1]);
        } else if (strcmp(inst, "neg") == 0) {
            return immediate_arith(1, flags, args[0], "rzr", (strcmp(shift, "") != 0), args[1]);
        } else if (strcmp(inst, "tst") == 0) {
            return reg_logic("and", "rzr", args[0], args[1], shift, true);
        } else if (strcmp(inst, "mvn") == 0) {
            return reg_logic("orn", args[0], "rzr", args[1], shift, false);
        } else if (strcmp(inst, "mov") == 0) {
            return reg_logic("orr", args[0], "rzr", args[1], "", false);
        } else {
            return wide_move(inst, args[0], shift, args[1]);
        }
        case 3:
        // two operand
        if (args[2][0] == '#') {
            // add/sub imm
            bool isSub = strcmp(inst, "sub") == 0;
            bool isShift = (shift[3] != ' ') && (strcmp(shift, "") != 0);
            return immediate_arith(isSub, flags, args[0], args[1], isShift, args[2]);
        } else if (strcmp(inst, "mul") == 0) {
            // mul
            return multiply("madd", args[0], args[1], args[2], "rzr");
        } else if (strcmp(inst, "mneg") == 0) {
            // mneg
            return multiply("msub", args[0], args[1], args[2], "rzr");
        } else if (strcmp(inst, "add") == 0 || strcmp(inst, "sub") == 0) {
            // register arithmetic
            return reg_arith(inst, args[0], args[1], args[2], shift, flags);
        } else {
            // register logical
            return reg_logic(inst, args[0], args[1], args[2], shift, flags);
        }
    case 4:
        // multiply
        return multiply(inst, args[0], args[1], args[2], args[3]);
    default:
        return 0;
    }
}

uint32_t load_store(char *inst, char **args, int addr) {
    if (args[1][0] != '[') {
        return load_literal(args[0], args[1], addr);
    }

    bool sf = args[0][0] == 'x';
    uint32_t result = 0; // to return
    result += sf << 30;  // adds sf bit
    result += reg_to_bin(args[0]); // adds rt bits
    result += (strcmp(inst, "ldr") == 0) << 22; // adds L bit
    result += reg_to_bin(args[1] + 1) << 5; // adds xn bits
    result += 0b10111 << 27;  // adds extra bits
    uint32_t offset = 0;
    bool u;

    if (args[2][0] == '\0') {
        // zero unsigned offset
        u = true;
    } else if (args[2][strlen(args[2]) - 1] == '!') {
        // pre-indexed
        u = false;
        offset += 0b11;
        offset += calc_num(true, 9, args[2]);
    } else if (args[2][0] != '#') {
        // register offset
        u = false;
        offset += 1 << 11;
        offset += 0b011010;
        offset += reg_to_bin(args[0]) << 6;
    } else if (args[2][strlen(args[2]) - 1] == ']') {
        // unsigned offset
        u = true;
        offset = calc_num(false, 12, args[2]) / (4 + (4 * sf));
    } else {
        // post-indexed
        u = false;
        offset += 0b01;
        offset += calc_num(true, 9, args[2]);
    }

    result += offset << 10;
    result += u << 24;

    return result;
}

uint32_t unc_branch(char *inst, char **args, int addr) {
    uint32_t result = 0;
    result += 0b101 << 26;  // adds extra bits
    result += calc_num(true, 26, args[0]) - addr; // adds simm26
    return result;
}

uint32_t reg_branch(char *inst, char **args, int addr) {
    uint32_t result = 0;
    result += 0b1101011000011111 << 16; // adds extra bits
    result += reg_to_bin(args[0]) << 5;
    return result;
}

uint32_t con_branch(char *inst, char **args, int addr) {
    char condition[2];
    strcpy(condition, inst + 2);
    
    uint32_t result = 0;
    result += 0b10101 << 26;  // adds extra bits
    result += (calc_num(true, 19, args[0]) - addr) << 5;  // adds simm19
    uint32_t cond;

    if (strcmp(condition, "eq") == 0) {
        cond = 0b0000;
    } else if (strcmp(condition, "ne") == 0) {
        cond = 0b0001;
    } else if (strcmp(condition, "ge") == 0) {
        cond = 0b1010;
    } else if (strcmp(condition, "lt") == 0) {
        cond = 0b1011;
    } else if (strcmp(condition, "gt") == 0) {
        cond = 0b1100;
    } else if (strcmp(condition, "le") == 0) {
        cond = 0b1101;
    } else {
        cond = 0b1110;
    }
    result += cond;  // adds cond
    return result;
}

uint32_t dot_int(char *inst, char **args, int addr) {
    return calc_num(true, 32, args[0]);
}