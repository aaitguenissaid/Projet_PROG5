.global main
.text
main:
    ldr r0, =var1  
    ldr r1, =var2
    ldrb r2,[r0]    
    strb r2, [r1] @nous devrions voir le nombre 0x11 écrire en mémoire
    ldr r3,[r0] @ devrait voir le entrée du 0x11 dans R3
    ldrh r2,[r0]    
    strh r2, [r1] @nous devrions voir le nombre 0x1111 écrire en mémoire
    ldr r3,[r0] @ devrait voir le entrée du 0x1111 dans R3
    ldr r2, [r0]    
    str r2, [r1] @nous devrions voir le nombre 0x11111111 écrire en mémoire
    ldr r3,[r0] @ devrait voir le entrée du 0x11111111 dans R3
    swi 0x123456
.data
var1: 
	.word 0x11111111  
var2: 
	.word 0x00000000