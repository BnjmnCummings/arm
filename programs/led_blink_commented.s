// load address of: GPIO pins 0 - 9 mode of input/output into x0
movz x0, #0x3f20, lsl #16
// store 64 in register w1, used to set FSEL2 to 001
movz w1, #64
// store 4 in register w2, used to set GPSET2 and GPCLR2 to 1
movz w2, #4
// store 50 << 16 in register w3, number to be decremented in wait functions
movz w3, #50, lsl #16
// store 1 in register w4, used to decrement above number in wait functions
movz w4, #1

// store 64 into w1, sets FSEL2 to output and all others (0-9) to input
str w1, [x0]
// put address of GPCLR into x1 register
add x1, x0, #0x28
// replace address of GPIO with adress of GPSET in x0 register
movk x0, #0x1c

// create a loop that turns LED on, waits, off, waits
start:

// set GPSET2 to high then continue to waitAfterOn
str w2, [x0]
mov w5, w3

// run a sub operation a bunch of times then continue
waitAfterOn:
    subs w5, w5, w4
    b.ne waitAfterOn

str w2, [x1]
mov w5, w3

// run a sub operation a bunch of times then continue to branch back to start
waitAfterOff:
    subs w5, w5, w4
    b.ne waitAfterOff

// branch back to start
b start
