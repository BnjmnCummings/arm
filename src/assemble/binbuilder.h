//
// Created by fn122 on 17/06/24.
//

#ifndef ARMV8_31_BINBUILDER_H
#define ARMV8_31_BINBUILDER_H
#include "assemble.h"
#include "util.h"
#include "tokenizer.h"

extern uint32_t data_process(tokenized_line line, int addr);
extern uint32_t load_store(tokenized_line line, int addr);
extern uint32_t unc_branch(tokenized_line line, int addr);
extern uint32_t reg_branch(tokenized_line line, int addr);
extern uint32_t con_branch(tokenized_line lines, int addr);
extern uint32_t dot_int(tokenized_line line, int addr);

#endif //ARMV8_31_BINBUILDER_H
