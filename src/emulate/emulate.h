#ifndef ARMV8_31_EMULATE_H
#define ARMV8_31_EMULATE_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <stdint.h>
#include "filewriter.h"
#include "processor.h"
#include "dataprocess.h"
#include "datatransfer.h"


#define BYTESIZE 8

extern u_int64_t get_mask(int start, int end);

#endif //ARMV8_31_EMULATE_H
