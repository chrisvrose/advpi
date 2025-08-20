mov r0,#0x4000

nop
nop
nop
nop
nop
add r0,r0, #0x0004
mov r1,#0x1500
mrs r2,cpsr
str r2,[r0]
WFI
sub r15,r15,#32

.ascii "MARK"
.nop 32
mov r2,#0x4345
str r2,[r0]
