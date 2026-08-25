mov r0, #0x10000
mov r1,#0x1500
mov r2,#0x4500
ldr r3,[r2]
add r1,r2,r3
str r1,[r2]
WFI
