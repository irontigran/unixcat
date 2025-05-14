#!/bin/sh

shellcheck ./*.sh

cppcheck --enable=all --std=c11 --check-level=exhaustive \
    -Iinclude -I. --suppress=missingIncludeSystem \
    src/*.c
