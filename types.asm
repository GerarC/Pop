BITS 64

;  ---------- Initializate program ----------
section .text
	global _start
print_int:
	mov	r9, -3689348814741910323
	sub	rsp, 40
	mov	BYTE [rsp+31], 10
	lea	rcx, [rsp+30]
.L2:
	mov	rax, rdi
	lea	r8, [rsp+32]
	mul	r9
	mov	rax, rdi
	sub	r8, rcx
	shr	rdx, 3
	lea	rsi, [rdx+rdx*4]
	add	rsi, rsi
	sub	rax, rsi
	add	eax, 48
	mov	BYTE [rcx], al
	mov	rax, rdi
	mov	rdi, rdx
	mov	rdx, rcx
	sub	rcx, 1
	cmp	rax, 9
	ja 	.L2
	lea	rax, [rsp+32]
	mov	edi, 1
	sub	rdx, rax
	xor	eax, eax
	lea	rsi, [rsp+32+rdx]
	mov	rdx, r8
	mov	rax, 1
	syscall
	add	rsp, 40
	ret

print_char:
	push	rdi
	mov	edi, 1
	mov	rax, 1
	mov	rdx, 1
	syscall
	add	rsp, 8
	ret

_start:
	push	rbp
	mov	rbp, rsp
	common	a 1:1
	mov	r8, 234
	mov	[a], r8
	common	b 8:8
	mov	r8, 116
	mov	[b], r8
	common	c 8:8
	mov	r8, 1
	mov	[c], r8
	mov	r8, 2
	mov	r9, 3
	mov	rcx, 0
	mov	rdx, 1
	cmp	r8, r9
	cmove	rcx, rdx
	mov	r8, rcx
	mov	[c], r8
	mov	r8, [a]
	mov	rdi, r8
	call	print_int
	mov	r8, [b]
	mov	rdi, r8
	call	print_int
	mov	r8, [c]
	mov	rdi, r8
	call	print_int

;  ---------- Terminate program ----------
	mov	eax, 1
	mov	ebx, 0
	syscall
