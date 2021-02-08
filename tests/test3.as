.extern W
STR: .string "hello world"
L1: .data 1,2,3,4,5,6,7

.define macro1 = 10

.entry L1


A: mov r3, L1[3]
LOOP: jmp W
inc L1[4]
mov STR[5], STR[2]
sub r1, r4
not L1[macro1]
bne W
.entry LOOP
bne LOOP
.define len = 4 

