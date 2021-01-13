.global main
.text
main:
    @Test unitaire 
    @MOV
    MOV R0, #1 @MOV avec valeur, écrit 0x00000001 dans R0
    MOV R1 , R0 @MOV avec register , écrit contenu de R0(0x00000001) dans R1
    MOVS R1 , R0 @MOV avec flag S. Change CPSR si nécessaire
    @ADD
    ADD R1, R0 , #0x01 @ADD avec duex registers et valeur, écrit 0x00000002 (R0+0x01) dans R1
    ADD R1, R0 , R1 @ADD avec trois register, écrit 0x00000003 (R0+R1) dans R1
    ADD R1, R0 @ADD avec duex register , écrit 0x00000004 (R1+R0) dans R1
    ADD R1, #1 @ADD avec register et valeur, écrit 0x00000005 (R1+1) dans R1
    ADD R1, #0 @ADD avec flag S. Change CPSR si nécessaire
    @SUB
    SUB R2 , R1 ,#1 @SUB avec duex registers et valeur, écrit 0x00000004 (R1-1) dans R2
    SUB R2 , R1 ,R0 @SUB avec trois register, écrit 0x00000004 (R1-R0) dans R2
    SUB R2 , R0 @SUB avec duex register , écrit 0x00000003 (R2-R0) dans R2
    SUB R2 , #1 @SUB avec register et valeur, écrit 0x00000002 (R2-1) dans R2
    SUB R2 , #0 @SUB avec flag S. Change CPSR si nécessaire
    @AND
    AND R2 , R1 ,#0xf0 @AND avec duex registers et valeur, écrit 0x00000000 (R1&0xf0) dans R2
    AND R2 , R1 ,R0 @AND avec trois register, écrit 0x00000001 (R1&R0) dans R1
    ANDS R2 , R1 ,R0 @AND avec flag S. Change CPSR si nécessaire
    @ORR
    ORR R2 , R1 ,#0x2 @ORR avec duex registers et valeur, écrit 0x00000007 (R1|0x02) dans R2
    ORR R2 , R1 ,R0 @ORR avec trois register, écrit 0x00000005 (R1|R0) dans R1
    ORRS R2 , R1 ,R0 @ORR avec flag S. Change CPSR si nécessaire
    @EOR
    EOR R2 , R1 ,#1 @EOR avec duex registers et valeur, écrit 0x00000004 (R1^0x01) dans R2
    EOR R2 , R2 ,R0  @EOR avec trois register, écrit 0x00000005 (R2^R0) dans R1
    EORS R2 , R2 ,R0 @EOR avec flag S. Change CPSR si nécessaire
   	@RSB
    RSB R2 , R0 , #0x05 @RSB avec duex registers et valeur, écrit 0x00000004 (0x05-R0) dans R2
    RSB R2 , R0 , R2 @RSB avec trois register, écrit 0x00000003 (R2-R0) dans R2
    MOV R0,#6 @supp 
    RSB R2 , R0 @RSB avec duex register , écrit 0x00000003 (R2-R0) dans R2
    MOV R0,#1 @supp 
    RSB R2 , #5 @RSB avec register et valeur, écrit 0x00000002 (R2-1) dans R2
    RSBS R2 , #4 @RSB avec flag S. Change CPSR si nécessaire
   	@ADC est le même que ADD mais ajoute un 1 supplémentaire si le indicateur de carry du processeur est défini
    ADC R1, R0 , #0x01 @ADD avec duex registers et valeur, écrit 0x00000003 (R0+0x01)+C(1) dans R1
    ADC R1, R0 , R1 @ADD avec trois register, écrit 0x00000005 (R0+R1)+C(1) dans R1
    ADC R1, R0 @ADD avec duex register , écrit 0x00000007 (R1+R0)+C(1) dans R1
    ADC R1, #1 @ADD avec register et valeur, écrit 0x00000009 (R1+1)+C(1) dans R1
    ADCS R1, #0 @ADD avec flag S. Change CPSR si nécessaire
    MOV R1 , #5
    @SBC est le même que SUB mais réduire un 1  si le indicateur de carry du processeur est zero 
    SBC R2 , R1 ,#1 @SBC avec duex registers et valeur, écrit 0x00000003 (R1-1)-C(1) dans R2
    SBC R2 , R1 ,R0 @SBC avec trois register, écrit 0x00000003 (R1+R0)-C(1) dans R2
    SBC R2 , R0 @SBC avec duex register , écrit 0x00000001 (R2+R0)-C(1) dans R2
    SBC R2 , #1 @SBC avec register et valeur, écrit 0xFFFFFFFF (R2-1)-C(1) dans R2
    SBC R2 , #0 @SBC avec flag S. Change CPSR si nécessaire
    @RSC est le même que RSB mais réduire un 1  si le indicateur de carry du processeur est zero 
    RSC R2 , R0 , #0x05 @RSB avec duex registers et valeur, écrit 0x00000003 (0x05-R0)-C(1) dans R2
    RSC R2 , R0 , R2 @RSB avec trois register, écrit 0x00000001 (R2-R0)-C(1) dans R2
    MOV R0,#6 @supp 
    RSC R2 , R0 @RSB avec duex register , écrit 0x00000004 (R2-R0)-C(1) dans R2
    MOV R0,#1 @supp 
    RSC R2 , #5 @RSB avec register et valeur, écrit 0x00000000 (R2-1)-C(1) dans R2
    RSC R2 , #4 @RSB avec flag S. Change CPSR si nécessaire
 	  @BIC
    BIC R2 , R2 ,#1 @BIC avec duex registers et valeur, écrit  0x00000002 dans R2
    BIC R2 , R1 ,R0 @BIC avec trois register, écrit  0x00000004 dans R2
    @MVN
    MOV R1, #0 @supp
    MVN R2, R1 @ MVN avec duex register , écrit 0xFFFFFFFF (~R1) dans R2
    MVN R2, #0x00 @ MVN avec register et valeur, écrit 0xFFFFFFFF (~(0x00)) dans R2
    @TST
    MOV R1, #1 @supp
    TST R1 ,#1 @ R1&0x01
    TST R1 ,R1 @ R1&R1
    @CMN
    MOV R0,#1 @supp
    MOV R1,#0 @supp
    CMN R0, R1
    @CMP
    CMP R0, R1
    @TEQ
    TEQ R0 ,R1
    LDR R0 , =word32 @LDR pseudo-instruction R0 = &word32
    LDR R1 , [R0] @ R1 = *R0 (Mem read 4 bytes)
    STR R1 , [R0] @ *R0=R1 (Mem write 4 bytes)
    LDR R1 , [R0,#2] @ R1 = *(R0+2) (Mem read 4 bytes)
    STR R1 , [R0,#2] @*(R0+2)=R1 (Mem write 4 bytes)
    LDRB R1 , [R0] @  R1 = *R0 (Mem read 1 bytes)
    STRB R1 , [R0] @  *R0=R1 (Mem write 1 bytes)
    LDRB R1 , [R0,#2] @ R1 = *(R0+2) (Mem read 1 bytes)
    STRB R1 , [R0,#2] @ *(R0+2)=R1 (Mem write 1 bytes)
    LDRH R1 , [R0] @  R1 = *R0 (Mem read 2 bytes)
    STRH R1 , [R0] @  *R0=R1 (Mem write 2 bytes)
    LDRH R1 , [R0,#2] @ R1 = *(R0+2) (Mem read 2 bytes)
    STRH R1 , [R0,#2] @ *(R0+2)=R1 (Mem write 2 bytes)
    LDR R0, =src  
    LDR R1, =dest
    LDM R0!, {R2-R4} @R2 = *(R0+0) (Mem read 4 bytes) R3 = *(R0+4) (Mem read 4 bytes) R4 = *(R0+8) (Mem read 4 bytes)
    STM R1!, {R2-R4} @*(R0+0)=R2 (Mem write 4 bytes) *(R0+4)=R3 (Mem write 4 bytes) *(R0+8)=R4 (Mem write 4 bytes) 
    swi 0x123456
word32:
    .word 0x11223344
    .word 0x55667788
src: 
  .word 0x11111111
  .word 0x22222222
  .word 0x33333333  
dest: 
  .word 0x00000000
  .word 0x00000000
  .word 0x00000000