%define SYS_WRITE	1
%define SYS_READ	0
%define SYS_OPEN 	2
%define SYS_CLOSE 	3
%define SYS_FSTAT	5
%define SYS_MMAP	9
%define SYS_EXIT 	60

section .data

section .bss
	stat_buf resb 144			; buffer for fstat 

section .text
	global _start

_start:
	mov rax, 2
	mov rdi, filename 
