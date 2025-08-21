mov r0,#0x4000
mrs r2,cpsr
bic r2,#0b11000000
msr cpsr,r2

nop
nop
nop
nop
nop
mov r5,#0x0
SUBS r5,r5,#1
mov r2, #0x18
add r0,r0, #0x0004
ldr r3,[r2]
str r3,[r0]
ldr r4,[r3]
str r4,[r0]

WFI
sub r15,r15,#32

.ascii "MARK"
.nop 32
mov r2,#0x4345
str r2,[r0]
sub r15,r15,#16
