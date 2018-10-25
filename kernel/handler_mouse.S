    .text
	.globl handler_mouse

handler_mouse:
	pusha
	call traitant_mouse
	popa
	iret