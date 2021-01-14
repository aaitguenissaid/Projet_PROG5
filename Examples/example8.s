.global main
.text
main:
    ldr r0, =var1  
    ldr r1, =var2
    ldrb r2,[r0]    
    strb r2, [r1] @var2 = 0x11
    ldr r3 , =var2 @R3 =0x11
    ldrh r2,[r0]    
    strh r2, [r1] @var2 = 0x1111
    ldr r3 , =var2 @R3 =0x1111
    ldr r2, [r0]    
    str r2, [r1] @var2 = 0x11111111
    ldr r3 , =var2 @R3 =0x11111111
    swi 0x123456
.data
var1: 
	.word 0x11111111  
var2: 
	.word 0x00000000