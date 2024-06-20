#include "binbuilder.h"
#include "util.h"

//builds special case loading instruction which loads a literal into register at 'rt'
static uint32_t load_literal(char *rt, char *literal, int addr) {
    uint32_t result = 0;

    result += check_sf(rt, "") << 30;  // adds sf bit
    result += reg_to_bin(rt); // adds rt bits
    result += (calc_offset(true, 19, literal + 1, addr)) << 5; // adds simm19
    result += 0b11 << 27;  // adds extra bits

    return result;
}

//builds instructions that handle multiplication of positive and negative numbers
// saving result to register at 'rd'
static uint32_t multiply(char *inst, char *rd, char *rn, char *rm, char *ra) {
    uint32_t result = 0;

    result += check_sf(rd, rn) << 31; // adds sf bit
    result += reg_to_bin(rd);  // adds rd bits
    result += reg_to_bin(rn) << 5;  // adds rn bits
    result += reg_to_bin(rm) << 16;  // adds rm bits
    result += reg_to_bin(ra) << 10;  // adds ra bits
    result += (strcmp(inst, "msub") == 0) << 15; // adds x bit
    result += 0b11011 << 24;  // adds extra bits

    return result;
}

//handles special case instructions for arithmetic using immediate values
static uint32_t immediate_arith(uint32_t isSub, bool flags, char *rd, char *rn, char *shift, char *num) {
    uint32_t result = 0;
    bool isShift = strlen(shift) == strlen("xxx #12");

    result += reg_to_bin(rd);  // adds rd bits
    result += reg_to_bin(rn) << 5;  // adds rn bits
    result += calc_num(false, 12, num + 1) << 10; // adds imm12 bits
    result += isShift << 22;  // adds sh bit
    result += 0b100010 << 23;  // adds opi and extra bits
    result += ((isSub << 1) + flags) << 29;  // adds opc bits
    result += check_sf(rd, rn) << 31;  // adds sf bit

    return result;
}

