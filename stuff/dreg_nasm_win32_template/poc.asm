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

add  esp, 4*6
push eax              ; Push address of FT_CreateDeviceInfoList

; [esp] FT_CreateDeviceInfoList address
; [esp+0x4] FTD2XX.dll base address
; [esp+0x8] ExitProcess
; [esp+0x0c] LoadLibraryA
; [esp+0x10] GetProcAddress
; [esp+0x14] kernel32 base address 


call [esp+0x8]        ; Call ExitProcess to end the program