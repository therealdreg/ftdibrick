# Contenido de break_on_int_80.gdb
break *0x08048080
commands
silent
x/i $eip
if (*(int *)($eip) == 0x80cd80)
    continue
end
