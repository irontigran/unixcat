#!/bin/sh

# shellcheck source=tests/test-lib.sh
. "$(dirname "$0")/test-lib.sh"

socket=$(mktemp -u sock.XXX)
results=$(mktemp result.XXX)
trap 'rm -f $socket $results' EXIT

# Test credential passing over sequenced packet sockets

./ucat -l --seq -R always "$socket" > "$results" &
pid=$!
check_listener_creation $pid "$socket" || exit $hard_fail

printf "test1" | ./ucat --seq "$socket" || exit $hard_fail
wait "$pid" 2>/dev/null || exit $hard_fail

expected="test1@ANC: SCM_CRED*"
check_pattern "$expected" "$results"
exit $?
