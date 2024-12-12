#starts at 0x10000000
.data       

#fp_exponent_bits - number of exponent bits. It should be 11 for double precision number
#fp_mantissa_bits - number of mantissa bits. It should be 52 for double precision number

.dword 11, 52

#starts at 0x10000010
#count_of_floating_pairs - tells the number of floating point pairs to be operated upon.

.dword 1
.dword 0xC000000000000000, 0x3FE0000000000000

.text

#The following line initializes register x3 with 0x10000000 
#so that you can use x3 for referencing various memory locations. 
lui sp, 0x50
lui x3, 0x10
#your code starts here

addi s0, x3, 0x200  # s0 = where to put current number, 0x10000200 

ld s1, 0(x3)  # s1 = 11, exponent bits
ld s2, 8(x3)  # s2 = 52, mantissa bits


ld s3, 16(x3) # s3 = counter = number of pairs
addi x3, x3, 24  # x3 = 0x100000024, at first number


loop_main:
beq s3, x0, exit # exit when counter (s3) = 0
addi s3, s3, -1 

# comment

ld a0, 0(x3)    # num1
ld a1, 8(x3)    # num2 
addi a2, x0, 64  # width
addi a3, s2, 0  # mantissaWidth
addi a4, s1, 0  # expWidth
jal x1, fp64add
sd a0, 0(s0)
addi s0, s0, 8

ld a0, 0(x3)    # num1
ld a1, 8(x3)    # num2 
addi a2, x0, 64  # width
addi a3, s2, 0  # mantissaWidth
addi a4, s1, 0  # expWidth
addi a5, x0, 1023
jal x1, fp64mul
sd a0, 0(s0)
addi s0, s0, 8


addi x3, x3, 0x16
jal x0, loop_main



fp64add:
    addi sp, sp, -48
    sd s0, 40(sp)
    sd s1, 32(sp)
    sd s2, 24(sp)
    sd s3, 16(sp)
    sd s4,  8(sp)
    sd s5,  0(sp)
    
    add s0, a3, a4 # s0 = fW+eW
    sub s1, a2, s0 # s1 = W-(fW+eW)
    addi s2, a3, 1 # s2 = fW+1
    
    addi t6, x0, 1
    sll t6, t6, a3
    
    # a0 -> t0,t1,t2
    srl t0, a0, s0  # t0= a0 >> ((fW + eW) ~ 63 for double precision)
    
    sll t1, a0, s1  # t1= a0 << ((W -(fW+eW)) ~ 1 for double precision)
    srl t1, t1, s1  # shift to remove the leading values
    srl t1, t1, a3  # t1 = a0.exp
    
    sll t2, a0, a4
    sll t2, t2, s1
    srl t2, t2, s1
    srl t2, t2, a4
    or t2, t2, t6  # add implicit 1 to fraction
    
    # a1 -> t3,t4,t5
    srl t3, a1, s0  # t3= a1 >> ((fW + eW) ~ 63 for double precision)
    
    sll t4, a1, s1  # t4= a1 << ((W -(fW+eW)) ~ 1 for double precision)
    srl t4, t4, s1  # shift to remove the leading values
    srl t4, t4, a3  # t4 = a1.exp
    
    sll t5, a1, a4
    sll t5, t5, s1
    srl t5, t5, s1
    srl t5, t5, a4
    or t5, t5, t6  # add implicit 1 to fraction
    
    # t0=a0.sign
    # t1=a0.exponent
    # t2=a0.fraction
    
    # t3=a1.sign
    # t4=a1.exponent
    # t5=a1.fraction
    
    # check for special cases
    sll t6, t2, a4
    sll t6, t6, s1
    bne t1, x0, check_2
    bne t2, x0, check_2
    add a0, a1, x0
    jal x0, return_from_fp64add

    check_2:
    sll t6, t5, a4
    sll t6, t6, s1
    bne t4, x0, normal_add
    bne t5, x0, normal_add
    add a1, a0, x0
    jal x0, return_from_fp64add
    
    normal_add:
    
    bge t1, t4, align_mantissa_2 # align if exp different
    align_mantissa_1:
    sub t6, t4, t1
    bge t6, a3, make_zero_1 # check if shifting makes mantissa 0
    srl t2, t2, t6 # shift a0 mantissa by difference in exp
    addi t1, t4, 0
    jal x0, do_add
        
    
    align_mantissa_2:
    sub t6, t1, t4
    bge t6, a3, make_zero_2 # check if shifting makes mantissa 0
    srl t5, t5, t6 # shift a0 mantissa by difference in exp
    addi t4, t1, 0
    jal x0, do_add
    
    make_zero_1:
    addi t2, x0, 0
    addi t1, t4, 0
    jal x0, do_add
    
    make_zero_2:
    addi t4, x0 ,0
    addi t4, t1, 0
    jal x0, do_add
    
    do_add:
    beq t0, t3, add_a_b # if sign same do add
    bge t2, t5, sub_a_b # sign NOT same, a0.f > a1.f, DO a0.f - a1.f
    #if a0.f < a1.f do a1.f-a0.f
        
    sub_b_a:
    sub t2, t5, t2
    addi t0, t3, 0
    jal x0, do_normalize
        
    sub_a_b:
    sub t2, t2, t5
    jal x0, do_normalize
    
    add_a_b:
    add t2, t2, t5
    # go round
        
    do_normalize:
    srl t6, t2, a3
    srli t6, t6, 1
    beq t6, x0, no_right_normalize
    slli t6, t2, 63 # get the LSB of result fraction
    srli t2, t2, 1
    addi t1, t1, 1
    beq t6, x0, no_round
    addi t2, t2, 1
    jal x0, reassemble_result
    
    no_right_normalize:
    slli t6, t2, 1
    srl t6, t6, a3
    beq t6, x0, no_left_normalize
    slli t2, t2, 1
    addi t1, t1, -1
    
        
    
    
    
    no_left_normalize:
    
    no_normalize:
    no_round:
    reassemble_result:
    #reassemble result
    #remove implicit 1 from fraction
    sll a7, t2, a4
    sll a7, a7, s1
    srl a7, a7, s1
    srl a7, a7, a4
    # left shift exp by fracWidth
    sll t1, t1, a3
    # (shifted exp) or fraction
    or a7, a7, t1
    
    sll t0, t0, s0
    or a7, a7, t0
    add a0, a7, x0
    
    return_from_fp64add:
    ld s0, 40(sp)
    ld s1, 32(sp)
    ld s2, 24(sp)
    ld s3, 16(sp)
    ld s4,  8(sp)
    ld s5,  0(sp)
    addi sp, sp, 48
    jalr x0, 0(x1)
   

