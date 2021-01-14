.global main
.text
main:
    LDR r0, =word32 
    LDR r1, [r0]    @devrait voir le entrée du  0x12345678 dans R1 
    LDRB r1, [r0]   @devrait voir le entrée du  0x12 dans R1
    LDRH r1, [r0]   @devrait voir le entrée du  0x1234 dans R1
    ADD r0, r0, #1
    LDRB r1, [r0]   @devrait voir le entrée du  0x34 dans R1
    ADD r0, r0, #1
    LDRB r1, [r0]   @devrait voir le entrée du  0x56 dans R1
    LDRH r1, [r0]   @devrait voir le entrée du  0x5678 dans R1
    ADD r0, r0, #1
    LDRB r1, [r0]   @devrait voir le entrée du  0x78 dans R1
    swi 0x123456
.data
word32:
    .word 0x12345678