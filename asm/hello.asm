; Data section is to define variables
section .data   
hello: db 'Hello world!', 10
hellolen: equ $-hello

; Text section is where the code is set
section .text
    global _start

_start:
    mov eax, 4          ; write syscall must be in eac
    mov ebx, 1          ; file descriptor of STDOUT
    mov ecx, hello
    mov edx, hellolen

    int 80h             ; Call to kernel

    mov eax, 39
    mov ebx, 38
    add eax, ebx
    mov ecx, eax


; ------Terminate program ----------
    mov eax, 1
    mov ebx, 1
    int 80h


