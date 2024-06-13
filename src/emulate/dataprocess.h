#ifndef ARMV8_31_DATAPROCESS_H
#define ARMV8_31_DATAPROCESS_H

#include "emulate.h"

bool decodeDataImmediate(uint32_t word);
bool decodeDataRegister(uint32_t word);

#endif //ARMV8_31_DATAPROCESS_H