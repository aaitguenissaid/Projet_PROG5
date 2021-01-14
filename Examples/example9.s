.global main
.text
main:
    LDR R0, =src  
    LDR R1, =dest
    LDM R0!, {r2-r4}    @ nous devrions voir trois nombres lus
    STM R1!, {r2-r4}    @ nous devrions voir un enregistrement de trois nombres
    LDR R1, =dest       @ vérifier que instruction est bien exécuter
    LDR r2, [r1] @ R2 = 0x11111111
    ADD r1, r1, #4
    LDR r2, [r1] @ R2 = 0x22222222
    ADD r1, r1, #4
    LDR r2, [r1] @ R2 = 0x33333333
    swi 0x123456
.data
src: 
	.word 0x11111111
	.word 0x22222222
	.word 0x33333333  
dest: 
	.word 0x00000000
	.word 0x00000000
	.word 0x00000000