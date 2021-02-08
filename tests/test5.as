.entry X
.define c = 1
.define d = 3
MAIN: mov r0, LIST[d]
Z: jmp MAIN
.define a = -33
prn #a
.extern Y
mov LIST[c], LIST[2]
sub r1, r4
L1: inc L3
.entry Z
END: stop
.define b = 4 
bne Z
X: .string "hellow"
LIST: .data 6,b,0,92,-11
.extern L3
