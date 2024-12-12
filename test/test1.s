.data
.dword 1000000000000000900, 0x12, 32134
.word 12
.byte 1

.text
addi x5,x0,0x20
addi x5,x5, 0x40
lui x3, 0x10
lw x10, 0(x3)
andi x10, x10, 0b1111

slli x10, x10, 8
slli x10, x10, 8



# 007201b3
# 00720863
# 00c0006f
# 00533623
# 100004b7
# 00c50493
