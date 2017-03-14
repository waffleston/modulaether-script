#!/bin/bash
echo "`pwd`"
echo `g++-4.9 -o ../build/convert ../convert.cpp -static-libgcc -static-libstdc++ -static`
output="$(../build/convert trash.aes)"
echo "$output"
