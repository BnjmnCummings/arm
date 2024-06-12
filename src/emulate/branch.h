#ifndef ARMV8_31_BRANCH_H
#define ARMV8_31_BRANCH_H

#include "emulate.h"

bool unconditionalBranch(const u_int32_t splitWord[]);
bool registerBranch(u_int32_t splitWord[]);
bool conditionalBranch(const u_int32_t splitWord[]);
bool decodeBranch(uint32_t word);

#endif //ARMV8_31_BRANCH_H
