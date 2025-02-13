jmp(#j) *start

label:

    str "Hello, world!"

start:
    ldr($port)
    mov r1 , label
    ldrom r0 , *r1