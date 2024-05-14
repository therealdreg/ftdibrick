;The MIT License (MIT)

;Copyright (c) 2024 David Reguera Garcia aka Dreg & Ivan Redondo aka S0S4

;Permission is hereby granted, free of charge, to any person obtaining a copy
;of this software and associated documentation files (the "Software"), to deal
;in the Software without restriction, including without limitation the rights
;to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
;copies of the Software, and to permit persons to whom the Software is
;furnished to do so, subject to the following conditions:

;The above copyright notice and this permission notice shall be included in all
;copies or substantial portions of the Software.

;THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
;IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
;FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
;AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
;LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
;OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
;SOFTWARE.


; By: @S0S4 & Dreg
; github.com/therealdreg


; https://docs.freebsd.org/en/books/developers-handbook/x86/

; By default, the FreeBSD kernel uses the C calling convention.
; Further, although the kernel is accessed using int 80h,
; it is assumed the program will call a function that issues int 80h,
; rather than issuing int 80h directly.

; This convention is very convenient, and quite superior to the
; Microsoft convention used by MS-DOS
; Why? Because the UNIX convention allows any program written
; in any language to access the kernel.

; https://alfonsosiciliano.gitlab.io/posts/2023-08-28-freebsd-15-system-calls.html

; x86 nasm code

section .data
    msg     db      'Hello Dreg from 32 bit code FreeBSD!!', 0Ah
    hbytes	equ	$-msg

section .text
    global _start

_start:
    ; sys_write
    push dword hbytes
    push dword msg
    push dword 1
    mov eax, 4
    push eax ; garbage... freebsd call convention...
    int 0x80

    ; sys_exit
    push 0
    mov eax, 1
    push eax ; garbage...
    int 0x80


; ---- from freebsd doc:

; kernel:
;	int	80h	; Call kernel
;	ret
; open:
;	push	dword mode
;	push	dword flags
;	push	dword path
;	mov	eax, 5
;	call	kernel
;	add	esp, byte 12
;	ret
; This is a very clean and portable way of coding.
; If you need to port the code to a UNIX system which uses a
; different interrupt, or a different way of passing parameters,
; all you need to change is the kernel procedure.

; But assembly language programmers like to shave off cycles.
; The above example requires a call/ret combination.
; We can eliminate it by pushing an extra dword:

; open:
;	push	dword mode
;	push	dword flags
;	push	dword path
;	mov	eax, 5
;	push	eax		; Or any other dword
;	int	80h
;	add	esp, byte 16
; The 5 that we have placed in EAX identifies the kernel function, in this case open.
