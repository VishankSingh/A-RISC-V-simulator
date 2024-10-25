label1: label3: add x0,0,zero       # error in this line due to 0 in add
    addi x0,x0,20478                # error here sue to imm > 2047
    add x0,x0, x0
    bge x0,x0,labe4                 # error here due to label not found
(label2:)
  ()()()  bge s9,x0,0b10
label4:       (x9)                  # error here due to register
    jalr x0, 0(x1)
    jal 0, label3                   # error here due to 0 not being register
   
   x0                               # error here due to standalone register
2label:                             # error here, label start with number           

not x1,x1
j x0, label4                        # error here due to 2 arguments given
jal x0, -16












