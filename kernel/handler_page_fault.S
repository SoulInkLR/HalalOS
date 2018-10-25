
.text
	.globl handler_page_fault


handler_page_fault:


	pusha
	call traitant_page_fault
	popa

	ret