#!/bin/sh

# shellcheck source=tests/test-lib.sh
. "$(dirname "$0")/test-lib.sh"

socket=$(mktemp -u sock.XXX)
results=$(mktemp result.XXX)
trap 'rm -f $socket $results' EXIT

# Test: specifying -S always but no -R option on Linux should result in no
# credentials being sent.

./ucat -l "$socket" > "$results" &
pid=$!
check_listener_creation $pid "$socket" || exit $hard_fail

printf "test1\ntest2\n" | ./ucat -S always "$socket" || exit $hard_fail
wait "$pid" 2>/dev/null || exit $hard_fail

check_exact_match "test1
test2" "$results"
exit $?
