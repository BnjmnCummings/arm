movz x0, #0x3f2, lsl #16
movz w1, #64
str w1, [x0]

add x2, x0, #0x28
movz w3, #4
movk x0, #0x1c
movz w5 #50, lsl #16
b on

waitAfterOn:
sub w10, w10, #1
b.ne waitAfterOn
b off

on:
str w3, [x0]
movz w10, w5
b waitAfterOn

waitAfterOff:
sub w10, w10, #1
b.ne waitAfterOff
b on

off:
str w3, [x2]
movz w10, w5
b waitAfterOff
