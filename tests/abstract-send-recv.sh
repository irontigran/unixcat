#!/bin/sh

# shellcheck source=tests/test-lib.sh
. "$(dirname "$0")/test-lib.sh"

results=$(mktemp result.XXX)
trap 'rm -f $results' EXIT
socket="@abstract"

# Abstract socket basic send and receive tests; sending a message from a sender
# to listener should work.

./ucat -l "$socket" > "$results" &
pid=$!

message=hi
echo $message | ./ucat "$socket" || exit $hard_fail
wait "$pid" 2>/dev/null || exit $hard_fail

if [ -f "$socket" ]; then
    exit $fail
fi
check_exact_match $message "$results"
exit $?
