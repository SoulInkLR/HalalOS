.text

.globl start
start :
	push %ebx
	push %esi
	
	movl 12(%esp), %ebx
	movl 16(%esp), %ecx
	movl 20(%esp), %edx
	movl 24(%esp), %esi
	movl $1, %eax
	int $49

	pop %esi
	pop %ebx
	ret

	
.globl getpid
getpid :
	movl $2, %eax
	int $49
	ret

	
.globl getprio
getprio :
	push %ebx

	movl 8(%esp), %ebx
	movl $3, %eax

	int $49

	pop %ebx
	ret

	
.globl chprio
chprio :
	push %ebx

	movl 8(%esp), %ebx
	movl 12(%esp), %ecx
	movl $4, %eax
	int $49

	pop %ebx
	ret


.globl kill
kill :
	push %ebx

	movl 8(%esp), %ebx
	movl $5, %eax
	int $49

	pop %ebx
	ret

	
.globl waitpid
waitpid :
	push %ebx
	
	movl 8(%esp), %ebx
	movl 12(%esp), %ecx
	movl $6, %eax
	int $49

	pop %ebx
	ret

		
.globl exit
exit :
	push %ebx
	
	movl 8(%esp), %ebx
	movl $7, %eax
	int $49

	pop %ebx
	ret

	
.globl current_clock
current_clock : 
	movl $8, %eax
	int $49

	ret

.globl clock_settings
clock_settings :
	push %ebx	
	
	movl 8(%esp), %ebx
	movl 12(%esp), %ecx
	movl $9, %eax
	int $49

	pop %ebx
	ret
	
.globl wait_clock
wait_clock :
	push %ebx
	
	movl 8(%esp), %ebx
	movl $10, %eax
	int $49
	
	pop %ebx
	ret


.globl pcreate
pcreate :
	push %ebx
	
	movl 8(%esp), %ebx
	movl $11, %eax
	int $49

	pop %ebx
	ret

.globl pdelete
pdelete :
	push %ebx

	movl 8(%esp), %ebx
	movl $12, %eax
	int $49

	pop %ebx
	ret


.globl psend
psend :
	push %ebx

	movl 8(%esp), %ebx
	movl 12(%esp), %ecx
	movl $13, %eax
	int $49

	pop %ebx
	ret


.globl preceive
preceive :

	push %ebx
	
	movl 8(%esp), %ebx
	movl 12(%esp), %ecx
	movl $14, %eax
	int $49

	pop %ebx
	ret

.globl pcount
pcount : 
	push %ebx

	movl 8(%esp), %ebx
	movl 12(%esp), %ecx
	movl $15, %eax	
	int $49

	pop %ebx
	ret

.globl preset
preset :

	push %ebx

	movl 8(%esp), %ebx
	movl $16, %eax
	int $49

	pop %ebx
	ret

	
.globl cons_write
cons_write :

	push %ebx
	
	movl 8(%esp), %ebx  //const char
	movl 12(%esp), %ecx //length
	movl $17, %eax
	int $49

	pop %ebx
	ret

.globl cons_echo
cons_echo :

	push %ebx

	movl 8(%esp), %ebx
	movl 12(%esp), %ecx
	movl $18, %eax
	int $49

	pop %ebx
	ret
	


.globl cons_read
cons_read :

	push %ebx

	movl 8(%esp), %ebx
	movl 12(%esp), %ecx
	movl $19, %eax
	int $49

	pop %ebx
	ret
	
	

.globl shm_create
shm_create :

	push %ebx

	movl 8(%esp), %ebx
	movl $20, %eax
	int $49
	pop %ebx
	ret
	


.globl shm_acquire
shm_acquire :

	push %ebx

	movl 8(%esp), %ebx
	movl $21, %eax
	int $49

	pop %ebx
	ret
	
	


.globl shm_release
shm_release :

	push %ebx

	movl 8(%esp), %ebx
	movl $22, %eax
	int $49

	pop %ebx
	ret
	
.globl efface_ecran
efface_ecran :

	movl $23, %eax
	int $49
	ret

.globl funct_sinfo
funct_sinfo :

	push %ebx

	movl 8(%esp), %ebx
	movl $24, %eax
	int $49

	pop %ebx
	ret

.globl funct_ps
funct_ps :

	push %ebx

	movl 8(%esp), %ebx
	movl $25, %eax
	int $49

	pop %ebx
	ret

.globl display_banner
display_banner :


	movl $26, %eax
	int $49

	ret
	
.globl switch_display
switch_display :

	push %ebx

	movl 8(%esp), %ebx
	movl 12(%esp), %ecx
	movl $27, %eax
	int $49

	pop %ebx
	ret
	
	