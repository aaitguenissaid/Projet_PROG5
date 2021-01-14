.global main
.text
abs:
    cmp r0, #0
    bge .end
    mvn r1, r0
    add r0, r1, #1
.end:
    mov pc, lr
main:
    mov R0, #-5
    bl abs
    ldr r1,=result 
    str r0 ,[r1]  @ nous devrions voir le nombre (-5) écrire en mémoire
    swi 0x123456
.data
result: 
  .word 0x00000000