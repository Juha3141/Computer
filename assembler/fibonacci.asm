; just basic fibonacci program
; set port
LDR($PORTA) 0x00
LDR($PORTRW) 0b0001 ; using port 1 as write port
LDR($PORTE)  0b0001 ; enable port 1
FIBONACCI:
    MOV R0 , 0x00
    MOV R1 , 0x01
    MOV R2 , 0x00

    MOV R3 , 0x00
L2:
    LDR($A) R0
    LDR($B) R1
    ALUOUT(%ADD) R2

    MOV R0 , R1
    MOV R1 , R2

    OUT R1
    
    MOV *R3 , R1
    LDR($A) R3
    LDR($B) 1
    ALUOUT($ADD) R3

    LDR($A) R1
    LDR($B) 0x7000
    ALUTEST(%SUB)

    JMP(#U) *END

    JMP(#J) *L2

END:
    HLT
