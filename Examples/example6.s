.global main
.text
main:
	MOVEQ R0 , #0x01 @false for 000001D3 donc la valeur de R0 n'a pas changé
	MOVNE R0 , #0x01 @true for 000001D3 donc la valeur de P a changé
	MOVCS R0 , #0x01 @false for 000001D3 donc la valeur de P n'a pas changé
	MOVCC R0 , #0x01 @true for 000001D3 donc la valeur de P a changé
	MOVMI R0 , #0x01 @false for 000001D3 donc la valeur de P n'a pas changé
	MOVPL R0 , #0x01 @true for 000001D3 donc la valeur de P a changé
	MOVVS R0 , #0x01 @false for 000001D3 donc la valeur de P n'a pas changé
	MOVVC R0 , #0x01 @true for 000001D3 donc la valeur de P a changé
	MOVHI R0 , #0x01 @false for 000001D3 donc la valeur de P n'a pas changé
	MOVLS R0 , #0x01 @true for 000001D3 donc la valeur de P a changé
	MOVGE R0 , #0x01 @false for 000001D3 donc la valeur de P n'a pas changé
	MOVLT R0 , #0x01 @true for 000001D3 donc la valeur de P a changé
	MOVGT R0 , #0x01 @true for 000001D3 donc la valeur de P a changé
	MOVLE R0 , #0x01 @false for 000001D3 donc la valeur de P n'a pas changé
	MOVAL R0 , #0x01 @true for 000001D3 donc la valeur de P a changé
	MOV R0 , #0x01 @true for 000001D3 donc la valeur de P a changé
    swi 0x123456
