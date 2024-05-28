#include <stdlib.h>

int main(int argc, char **argv) {
  return EXIT_SUCCESS;
}
/* TODO( complete all the below sections for the emulator)
 binary file loader - create a function that takes in a (binary)
 file location and reads it into memory

 write emulator loop:
    simulator of arm execution cycle - want to simulate the fetch and decode
    of the FDE cycle so something like a:
        struct for Z/P/S...
        struct for the CPU initialised once as a global variable - the CPU
        function made to read the instruction indicated by the CPU state
        functions to decode different forms of instructions

    simulate execution of instructions - quite probably a set of functions
    and a massive switch statement that looks at the current instruction and
    just does the necessary operations on the CPU (global instance)

    terminate upon instruction with encoding 0x8a000000 - probably just an added
    condition on the above part

    Upon termination, output the registers values, the PSTATE condition flags, and any non-zero memory to a .out file-
    simply a long print statement -
        function to output the correct formatting of CPU states
        may need some helper functions to clear things up
 */