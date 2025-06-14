#!/bin/sh

# shellcheck source=tests/test-lib.sh
. "$(dirname "$0")/test-lib.sh"

socket=$(mktemp -u sock.XXX)
results=$(mktemp result.XXX)
trap 'rm -f $socket $results' EXIT

# Basic sequenced packet send and receive test

./ucat -l --seq "$socket" > "$results" &
pid=$!
check_listener_creation $pid "$socket" || exit $hard_fail

message=hello_seq
echo $message | ./ucat --seq "$socket" || exit $hard_fail
wait "$pid" 2>/dev/null || exit $hard_fail

check_exact_match $message "$results"
exit $?