#!/bin/bash

socket=$(mktemp -u sock.XXX)
results=$(mktemp ucat.XXX)
fdfile=$(mktemp /tmp/file.XXX)

clean_and_exit() {
    rm -f "$socket" "$results" "$fdfile"
    exit "$1"
}

success=0
fail=1
hard_fail=99

(./ucat -l "$socket" > "$results" &) || clean_and_exit $hard_fail
echo -n "hi" | ./ucat --fd "$fdfile" "$socket" || clean_and_exit $hard_fail

expected="hi@ANC: SCM_RIGHTS $fdfile"
stat=$success
if [ "$(< "$results")" != "$expected" ]; then
    echo "expected $expected, got $results"
    stat=$fail
fi
clean_and_exit $stat
