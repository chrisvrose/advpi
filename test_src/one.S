mov r0,#0x4000
nop
nop
nop
add r0,r0, #0x0004
mov r1,#0x1500
mov r2,#0x4049
str r2,[r0]
WFI
sub r15,r15,#28
