beq x1, x2, L1
jal x0, L2
L1: xor a5, a3, a7
lui x9, 0x10000
L2: add t1, x8, s10

# 00208463
# 00c0006f 
# 0116c7b3    
# 100004b7     
# 01a40333 