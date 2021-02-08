.entry C
.extern A
.define x = 5
.define t = 73 
mov B ,r2
add C ,r3
.entry B
.define z = -4 
red r1
jsr C
bne B
.define y = 55 
B: .data y
C: .data 5
END: stop
