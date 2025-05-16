#!/bin/bash

socket=$(mktemp -u sock.XXX)
results=$(mktemp result.XXX)

clean_and_exit() {
    rm -f "$socket" "$results"
    exit "$1"
}

success=0
fail=1
hard_fail=99

(./ucat -l "$socket" > "$results" &) || clean_and_exit $hard_fail
expected="hi"
echo -n "$expected" | ./ucat "$socket" || clean_and_exit $hard_fail

stat=$success
r=$(cat "$results")
if [ "$r" != "$expected" ]; then
    echo "expected $expected, got $results"
    stat=$fail
fi
clean_and_exit $stat
