LDR($PORTRW) 0b0011 ; Port A,B : Both RW 
LDR($PORTE)  0b0011 ; Enable Port A,B

MOV R2 , 0x00       ; Memory Index

L1:
    MOV R1 , 0x00       ; Page Index, RAM

    OneFrame:
        MOV R0 , 0b00100000 ; Port B data
        LDR($A) R1
        LDR($B) 0b11111
        ALUOUT(%AND) R1 ; R1 = R1 & 0b11111

        LDR($A) R0
        LDR($B) R1
        ALUOUT(%OR) R0 ; R0 = R0|R1

        LDR($A) R1
        LDR($B) R2
        ALUOUT(%ADD) R4 ; R4 = R1+R2

        MOV R3 , *R4    ; MOV R3 , [R4]

        LDR($A) R3
        LDR($B) 0xFAFA
        ALUTEST(%SUB)
        JMP(#Z) *ENDONEFRAME ; If memory data is 0xFAFA --> end of one frame

        LDR($B) 0xEDED
        ALUTEST(%SUB)
        JMP(#Z) *END         ; IF memory data is 0xEDED --> halt

        LDR($PORTA) 0x00
        OUT R3
        LDR($PORTA) 0x01
        OUT R0              ; Apply
        OUT 0x00

        LDR($A) R1
        LDR($B) 0x01
        ALUOUT(%ADD) R1 ; R1 += 1

        JMP(#J) *OneFrame

    ENDONEFRAME:
        LDR($A) R2
        LDR($B) 25
        ALUOUT(%ADD) R2

        JMP(#J) *L1

END:
    HLT
