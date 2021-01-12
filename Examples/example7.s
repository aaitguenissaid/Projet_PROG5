.global main
.text
main:
    ldr r0, =word32
    ldr r1, [r0]
    ldrb r1, [r0]
    ldrh r1, [r0]
    add r0, r0, #1
    ldrb r1, [r0]
    add r0, r0, #1
    ldrb r1, [r0]
    ldrh r1, [r0]
    add r0, r0, #1
    ldrb r1, [r0]
    swi 0x123456
.data
word32:
    .word 0x12345678