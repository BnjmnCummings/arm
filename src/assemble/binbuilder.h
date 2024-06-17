//
// Created by fn122 on 17/06/24.
//

#ifndef ARMV8_31_BINBUILDER_H
#define ARMV8_31_BINBUILDER_H
#include "assemble.h"

uint32_t imm_data(char *inst, char **args);
uint32_t reg_data(char *inst, char **args);
uint32_t load_store(char *inst, char **args);
uint32_t branch(char *inst, char **args);

#endif //ARMV8_31_BINBUILDER_H
