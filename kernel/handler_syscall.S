.text
	.globl handler_syscall
	#include "segment.h"
	
handler_syscall :

	pushw %fs
	pushw %es
	pushw %ds
	pushw %gs
	
	push %esi
	push %edx
	push %ecx
	push %ebx
	push %eax


	movw $KERNEL_DS, %cx
	movw %cx, %es
	movw %cx, %fs
	movw %cx, %gs
	movw %cx, %ds

	call select_call

	addl $4, %esp
	pop %ebx
	pop %ecx
	pop %edx
	pop %esi

	popw %gs
	popw %ds
	popw %es
	popw %fs



	iret
