.global main
.text
main:
	mov r0, #0x10
	mov r1, #0x20	
	add r3, r0, r1		@ write 0x30 to R3
	adc r3, r0 
	sub r3, r1, r0 		@ write 0x10 to R3
	rsb r3, r1, r0 		@ write 0xFFFFFFF0 to R3
	sub r3, r0, r1 		@ write 0xFFFFFFF0 to R3
	rsb r3, r0, r1 		@ write 0x10 to R3
	adc r3, r0, r1		@ write 0x31 to r3
	sbc r3, r1 ,r0		@ write 0x0F to r3
	rsc r3, r1, r0		@ write 0x0F to r3
	
	@mov r0, #0x0000FFFF
	@mov r1, #0xFFFF0000
	@mov r2, #0			
	@and r4, r0, r1		@ write 0 to r4
	@and r4, r0, r2 		@ write 0 to r4
	@orr r4, r0, r1 		@ write 0xFFFFFFFF to r4
	@orr r4, r0, r2 		@ write 0x0000FFFF to r4
	@bic r4, r0, r1		@ write 0x0000FFFF to r4
	@bic r4, r0, r0		@ write 0 to r4
	mov r2, #0
	mvn r5, r2 			@ write 0xFFFFFFFF to r5
	mov r2, #0x00000001
	
	cmn r5, r2			@ z = 1
	cmp r5, r2 			@ z = 0
	cmp r2, r2 			@ z = 1
	
	tst r2, #1			@ z = 1
	tst r2, r2 			@ z = 1
	
	sub r3, r2, r2
	TEQ r2, r2 			@ z = 0
 	TEQ r2, r3 			@ z = 1
 	swi 0x123456

