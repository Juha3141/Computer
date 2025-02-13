; Port A : Screen Data
; Port B : Control signals for the screen

LDR($PORTRW) 0b0011 ; Port A,B : Both RW 
LDR($PORTE)  0b0011 ; Enable Port A,B

MOV R5 , 0x00       ; Address of the memory

L1:
     MOV R4 , 0b00100000 ; counter, counts the number of rows
     MOV R6 , 0x00
     L2:
          ; Read from the memory
          MOV R0 , *R5
          
          LDR($PORTA) 0x01   ; Port Address = 0x01 (PORT B)
          OUT R4 ; Row = 0, WriteIn = 1, DisplayOut = 0
          
          LDR($PORTA) 0x00   ; Port Address = 0x00 (PORT A)
          OUT R0
          
          ; Increment the row number
          LDR($A) R4
          LDR($B) 1
          ALUOUT(%ADD) R4
          
          LDR($A) R5
          LDR($B) 1
          ALUOUT(%ADD) R5
          
          LDR($A) R6
          LDR($B) R0
          ALUOUT(%ADD) R6
          
          LDR($A) R4
          LDR($B) 0b00111000
          ALUTEST(%SUB)
          JMP(#Z) *L3
          
          JMP(#J) *L2
L3:
     LDR($A) R6
     LDR($B) R6
     ALUTEST(%AND)
     JMP(#Z) *HALT
     
     ;;;;;;;;;; turn on the display out
     LDR($PORTA) 0x01
     OUT 0b01000000 ; Row = 0, WriteIn = 0, DisplayOut = 1

     JMP(#J) *L1

HALT:
     HLT
