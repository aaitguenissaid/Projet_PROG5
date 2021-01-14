.global main
.org 0x8
	B traitant_SWI
.text
main:
	swi 0x000000
	swi 0x123456
traitant_SWI:
	mov r0, #5
	movs PC,R14
.data