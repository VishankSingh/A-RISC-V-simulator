.data
.dword 0x1234567890abcdef

.text
lui x3, 0x10
lbu x10, 0(x3)
