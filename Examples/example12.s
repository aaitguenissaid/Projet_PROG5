.global main
.org 0x8
	B traitant_SWI
.text
main:
	swi 0x000000 @devrait lancer le traitant traitant_SWI
	swi 0x123456 @devrait mettre fin à l'exécution
traitant_SWI:
	mov r0, #5 @devrait mettre 5 dans r0 (pour s'assurer que le traitant s'exécute)
	movs PC,R14 @devrait renvoyer dans le main
.data
