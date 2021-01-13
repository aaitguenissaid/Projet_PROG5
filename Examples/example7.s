.global main
.text
main:
    LDR r0, =word32 
    LDR r1, [r0] @R1=0x12345678
    LDRB r1, [r0] @R1=0x12
    LDRH r1, [r0] @R1=0x1234
    ADD r0, r0, #1
    LDRB r1, [r0]@R1=0x34
    ADD r0, r0, #1
    LDRB r1, [r0]@R1=0x56
    LDRH r1, [r0]@R1=0x5678
    ADD r0, r0, #1
    LDRB r1, [r0]@R1=0x78
    swi 0x123456
.data
word32:
    .word 0x12345678