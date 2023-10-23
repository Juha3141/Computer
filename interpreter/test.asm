MOV R0 , 0x01
MOV R1 , R0
L1:
    LDR($A) R0
    LDR($B) R1
    ALUOUT(%ADD) R0

    JMP(#J) L1
    JMP(#J) 0x12
    JMP(#J) R5
    JMP(#J) *0x12