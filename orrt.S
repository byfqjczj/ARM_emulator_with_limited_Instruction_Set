.section .data						
friendly:
	.asciz "This is everyone's favorite test, right?\n \0"
return:
	.word 0xd65f03c0
num:
	.quad 0xFFFFFFFFFFFFFFFF

.section .text					
.global _start
_start:
	mov x2, #0xFFFF
orr x5, x2, #15	