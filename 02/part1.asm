%define SYS_WRITE	1
%define SYS_READ	0
%define SYS_OPEN 	2
%define SYS_CLOSE 	3
%define SYS_LSEEK 	8
%define SYS_FSTAT	5
%define SYS_MMAP	9
%define SYS_EXIT 	60
%define STDOUT 		1
%define SEEK_END 	2

section .data

section .bss
	num_buffer resb 64

section .text
	global _start

_start:
	pop rsi
	cmp rsi, 2		; if argc != 2
	jne .exit_error
	pop rdi 		; program name (discard)
	pop rdi 		; input file 
	
	; -- Open file --
	mov rax, SYS_OPEN 
	mov rsi, 0
	xor rdx, rdx 
	syscall			; open

	test rax, rax
	js .exit_error 
	mov r15, rax 	; save FD in r15

	; -- Get file size (lseek
	mov rax, SYS_LSEEK
	mov rdi, r15
	mov rsi, 0
	mov rdx, SEEK_END
	syscall

	mov r14, rax	; r14 = filesize

	; -- Map file to memory (mmap) --
	mov rax, SYS_MMAP
	xor rdi, rdi				; address = null (os chooses)
	mov rsi, r14 				; length = file size 
	mov rdx, 1 					; prot_read
	mov r10, 2					; map_private
	mov r8, r15 				; fd
	xor r9, r9 					; offset = 0
	syscall

	test rax, rax 
	js .exit_close_fd

	mov rsi, rax 				; rsi = pointer to start of file data 
	add r14, rsi 				; r14 = pointer to end of file bfr 

	; --- Close file ---
	mov rax, SYS_CLOSE
	mov rdi, r15 
	syscall 

	; --- Puzzle logic ---
	; rsi: Current character pointer 
	; r14: End of buffer pointer 
	; r12: Total joltage sum
	xor r12, r12 				; sum = 0

.line_loop:
	cmp rsi, r14				; if at end 
	jge .done_reading

	xor r13, r13 				; maximum joltage for line 
	mov r10, rsi 				; inner loop pointer (start of line)

.find_max_loop_outer:
	mov al, byte [r10]			; move first char to al 
	cmp al, 0x0a				; check for newline
	je .line_done 
	cmp r10, r14 				; check for eof 
	jge .line_done_eof 

	sub al, '0'					; convert outer digit to int 
	movzx rdx, al				; rdx = first digit 

	; Inner loop: check digits after r10 
	mov r11, r10 
	inc r11 

.find_max_loop_inner:
	mov cl, byte [r11]
	cmp cl, 0x0a 				; check nl 
	je .next_outer_digit
	cmp r11, r14 				; check eof 
	jge .next_outer_digit 

	sub cl, '0'
	movzx rcx, cl 				; rcx = second digit 

	; calculate joltage (first * 10) + second
	mov rax, rdx 
	imul rax, 10 
	add rax, rcx 

	cmp rax, r13 
	cmovg r13, rax

	inc r11 
	jmp .find_max_loop_inner 

.next_outer_digit:
	inc r10 
	jmp .find_max_loop_outer

.line_done:
	add r12, r13				; add line to total sum 
	; Move rsi pointer to one character after r10 
	; since r10 is now sitting on a newline 
	lea rsi, [r10 + 1]
	jmp .line_loop

.line_done_eof:
	add r12, r13 

.done_reading:
	; r12 = total sum 
	mov rax, r12
	lea rsi, [num_buffer + 63]	; build from end to start
	mov byte [rsi], 0x0a 
	dec rsi 

	cmp rax, 0 
	jne .convert_loop 
	mov byte [rsi], 0x30 
	dec rsi 
	jmp .print_result 

.convert_loop:
	cmp rax, 0 
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

.exit_close_fd:
	mov rax, SYS_CLOSE 
	mov rdi, r15
	syscall 

.exit_error:
	mov rax, SYS_EXIT 
	mov rdi, 1 
	syscall 


	
