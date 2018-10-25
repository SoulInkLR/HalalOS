
.text
	.globl ctx_sw
	
// Structure de la pile en entree :
//   %esp + 4 : adresse de l'ancien contexte
//   %esp + 8 : adresse du nouveau contexte


ctx_sw:
   
    // Sauvegarde de l'ancien contexte => registres non-scratch
    // %ebx, %esi, %edi, %ebp, %esp, %cr3
    

	movl 4(%esp), %eax

	movl %esp, 0(%eax)
	movl %ebp, 4(%eax)
	movl %edi, 8(%eax)
	movl %esi, 12(%eax)
	movl %ebx, 16(%eax)
	movl %cr3, %ecx
	movl %ecx, 20(%eax)
    // Restauration du nouveau contexte 
	movl 8(%esp), %eax

	movl 0(%eax),  %esp
	movl 4(%eax),  %ebp
	movl 8(%eax),  %edi
	movl 12(%eax), %esi
	movl 16(%eax), %ebx
	movl 20(%eax), %ecx
	
	movl %ecx, 0x2001C	
	movl %ecx, %cr3
    // Activation du nouveau processus :
	
	
	ret
