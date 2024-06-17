#include "binbuilder.h"

uint32_t load_literal(char *rt, char *literal) {
    return 0;
}

uint32_t imm_data(char *inst, char **args) {
    return 0;
}

uint32_t reg_data(char *inst, char **args) {
    return 0;
}

uint32_t load_store(char *inst, char **args, int addr) {
    if (args[1][0] != '[') {
        return load_literal(args[0], args[1]);
    }

    bool sf = args[0][0] == 'X';
    uint32_t result = 0; // to return
    result += sf << 30;  // adds sf bit
    result += reg_to_bin(args[0]); // adds rt bits
    result += (strcmp(inst, "ldr") == 0) << 22; // adds L bit
    result += calc_offset(addr, strip_address(args[1])) << 5; // adds xn bits
    uint32_t offset = 0;
    bool u;

    if (args[2][0] == '\0') {
        // zero unsigned offset
        u = true;
    } else if (args[2][strlen(args[2]) - 1] == '!') {
        // pre-indexed
        u = false;
        offset += 0b11;
        offset += strip_num(args[2]);
    } else if (args[2][0] != '#') {
        // register offset
        u = false;
        offset += 1 << 11;
        offset += 0b011010;
        offset += reg_to_bin(args[0]) << 6;
    } else if (args[2][strlen(args[2]) - 1] == ']') {
        // unsigned offset
        u = true;
        offset = strip_num(args[2]) / (4 + (4 * sf));
    } else {
        // post-indexed
        u = false;
        offset += 0b01;
        offset += strip_num(args[2]);
    }

    result += offset << 10;
    result += u << 24;

    return result;
}

uint32_t branch(char *inst, char **args) {
    return 0;
}