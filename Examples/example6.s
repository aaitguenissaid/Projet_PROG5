.global main
.text
main:
	MOVEQ R0 , #0x01 @false for 000001D3
	MOVNE R0 , #0x01 @true for 000001D3
	MOVCS R0 , #0x01 @false for 000001D3
	MOVCC R0 , #0x01 @true for 000001D3
	MOVMI R0 , #0x01 @false for 000001D3
	MOVPL R0 , #0x01 @true for 000001D3
	MOVVS R0 , #0x01 @false for 000001D3
	MOVVC R0 , #0x01 @true for 000001D3
	MOVHI R0 , #0x01 @false for 000001D3
	MOVLS R0 , #0x01 @true for 000001D3
	MOVGE R0 , #0x01 @false for 000001D3
	MOVLT R0 , #0x01 @true for 000001D3
	MOVGT R0 , #0x01 @true for 000001D3
	MOVLE R0 , #0x01 @false for 000001D3
	MOVAL R0 , #0x01 @true for 000001D3
	MOV R0 , #0x01 @true for 000001D3
    swi 0x123456