fp64mul:
    addi sp, sp, -48
    sd s0, 40(sp)
    sd s1, 32(sp)
    sd s2, 24(sp)
    sd s3, 16(sp)
    sd s4,  8(sp)
    sd s5,  0(sp)

    add s0, a3, a4 # s0 = fW+eW
    sub s1, a2, s0 # s1 = W-(fW+eW)
    addi s2, a3, 1 # s2 = fW+1
    
    addi t6, x0, 1
    sll t6, t6, a3
    
    # a0 -> t0,t1,t2
    srl t0, a0, s0  # t0= a0 >> ((fW + eW) ~ 63 for double precision)
    
    sll t1, a0, s1  # t1= a0 << ((W -(fW+eW)) ~ 1 for double precision)
    srl t1, t1, s1  # shift to remove the leading values
    srl t1, t1, a3  # t1 = a0.exp
    
    sll t2, a0, a4
    sll t2, t2, s1
    srl t2, t2, s1
    srl t2, t2, a4
    or t2, t2, t6  # add implicit 1 to fraction
    
    # a1 -> t3,t4,t5
    srl t3, a1, s0  # t3= a1 >> ((fW + eW) ~ 63 for double precision)
    
    sll t4, a1, s1  # t4= a1 << ((W -(fW+eW)) ~ 1 for double precision)
    srl t4, t4, s1  # shift to remove the leading values
    srl t4, t4, a3  # t4 = a1.exp
    
    sll t5, a1, a4
    sll t5, t5, s1
    srl t5, t5, s1
    srl t5, t5, a4
    or t5, t5, t6  # add implicit 1 to fraction
    
    # t0=a0.sign
    # t1=a0.exponent
    # t2=a0.fraction
    
    # t3=a1.sign
    # t4=a1.exponent
    # t5=a1.fraction
    
    # handle sign
    xor t0, t0, t3
    




    return_from_fp64mul:
    ld s0, 40(sp)
    ld s1, 32(sp)
    ld s2, 24(sp)
    ld s3, 16(sp)
    ld s4,  8(sp)
    ld s5,  0(sp)
    addi sp, sp, 48
    jalr x0, 0(x1)


dmult:
    addi sp, sp, -48
    sd s0, 40(sp)
    sd s1, 32(sp)
    sd s2, 24(sp)
    sd s3, 16(sp)
    sd s4,  8(sp)
    sd s5,  0(sp)
    
    addi s0, a0, 0
    addi s1, a1, 0
    addi a0, x0, 0
    
    beq s0, x0, return_zero_dmult
    beq s1, x0, return_zero_dmult
    
    loop_dmult:
    add a0, a0, s0
    addi s1, s1, -1
    bne s1, x0, loop_dmult
    jal x0, return_from_dmult
    
    return_zero_dmult:
    addi a0, x0, 0
    
    return_from_dmult:
    ld s0, 40(sp)
    ld s1, 32(sp)
    ld s2, 24(sp)
    ld s3, 16(sp)
    ld s4,  8(sp)
    ld s5,  0(sp)
    addi sp, sp, 48
    jalr x0, 0(x1)










exit:
    addi x0, x0, 0







#The final result should be in memory starting from address 0x10000200
#The first dword location at 0x10000200 contains sum of input11, input12
#The second dword location at 0x10000200 contains product of input11, input12
#The third dword location from 0x10000200 contains sum of input21, input22,
#The fourth dword location from 0x10000200 contains product of input21, input22, and so on.
