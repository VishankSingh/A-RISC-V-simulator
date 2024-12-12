.data
.dword 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160
.text

lui x3, 0x10
ld x4, 0(x3)
ld x5, 8(x3)
ld x6, 16(x3)
ld x7, 24(x3)
ld x8, 32(x3)
ld x9, 40(x3)
ld x10, 48(x3)
ld x11, 56(x3)
ld x12, 64(x3)
ld x13, 72(x3)
ld x14, 80(x3)
ld x15, 88(x3)
ld x16, 96(x3)
ld x17, 104(x3)
ld x18, 112(x3)
ld x20, 120(x3)

ld x21, 0(x3)
ld x22, 8(x3)
ld x23, 16(x3)
ld x24, 24(x3)
ld x25, 32(x3)

sd x9, 8(x3)
sd x10, 256(x3)
sd x10, 264(x3)



