jmp(#j) *start

hello: 
    str "Hello, world!"

start:
    ldr($porte)  0b0000000000000100 ; Enable Port C
    ldr($portrw) 0b0000000000000100 ; Set Writable to Port C
    ldr($portaddr)  0x02 ; Port Address = Port C(0x02)

    ; Port C(TTY display), Bit 0~6: data, Bit 7: Clock, Bit 8: Write Enable, Bit 9: Clear
    out 0b1000000000

    mov r0 , hello ; r0: string pointer
    mov r2 , 0x00

    L1:
        ldrom r1 , *r0    ; load the string data from ROM
        
        ldr($a) r1
        ldr($b) 0
        alutest(%add)
        jmp(#z) *end      ; if r0 == 0 --> halt
        
        ldr($a) r1
        ldr($b) 0b0110000000
        aluout(%or) r1

        out r1
        ldr($a) r1
        ldr($b) 0b001111111
        aluout(%and) r1
        out r1

        ldr($a) r0
        ldr($b) 1
        aluout(%add) r0   ; r0 = r0+1

        jmp(#j) *L1

end:
    hlt
