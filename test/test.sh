#!/bin/bash
echo "`pwd`"
echo `g++-4.9 -o ../build/maejs ../convert.cpp -static-libgcc -static-libstdc++ -static`
output="$(../build/maejs trash.aes memes.aes)"
echo "$output"
