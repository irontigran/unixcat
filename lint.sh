#!/bin/sh

shellcheck ./*.sh ./tests/*.sh

cppcheck --enable=all --std=c11 --check-level=exhaustive \
    -Iinclude -I. --suppress=missingIncludeSystem \
    src/*.c
