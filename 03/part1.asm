%define SYS_WRITE	1
%define SYS_READ	0
%define SYS_EXIT 	60
%define STDOUT 		1
%define STDIN 		0

%define BUFFER_SIZE 2097152 ; 2MiB
%define PADDING		4096

%macro CHECK_N 2 
	cmp byte [rsi + %1 + %2], '@'
	jne %%skip 
	inc rcx 
%%skip:
%endmacro

default rel
align 16

section .data

section .bss
	buffer  	resb BUFFER_SIZE + (PADDING * 2)
	num_buffer	resb 64

section .text
	global _start

_start:
	mov rax, SYS_READ 
	mov rdi, STDIN
	lea rsi, [buffer + PADDING]
	mov rdx, BUFFER_SIZE
	syscall

	test rax, rax 
	jle .exit_error

	; rsi: Current character pointer 
	; r14: End of buffer pointer 
	mov r14, rax 				; total bytes read 
	mov r15, rsi 				; start of data 
	add r14, r15 				; end of data 

	mov r8, rsi 

.find_width:
	cmp byte [r8], 0x0a 
	je .found_width 
	inc r8 
	jmp .find_width

.found_width:
	sub r8, rsi 
	inc r8
	mov r9, r8					; stride (width + \n)

	xor r12, r12				; total count

.grid_loop:
	cmp rsi, r14 
	jge .done 

	cmp byte [rsi], '@'
	jne .next_cell 

	xor rcx, rcx 				; neighbor count 
	mov r10, r9 				; stride 
	neg r10 					; -stride 

	CHECK_N r10, -1				; top left
	CHECK_N r10, 0 				; top mid 
	CHECK_N r10, 1				; top right 
	CHECK_N -1, 0				; left 
	CHECK_N 1, 0 				; right 
	CHECK_N r9, -1 				; bottom left 
	CHECK_N r9, 0 				; botttom mid 
	CHECK_N r9, 1 				; bottom right 

	cmp rcx, 4
	jge .next_cell

	inc r12 

.next_cell:
	inc rsi 
	jmp .grid_loop 

.done:
	; r12 = total sum 
	mov rax, r12
	lea rsi, [num_buffer + 63]	; build from end to start
	mov byte [rsi], 0x0a 
	dec rsi 

	test rax, rax 
	jnz .convert_loop 
	mov byte [rsi], 0x30 
	dec rsi 
	jmp .print_result 

.convert_loop:
	test rax, rax 
	je .print_result 

	xor rdx, rdx 
	mov rcx, 0x0a 
	div rcx 
	add dl, 0x30 
	mov [rsi], dl 
	dec rsi 
	jmp .convert_loop 

.print_result:
	inc rsi 

	lea rdx, [num_buffer + 64]
	sub rdx, rsi 
	mov rax, SYS_WRITE 
	mov rdi, STDOUT
	syscall 

.exit_success:
	mov rax, SYS_EXIT 
	xor rdi, rdi 
	syscall 

.exit_error:
	mov rax, SYS_EXIT 
	mov rdi, 1 
	syscall 

