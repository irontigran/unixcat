#!/bin/sh

shellcheck ./*.sh

cppcheck --enable=all --std=c11 -Iinclude -I. --suppress=missingIncludeSystem \
    src/*.c
