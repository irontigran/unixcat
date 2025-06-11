#!/bin/sh

shellcheck -x ./*.sh ./tests/*.sh

cppcheck --enable=all --std=c11 --check-level=exhaustive \
    -Iinclude -I. --suppress=missingIncludeSystem \
    src/*.c src/creds/*.c src/printfd/*.c src/security/*.c
