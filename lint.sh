#!/bin/sh

shellcheck ./*.sh

cppcheck src/*.c
