#!/bin/sh

socket=$(mktemp -u sock.XXX)
results=$(mktemp result.XXX)

clean_and_exit() {
    rm -f "$socket" "$results"
    exit "$1"
}

success=0
fail=1
hard_fail=99

(./ucat -lR once "$socket" > "$results" &) || clean_and_exit $hard_fail
echo "hi" | ./ucat "$socket" || clean_and_exit $hard_fail
expected="hi
@ANC: SCM_CREDS"
r=$(cat "$results")

case "$r" in
    "$expected"*) ;;
    *)
        echo "expected to start with $expected, actual was $r"
        clean_and_exit $fail
        ;;
esac

clean_and_exit $success

