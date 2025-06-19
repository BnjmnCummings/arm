# ARMv8 AArch64 – Assembler and Emulator

An AArch64 emulator that simulates the execution of an AArch64 binary file working with a subset of the A64 [Reduced Instruction Set Computer (RISC)](https://en.wikipedia.org/wiki/Reduced_instruction_set_computer) architecture for AArch64 (the 64-bit execution mode of the ARMv8-A architecture).

This project also includes an AArch64 assembler that translates an AArch64 assembly source file containing A64 instructions
into a binary file that can subsequently be executed by the emulator. We also created a simple example program in assembly, [led_blink.s](https://github.com/BnjmnCummings/ARMv8-Emulator-and-Assembler/blob/main/programs/led_blink.s) which can run on a [Raspberry Pi 3B](https://www.raspberrypi.com/) after being translated by our assembler!

Finally, our assembler allows for commenting in assembly files, which isn't usually supported, to grant a slightly more comfortable coding experience in assembly. These use the ```//``` line by line commenting syntax. You can see an example of commented assembly in [led_blink-commented.s](https://github.com/BnjmnCummings/ARMv8-Emulator-and-Assembler/blob/main/programs/led_blink_commented.s).

If you like our ARMv8 Emulator and Assembler, why not try... [PintOS](https://github.com/BnjmnCummings/PintOS)?
