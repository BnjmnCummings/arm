x0 = 0x3f20000
ldr w1, #64
str w1, [x0]
x2 = 0x3f20001c
ldr w3, #4
x4 = 0x3f200028
b on

waitAfterOn:
sub w10, w10, #1
b.ne waitAfterOn
b off

on:
str w3, [x2]
movk w10, #50, lsl #16
b waitAfterOn

waitAfterOff:
sub w10, w10, #1
b.ne waitAfterOff
b on

off:
str w3, [x4]
movk w10, #50, lsl #16
b waitAfterOff
