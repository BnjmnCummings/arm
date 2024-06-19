movz x0, #0x3f20, lsl #16
movz w1, #64
movz w2, #4
movz w3, #50, lsl #16
movz w4, #1

str w1, [x0]
add x1, x0, #0x28
movk x0, #0x1c

start:
str w2, [x0]
mov w5, w3

waitAfterOn:
    subs w5, w5, w4
    b.ne waitAfterOn

str w2, [x1]
mov w5, w3

waitAfterOff:
    subs w5, w5, w4
    b.ne waitAfterOff

b start
