; just basic fibonacci program
MOV R0 , 0x00
MOV R1 , 0x01
MOV R2 , 0x00
L1:
    LDR($A) R0
    LDR($B) R1
    ALUOUT(%ADD) R2

    MOV R0 , R1
    MOV R1 , R2

    LDR($A) R1
    LDR($B) 1000
    ALUTEST(%SUB)

    JMP(#U) *DONE

    JMP(#J) *L1

DONE:
    HLT