#!/bin/bash

# before running this script, make sure you have a makefile and the you have run the isolate_folders.sh script
meta="meta4" # Change this to the meta folder you want to test
executable="./lexer" # Change this to your executable
count=0
passed=0
RED='\033[0;31m'
BLACK='\033[0m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
make
for file in "./c/$meta/"*; do
    rm -f $file/*.diff
    for file2 in $file/*c; do
        count=$((count+1))
        "$executable" < $file2 > $file/$(basename "${file}").result
        DIFF=$(diff $file/$(basename "${file}").result $file/$(basename "${file}").out)
        if [ "$DIFF" != "" ] 
        then
            diff $file/$(basename "${file}").result $file/$(basename "${file}").out >   $file/$(basename "${file}").diff
            echo -e -n "${RED}$((count)) "
            passed=$((passed+1))
        else
            echo -e -n "${GREEN}$((count)) "
        fi
        echo -e -n "${BLACK}"
        # echo $file2
        # diff $file/$(basename "${file}").result $file/$(basename "${file}").out
    done
done
echo ""
echo -e "${YELLOW}$((count-passed))/$count tests passes"