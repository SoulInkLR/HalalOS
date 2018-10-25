    .text
	.globl handler_horloge

	//traitant d'interuption horlge

handler_horloge:
	pusha
	call traitant_horloge
	popa
	iret
