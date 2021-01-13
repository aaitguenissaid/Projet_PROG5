.global main
.text
main:
    LDR r0, =word32 
    LDR r1, [r0] 
    LDRB r1, [r0] 
    LDRH r1, [r0] 
    ADD r0, r0, #1
    LDRB r1, [r0]
    ADD r0, r0, #1
    LDRB r1, [r0]
    LDRH r1, [r0]
    ADD r0, r0, #1
    LDRB r1, [r0]
    swi 0x123456
.data
word32:
    .word 0x12345678