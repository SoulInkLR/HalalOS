    .text
	.globl handler_clavier

	//traitant d'interuption du clavier

handler_clavier:
	pusha
	call traitant_clavier
	popa
	iret