//handles arithmetic between values stored in registers
static uint32_t reg_arith(char *inst, char *rd, char *rn, char *rm, char *shift, bool flags) {
    uint32_t opr = 0b1000;
    uint32_t operand = 0;

    if (strncmp(shift, "", sizeof(char)) != 0) {
        operand = calc_num(false, 6, shift + 5);
        if (strncmp(shift, "lsr", 3 * sizeof(char)) == 0) {
            opr += 0b10;
        } else if (strncmp(shift, "asr", 3 * sizeof(char)) == 0) {
            opr += 0b100;
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
    result += check_sf(rd, rn) << 31;  // adds sf bit

    return result;
}

//handles various instruction type cases for registry logic
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

    if (strncmp(shift, "", sizeof(char)) != 0) {
        operand = calc_num(false, 6, shift + 5);
        if (strncmp(shift, "lsr", 3 * sizeof(char)) == 0) {
            opr += 0b10;
        } else if (strncmp(shift, "asr", 3 * sizeof(char)) == 0) {
            opr += 0b100;
        } else if (strncmp(shift, "ror", 3 * sizeof(char)) == 0) {
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
    result += check_sf(rd, rn) << 31;

    return result;
}

//handles instructions for storing literals into register at 'rd'
static uint32_t wide_move(char *inst, char *rd, char *shift, char *num_str) {
    uint32_t opc = 0;
    uint32_t hw = 0;

    if (strcmp(shift, "") != 0) {
        hw = calc_num(false, 64, shift + 5) / 16;
    }
    if (strcmp(inst, "movn") == 0) {
        opc = 0;
    } else if (strcmp(inst, "movz") == 0) {
        opc = 0b10;
    } else if (strcmp(inst, "movk") == 0) {
        opc = 0b11;
    }

    uint32_t result = 0;

    result += reg_to_bin(rd);
    result += calc_num(false, 16, num_str + 1) << 5;
    result += hw << 21;
    result += 0b100101 << 23;
    result += opc << 29;
    result += check_sf(rd, "") << 31;

    return result;
}

//handles all the data processing instructions
uint32_t data_process(tokenized_line line, int addr) {
    bool flags = false;

    if (line.inst[strlen(line.inst) - 1] == 's') {
        line.inst[strlen(line.inst) - 1] = '\0';
        flags = true;
    }

    int num_args = line.ntokens;
    char shift[10] = "";
    if (line.args[num_args - 2][1] == 's' || line.args[num_args - 2][1] == 'o') {
        sprintf(shift, "%s %s", line.args[num_args - 2], line.args[num_args - 1]);
        num_args -= 2;
    }

    switch (num_args)
    {
    case 2:
        if (strcmp(line.inst, "cmp") == 0) {
            //checks if we are dealing with literals or loaded values
            if (line.args[1][0] == '#') {
                return immediate_arith(1, true, "rzr", line.args[0], shift, line.args[1]);
            } else {
                return reg_arith("sub", "rzr", line.args[0], line.args[1], shift, true);
            }
        } else if (strcmp(line.inst, "cmn") == 0) {
            //checks if we are dealing with literals or loaded values
            if (line.args[1][0] == '#') {
                return immediate_arith(0, true, "rzr", line.args[0], shift, line.args[1]);
            } else {
                return reg_arith("add", "rzr", line.args[0], line.args[1], shift, true);
            }
        } else if (strcmp(line.inst, "neg") == 0) {
            //negates an immediate
            return immediate_arith(1, flags, line.args[0], "rzr", shift, line.args[1]);
        } else if (strcmp(line.inst, "tst") == 0) {
            //handles test bits
            return reg_logic("and", "rzr", line.args[0], line.args[1], shift, true);
        } else if (strcmp(line.inst, "mvn") == 0) {
            //moves and negates
            return reg_logic("orn", line.args[0], "rzr", line.args[1], shift, false);
        } else if (strcmp(line.inst, "mov") == 0) {
            //moves register
            return reg_logic("orr", line.args[0], "rzr", line.args[1], "", false);
        } else {
            //move 'n' register
            return wide_move(line.inst, line.args[0], shift, line.args[1]);
        }
        case 3:
        // two operand
        if (line.args[2][0] == '#') {
            // add/sub imm
            bool isSub = strcmp(line.inst, "sub") == 0;
            return immediate_arith(isSub, flags, line.args[0], line.args[1], shift, line.args[2]);
        } else if (strcmp(line.inst, "mul") == 0) {
            // mul
            return multiply("madd", line.args[0], line.args[1], line.args[2], "rzr");
        } else if (strcmp(line.inst, "mneg") == 0) {
            // mneg
            return multiply("msub", line.args[0], line.args[1], line.args[2], "rzr");
        } else if (strcmp(line.inst, "add") == 0 || strcmp(line.inst, "sub") == 0) {
            // register arithmetic
            return reg_arith(line.inst, line.args[0], line.args[1], line.args[2], shift, flags);
        } else {
            // register logical
            return reg_logic(line.inst, line.args[0], line.args[1], line.args[2], shift, flags);
        }
    case 4:
        // multiply
        return multiply(line.inst, line.args[0], line.args[1], line.args[2], line.args[3]);
    default:
        return 0;
    }
}

//handles all the loading and storing instructions.
uint32_t load_store(tokenized_line line, int addr) {
    //handles literal case
    if (line.args[1][0] != '[') {
        return load_literal(line.args[0], line.args[1], addr);
    }

    bool sf = line.args[0][0] == 'x';
    uint32_t result = 0; // to return

    result += sf << 30;  // adds sf bit
    result += reg_to_bin(line.args[0]); // adds rt bits
    result += (strcmp(line.inst, "ldr") == 0) << 22; // adds L bit
    result += 0b10111 << 27;  // adds extra bits

    //calculating offset
    bool u;
    uint32_t offset = 0;

    if (line.args[2][0] == '\0') {
        // zero unsigned offset
        u = true;
    } else if (line.args[2][strlen(line.args[2]) - 1] == '!') {
        // pre-indexed
        u = false;
        offset += 0b11;
        offset += calc_num(true, 9, line.args[2] + 1) << 2;
    } else if (line.args[2][0] != '#') {
        // register offset
        u = false;
        offset += 1 << 11;
        offset += 0b011010;
        offset += reg_to_bin(line.args[2]) << 6;
    } else if (line.args[2][strlen(line.args[2]) - 1] == ']') {
        // unsigned offset
        u = true;
        offset = calc_num(false, 12, line.args[2] + 1) / (4 + (4 * sf));
    } else {
        // post-indexed
        u = false;
        offset += 0b01;
        offset += calc_num(true, 9, line.args[2] + 1) << 2;
    }

    result += offset << 10;
    result += u << 24;
    result += reg_to_bin(line.args[1] + 1) << 5; // adds xn bits

    return result;
}

//handles unconditional branch instructions
uint32_t unc_branch(tokenized_line line, int addr) {
    uint32_t result = 0;

    result += 0b101 << 26;  // adds extra bits
    result += calc_offset(true, 26, line.args[0] + 1, addr); // adds simm26

    return result;
}

//handles regular branch instructions
uint32_t reg_branch(tokenized_line line, int addr) {
    uint32_t result = 0;

    result += 0b1101011000011111 << 16; // adds extra bits
    result += reg_to_bin(line.args[0]) << 5;

    return result;
}

//handles conditional branch instructions
uint32_t con_branch(tokenized_line line, int addr) {
    char condition[2];
    strcpy(condition, line.inst + 2);
    
    uint32_t result = 0;
    result += 0b10101 << 26;  // adds extra bits
    result += calc_offset(true, 19, line.args[0] + 1, addr) << 5; // addrs simm19
    uint32_t cond;

    //match conditions
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
    //add condition
    result += cond;

    return result;
}

//returns binary representation of number defined by '.int' directive
uint32_t dot_int(tokenized_line line, int addr) {
    return calc_num(true, 32, line.args[0]);
}