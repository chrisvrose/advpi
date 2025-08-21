Vectors:
// Reset 0x0
NOP
// Undefined 0x4
NOP
// SWI 0x8
NOP
// p abort 0xc
NOP
// d abort 0x10
NOP
//something 0x14
NOP
// irq 0x18
LDR R15, =INTHANDLE
// fiq 0x1c
LDR R15, =FIQHANDLE
// noop sled in case I made a mistake
NOP
NOP
NOP
NOP
// 6000 is the generic catch handler
mov r7,#0x6000
mov r8,#0x1004
str r7,[r8]
// what the hell
WFI
sub r15,r15,#8

INTHANDLE:
mov r7,#0x4000
mov r8,#0x1004
str r7,[r8]
WFI
sub r15,r15,#8

FIQHANDLE:
mov r7,#0x3000
mov r8,#0x1004
str r7,[r8]
WFI
sub r15,r15,#8

.zero 1024
