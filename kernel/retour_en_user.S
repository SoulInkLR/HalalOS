
.text
	#include "segment.h"
	.globl retour_en_user
retour_en_user:

	movw $USER_DS, %cx
	movw %cx, %es
	movw %cx, %fs
	movw %cx, %gs
	movw %cx, %ds
	iret