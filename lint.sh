#!/bin/sh

shellcheck ./*.sh

cppcheck --std=c11 src/*.c
cppcheck --std=c11 include/*.h
