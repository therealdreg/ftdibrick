@echo off
REM Eliminar archivos anteriores
del /f /q poc.o* poc.exe*

REM Compilar el archivo ASM
"C:\Program Files\NASM\nasm.exe" -f win32 "C:\Users\dreg\Desktop\ftdibrick-main\stuff\dreg_nasm_win32_template\poc.asm" -o "C:\Users\dreg\Desktop\ftdibrick-main\stuff\dreg_nasm_win32_template\poc.o"

REM Linkear el archivo objeto
"C:\Users\dreg\Downloads\MinGW 4.8.1\MinGW32\bin\ld.exe" -mi386pe poc.o -o poc.exe

echo Compilation and linking completed!
pause
