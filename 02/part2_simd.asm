%define SYS_WRITE	1
%define SYS_READ	0
%define SYS_OPEN 	2
%define SYS_CLOSE 	3
%define SYS_FSTAT 	5
%define SYS_LSEEK	8
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
	cmp rsi, 2 
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

	; -- Get file size (lseek)
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

	; -- Close file --
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

	; for part 2 we need to first find newline to calculate bounds
	mov r8, rsi 

.find_newline:
	cmp r8, r14					; check eof 
	jge .found_line_end 
	cmp byte [r8], 0x0a 		; check nl 
	je .found_line_end 
	inc r8 
	jmp .find_newline 

.found_line_end:
	; rsi = start of curr line 
	; r8 = address of nl / eof for this line 
	xor r13, r13 
	mov rcx, 12 				; looking for 12 digits 
	mov r9, rsi 				; search start pointer 

.digit_search_loop:
	mov rbx, r8 				; rbx = nl address 
	sub rbx, rcx 				; rbx = last valid index (nl - 12)

	xor rdx, rdx 				; rdx = best digit found so far 
	mov r10, r9 				; r10 = iterator
	mov r11, r9					; r11 = addr of best digit found 

	mov al, byte [r10]
	sub al, '0'
	movzx rdx, al
	inc r10 

.scan_candidates:
	mov rax, r10 
	add rax, 31
	cmp rax, rbx 
	jg .scalar_fallback 
	cmp rax, r14 
	jg .scalar_fallback 

	movzx eax, dl 
	add eax, '0'
	vmovd xmm1, eax
	vpbroadcastb ymm1, xmm1 
	
	vmovdqu ymm0, [r10]
	vpcmpgtb ymm2, ymm0, ymm1 

	vpmovmskb eax, ymm2 

	test eax, eax 
	jz .skip_32_bytes

	tzcnt eax, eax 
	add r10, rax 

	mov al, byte [r10]
	sub al, '0'
	movzx rdx, al
	mov r11, r10

	inc r10
	jmp .scan_candidates

.skip_32_bytes:
	add r10, 32 
	jmp .scan_candidates

.scalar_fallback:
	cmp r10, rbx 				; while r10 <= rbx 
	jg .found_max_digit 		; if i > limit, done 

	mov al, byte [r10]
	sub al, '0'
	cmp al, dl 					; if new digit <= oldmax, don't upd 
	jle .skip_update 
	movzx rdx, al				; upd max value 
	mov r11, r10 				; upd max addr 

.skip_update:
	inc r10 
	jmp .scalar_fallback 

.found_max_digit:
	imul r13, r13, 10 
	add r13, rdx

	mov r9, r11 				; upd search pointer 
	inc r9

	dec rcx 					; decrement digits needed ctr 
	jnz .digit_search_loop 

	add r12, r13 				; upd to total 

	lea rsi, [r8 + 1]
	jmp .line_loop 

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

