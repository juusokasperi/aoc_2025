%define SYS_WRITE	1
%define SYS_READ	0
%define SYS_OPEN 	2
%define SYS_CLOSE 	3
%define SYS_EXIT 	60
%define BUFFER_SIZE 65536

section .bss
	buffer: resb BUFFER_SIZE	; for read
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
	mov rbp, 50		; dial position (starts at 50)
	xor r12, r12	; zero counter
	xor r13, r13	; total bytes in buffer 
	xor r14, r14 	; current buffer index 

.open:
	; open(path, flag, mode)
	mov rax, SYS_OPEN
	mov rdi, rsi		; filename to rdi
	xor rsi, rsi		; 0 = O_RDONLY
	syscall				; returns fd in rax
	cmp rax, -1
	je .exit_err
	mov rbx, rax		; save the fd to rbx

	lea r8, [.process]
	jmp .refill_buffer

.refill_buffer:
	push rcx			; Push rcx and r8 to stack for duration of syscall  
	push r8 

	sub r13, r14 		; total bytes in buffer - current index 
	mov r10, r13 

	cmp r10, 0 
	je .read_new_chunk

	mov rcx, r10 
	lea rsi, [buffer + r14]
	mov rdi, buffer 
	rep movsb			; copy r10 (remaining bytes) from rsi (buffer + index) to rdi (buffer)

.read_new_chunk:
	xor r14, r14

	mov rax, SYS_READ 
	mov rdi, rbx
	lea rsi, [buffer + r13]	; append to potential partial data
	mov rdx, BUFFER_SIZE
	sub rdx, r10 			; read BUFFER_SIZE - remaining bytes from last read 
	syscall 

	cmp rax, 0 
	je .pop_and_exit_eof
	cmp rax, -1
	je .pop_and_exit_err
	add r13, rax 
	pop r8					; where to jmp to 
	pop rcx 				; restore RCX (our accumulator)
	jmp r8

.pop_and_exit_eof:
	pop r8
	pop rcx 
	jmp .done_reading 

.pop_and_exit_err:
	pop r8 
	pop rcx 
	jmp .err_close_fd

.process:
	cmp r14, r13		; if we still have chars to read 
	jl .scan_char

	lea r8, [.process]
	jmp .refill_buffer

.scan_char:
	movzx rax, byte [buffer + r14]	; load one byte to rax

	cmp rax, 0x4c 		; if byte is 'L'
	je .sign_negate
	cmp rax, 0x52		; if byte is 'R'
	je .sign
	cmp rax, 0x0a 		; if newline, skip 
	je .skip_newline

	jmp .err_close_fd

.sign_negate:
	mov r15, -1
	inc r14				; move past the sign (L) 
	jmp .parse_number

.sign:
	mov r15, 1
	inc r14				; move past the sign (R)
	jmp .parse_number	

.skip_newline:
	inc r14 
	jmp .process

.parse_number:
	xor rcx, rcx		; use this to count the number

.digit_loop:
	cmp r14, r13
	jl .read_digit

	lea r8, [.digit_loop]
	jmp .refill_buffer 

.read_digit:
	movzx rax, byte [buffer + r14]
	cmp rax, 0x0a 		; '\n'
	je .apply_rotation	; if newline, done with reading the number

	cmp rax, 0x30 		; if byte less than '0', invalid
	jl .err_close_fd
	cmp rax, 0x39 		; if byte greater than '9', invalid 
	jg .err_close_fd

	sub rax, 0x30 		; a to i 
	imul rcx, 0x0a 		; rcx *= 10
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

.done_reading:
	mov rax, SYS_CLOSE 
	mov rdi, rbx 
	syscall 

	mov rax, r12 
	lea rsi, [num_buffer + 63] 	; build from end to start 
	mov byte [rsi], 0x0a
	dec rsi 

	cmp rax, 0
	jne .convert_loop 
	mov byte [rsi], 0x30		; '0'
	dec rsi 
	jmp .print_result

.convert_loop:
	cmp rax, 0
	je .print_result 

	xor rdx, rdx 
	mov rcx, 0x0a 
	div rcx 			; rax = rax / 10, rdx = remainder 
	add dl, 0x30 		; convert to ascii
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
