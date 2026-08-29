Vectors:
@ Reset 0x0
b starter
@ Undefined 0x4
NOP
@ SWI 0x8
NOP
@ p abort 0xc
NOP
@ d abort 0x10
NOP
@something 0x14
NOP
@ irq 0x18
LDR R15, =starter
@ fiq 0x1c
LDR R15, =starter
@ noop sled in case I made a mistake
NOP
NOP
NOP
NOP

@start func
starter:
.rept 10
mov r2, #0x5a5a
mov r5, #0x9000
str r2, [r5]
.endr


WFI
.short 0x5a2b
