//
// Created by fn122 on 17/06/24.
//

#ifndef ARMV8_31_BINBUILDER_H
#define ARMV8_31_BINBUILDER_H
#include "assemble.h"

uint32_t data_process(char *inst, char **args, int addr);
uint32_t load_store(char *inst, char **args, int addr);
uint32_t unc_branch(char *inst, char **args, int addr);
uint32_t reg_branch(char *inst, char **args, int addr);
uint32_t con_branch(char *inst, char **args, int addr);
uint32_t dot_int(char *inst, char **args, int addr);

#endif //ARMV8_31_BINBUILDER_H
