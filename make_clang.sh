#!/bin/bash
dirs=$(grep -oPz '(?s)\[includedirs\].*?\[libdirs\]' build/conanbuildinfo.txt | grep -Eav 'includedirs|libdirs' | grep .)

printf "CompileFlags:
  Add: [-Wall -Wextra -std=gnu++20"
for d in $dirs; do
  printf " -I$d"
done
printf "]\n"