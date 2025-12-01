%define SYS_WRITE	1
%define SYS_READ	0
%define SYS_OPEN 	2
%define SYS_CLOSE 	3
%define SYS_EXIT 	60

section .bss
	buffer: resb 65536	; for reading file into memory
	num_buffer: resb 64 	; for number to string conversion

section .text
	global _start

_start:
	pop rdi		; argc
	cmp rdi, 2	; if argc != 2
	jne .exit_err
	pop rsi		; program name
	pop rsi 	; argv[1]

.initialize:
	mov rbp, 50	; dial position (starts at 50)
	mov r12, 0	; zero counter

.open:
	; open(path, flag, mode)
	mov rax, SYS_OPEN
	mov rdi, rsi		; filename to rdi
	mov rsi, 0			; 0 = O_RDONLY
	syscall				; returns fd in rax
	cmp rax, -1
	je .exit_err
	mov rbx, rax		; save the fd to rbx

.read_buffer:
	; read(fd, buffer, count)
	mov rax, SYS_READ
	mov rdi, rbx 
	mov rsi, buffer 
	mov rdx, 65536
	syscall

	cmp rax, 0			; 0 == eof
	je .done_reading
	cmp rax, -1			; -1 == error
	je .err_close_fd

	mov r13, rax		; save amount of bytes read to r13
	xor r14, r14		; use r14 as index, set it to 0

.process:
	cmp r14, r13		; if we processed all bytes, read more
	jge .read_buffer

	movzx rax, byte [buffer + r14]	; load one byte to rax

	cmp rax, 0x4c 		; if byte is 'L'
	je .sign_negate
	cmp rax, 0x52		; if byte is 'R'
	je .sign
	cmp rax, 0x0a 		; if newline, skip 
	je .skip_newline

	jmp .err_close_fd

.skip_newline:
	inc r14 
	jmp .process

.parse_number:
	xor rcx, rcx		; use this to count the number

.digit_loop:
	movzx rax, byte [buffer + r14]
	cmp rax, 0x0a 		; '\n'
	je .apply_rotation	; if newline, done with reading the number

	cmp rax, 0x30 		; if byte less than '0', invalid
	jl .err_close_fd
	cmp rax, 0x39 		; if byte greater than '9', invalid 
	jg .err_close_fd

	sub rax, '0' 		; a to i 
	imul rcx, 10 		; rcx *= 10
	add rcx, rax 		; rcx += digit 

	inc r14 
	jmp .digit_loop 

.apply_rotation: 
	inc r14 			; skip '\n' for next line 

	; r15 = sign (-1 or 1) 
	; rcx = rotation 
	; rbp = current dial position 
	imul rcx, r15 		; rcx *= sign 
	add rbp, rcx 		; apply rotation 

	mov rax, rbp 
	cqo 
	mov rcx, 100
	idiv rcx 

	mov rbp, rdx

	cmp rbp, 0
	jge .check_zero
	add rbp, 100 		; if negative, wrap around

.check_zero: 
	cmp rbp, 0
	jne .loop_done
	inc r12 			; zero_counter += 1 

.loop_done:
	jmp .process 

.sign_negate:
	mov r15, -1
	inc r14				; move past the number 
	jmp .parse_number

.sign:
	mov r15, 1
	inc r14				; move past the number 
	jmp .parse_number	

.done_reading:
	mov rax, SYS_CLOSE 
	mov rdi, rbx 
	syscall 

	mov rax, r12 
	lea rsi, [num_buffer + 63] 	; build from end to start 
	mov byte [rsi], 10
	dec rsi 

	cmp rax, 0
	jne .convert_loop 
	mov byte [rsi], '0'
	dec rsi 
	jmp .print_result

.convert_loop:
	cmp rax, 0
	je .print_result 

	xor rdx, rdx 
	mov rcx, 10 
	div rcx 			; rax = rax / 10, rdx = remainder 
	add dl, '0' 		; convert to ascii
	mov [rsi], dl 		; store it 
	dec rsi 			; move ptr back
	jmp .convert_loop 
	
.print_result: 
	inc rsi 			; move forward one byte (we decremented one too many)

	lea rdx, [num_buffer + 64]
	sub rdx, rsi 		; rdx = length 
	mov rax, SYS_WRITE 
	mov rdi, 1 			; stdout 
	syscall

.exit_success:
	mov rax, SYS_EXIT
	xor rdi, rdi
	syscall
	
.err_close_fd:
	mov rax, SYS_CLOSE
	mov rdi, rbx
	syscall

.exit_err:
	mov rax, SYS_EXIT
	mov rdi, 1
	syscall
