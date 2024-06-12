#ifndef ARMV8_31_DATATRANSFER_H
#define ARMV8_31_DATATRANSFER_H

#include "emulate.h"

bool decodeDataTransfer(uint32_t word);

bool loadLiteralInstruction(u_int32_t splitWord[]);
bool singleDataTransferInstruction(const u_int32_t splitWord[]);

#endif //ARMV8_31_DATATRANSFER_H