#!/bin/sh

# shellcheck source=tests/test-lib.sh
. "$(dirname "$0")/test-lib.sh"

socket=$(mktemp -u sock.XXX)
results=$(mktemp result.XXX)
trap 'rm -f $socket $results' EXIT

# Basic send and receive tests; sending a message from a sender to listener
# should work.

./ucat -l "$socket" > "$results" &
pid=$!
check_listener_creation $pid "$socket" || exit $hard_fail

message=hi
echo $message | ./ucat "$socket" || exit $hard_fail
wait "$pid" 2>/dev/null || exit $hard_fail

check_exact_match $message "$results"
exit $?
