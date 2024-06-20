#ifndef ARMV8_31_DATAPROCESS_H
#define ARMV8_31_DATAPROCESS_H

#include "emulate.h"

extern bool decode_data_immediate(uint32_t word);
extern bool decode_data_register(uint32_t word);

#endif //ARMV8_31_DATAPROCESS_H