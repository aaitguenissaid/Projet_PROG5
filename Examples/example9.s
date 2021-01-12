.global main
.text
main:
    LDR R0, =src  
    LDR R1, =dest
    LDM R0!, {r2-r4}
    STM R1!, {r2-r4}
    LDR R1, =dest
    LDR r2, [r1]
    ADD r1, r1, #4
    LDR r2, [r1]
    ADD r1, r1, #4
    LDR r2, [r1]
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