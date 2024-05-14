#!/bin/bash

echo "By Dreg, usage: ./rev_ft_prog_eeprom_dump.sh 0040 1003 1294 ...."
echo ""
echo "result:"

reverse_bytes() {
    local input=$1
    local length=${#input}
    local result=""

    for ((i=length-2; i>=0; i-=2)); do
        local byte=${input:$i:2}
        result="${result}${byte}"
    done

    echo $result
}

for number in $@; do
    reversed=$(reverse_bytes $number)
    echo -n "$reversed "
done
echo
