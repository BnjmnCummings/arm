#ifndef ARMV8_31_DATAPROCESS_H
#define ARMV8_31_DATAPROCESS_H

#include "emulate.h"

bool decodeDataImmediate(uint32_t word);
bool decodeDataRegister(uint32_t word);

bool arithmeticInstruction(u_int32_t splitWord[]);
bool wideMoveInstruction(u_int32_t splitWord[]);

bool multiplyInstruction(u_int splitWord[]);
bool logicalShiftInstruction(u_int splitWord[]);
bool arithmeticShiftInstruction(u_int splitWord[]);

#endif //ARMV8_31_DATAPROCESS_H