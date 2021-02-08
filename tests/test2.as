.entry C
.extern A
.define g1 = 5
.define t = 4 
dec C
prn #g1
.entry B
cmp LIST[1] , #5
bne C
bne B
B: .data t
C: .data 5
END: stop
LIST: .data t,g1,3,2
